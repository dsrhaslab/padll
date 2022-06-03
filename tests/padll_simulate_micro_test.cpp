#include <cinttypes>
#include <cstdio>
#include <padll/options/options.hpp>
#include <paio/interface/paio_instance.hpp>
#include <paio/stage/paio_stage.hpp>
#include <random>

namespace padll::tests {

// Struct to store temporary results of each worker thread.
struct ThreadResults {
    double m_iops;
};

// Struct that stores the cumulative IOPS results (of all worker threads) of a given
// run.
struct MergedResults {
    uint32_t m_run_id;
    std::vector<double> m_iops;
    double m_cumulative_iops;
};

// Struct that stores the average and standard deviation of IOPS results of all runs.
struct SetupResults {
    double m_avg_cumulative_iops;
    double m_stdev_cumulative_iops;
};

/**
 * record_stress_test_results: Store the results of a single worker stress test in a MergedResults
 * object.
 * @param results Pointer to a shared MergedResults object, that contains the results of all
 * threads.
 * @param threaded_results Const reference to the results' object of the single worker stress test.
 */
void record_stress_test_results (MergedResults* results, const ThreadResults& threaded_results)
{
    results->m_iops.emplace_back (threaded_results.m_iops);
    results->m_cumulative_iops += threaded_results.m_iops;
}

/**
 * stdout_results: Print performance report of the MergedResults object to a given file (including
 * stdout). If detailed flag is enabled, the method also logs the performance results (IOPS) of each worker.
 * @param results Performance results to be logged.
 * @param print_detailed Flag that defines if the performance results of each worker thread should
 * be also printed to file.
 */
void log_results (FILE* fd, const MergedResults& merged_results, bool print_detailed)
{
    std::fprintf (fd, "Run: %u\n", merged_results.m_run_id);
    std::fprintf (fd, "\tIOPS (KOps/s):\t%.3lf\n", merged_results.m_cumulative_iops);
    std::fprintf (fd, "----------------------------------\n");

    // log performance results of each worker thread
    if (print_detailed) {
        for (unsigned int i = 0; i < merged_results.m_iops.size (); i++) {
            std::fprintf (fd, "Thread-%d:\t", i);
            std::fprintf (fd, "%.3lf KOps/s\n", merged_results.m_iops[i]);
        }
    }

    std::fflush (fd);
}

/**
 * log_final_results: Record the results of the overall execution (all runs).
 * @param fd Pointer to a FILE object to write the performance report.
 * @param results Performance results of the overall benchmark execution, which includes the average
 * of all runs.
 * @param setup_name Name of the benchmarking setup.
 */
void log_final_results (FILE* fd, const SetupResults& results, const std::string& setup_name)
{
    std::fprintf (fd, "----------------------------------\n");
    std::fprintf (fd, "Setup results: %s\n", setup_name.c_str ());
    std::fprintf (fd, "\tIOPS (KOps/s):\t%.3lf\n", results.m_avg_cumulative_iops);
    std::fprintf (fd, "\tstdev-iops:\t%.3lf\n", results.m_stdev_cumulative_iops);
    std::fprintf (fd, "----------------------------------\n");
}

/**
 * compute_stdev: Calculate standard deviation of a given sample.
 * @param sample Object that contains all values of the sample.
 * @return Returns the standard deviation value of the sample.
 */
double compute_stdev (const std::vector<double>& sample)
{
    double sum;
    double mean;
    double stdev = 0;

    int i;
    int sample_size = static_cast<int> (sample.size ());

    sum = std::accumulate (sample.begin (), sample.end (), 0.0);
    mean = sum / sample_size;

    for (i = 0; i < sample_size; i++)
        stdev += pow (sample[i] - mean, 2);

    return sqrt (stdev / sample_size);
}

/**
 * merge_final_results: Merge the performance results of all runs into a single object, which will
 * report the average and standard deviation values of the cumulative IOPS and throughput.
 * @param results Container with the performance results of all runs.
 * @return Returns a SetupResults object with the cumulative performance result of all runs.
 */
SetupResults merge_final_results (const std::vector<MergedResults>& results)
{
    SetupResults final_results {};
    int total_runs = static_cast<int> (results.size ());

    double cumulative_iops = 0;
    std::vector<double> iops_sample_stdev {};

    // compute cumulative IOPS
    for (int i = 0; i < total_runs; i++) {
        cumulative_iops += results[i].m_cumulative_iops;
        iops_sample_stdev.push_back (results[i].m_cumulative_iops);
    }

    // compute average and standard deviation values and store them in the SetupResults object
    final_results.m_avg_cumulative_iops = (cumulative_iops / total_runs);
    final_results.m_stdev_cumulative_iops = compute_stdev (iops_sample_stdev);

    return final_results;
}

class SimulateMicroTest {

private:
    FILE* m_fd { stdout };
    std::shared_ptr<paio::PaioStage> m_stage_ptr { nullptr };
    std::unique_ptr<paio::PaioInstance> m_instance_ptr { nullptr };

    /**
     * set_env: Set environment variable with name 'env_name' and value 'env_value'.
     * @param env_name Name of the environment variable.
     * @param env_value Value to be set in the environment variable.
     * @return Returns true if the environment variable was successfully set; false otherwise.
     */
    bool set_env (const std::string_view& env_name, const std::string_view& env_value)
    {
        if (env_name.empty () || env_value.empty ()) {
            auto message { __func__ + std::string (": env_name/env_value cannot be empty.") };
            throw std::invalid_argument (message);
        }

        auto return_value = ::setenv (env_name.data (), env_value.data (), 1);
        return (return_value == 0);
    }

    /**
     * submit_request:
     * @param workflow_id
     * @param operation_type
     * @param operation_context
     * @param detailed_debug
     */
    void submit_request (long workflow_id, int operation_type, int operation_context, bool detailed_debug)
    {
        // generate Context object
        paio::core::Context context_object { workflow_id, operation_type, operation_context, 1, 1 };

        if (detailed_debug) {
            std::fprintf (this->m_fd, "%s\n", context_object.to_string ().c_str ());
        }
        
        // create Result object
        Result result {};
        // enforce request through the PaioInstance
        this->m_instance_ptr->enforce (context_object, result);

        // validate ResultStatus
        if (result.get_result_status () != paio::enforcement::ResultStatus::success) {
            std::fprintf (stderr, "%s: enforce failed.\n", __func__);
        }
        
        // log Result object
        if (detailed_debug) {
            std::fprintf (this->m_fd, "Result: %s\n", result.to_string ().c_str ());
        }
    }

    /**
     * spawn_worker:
     * @param worker_id
     * @param iterations
     * @param workflow_id
     * @param operation_type
     * @param operation_context
     * @param detailed_debug
     */
    [[nodiscard]] ThreadResults spawn_worker (int iterations, long workflow_id, int operation_type, int operation_context, bool detailed_debug)
    {
        auto start = std::chrono::high_resolution_clock::now ();

        for (int i = 0; i < iterations; i++) {
            this->submit_request (workflow_id, operation_type, operation_context, detailed_debug);
        }

        // calculate elapsed time
        auto end = std::chrono::high_resolution_clock::now ();
        std::chrono::duration<double> elapsed_seconds = end - start;

        // store performance results of the worker thread
        ThreadResults results { static_cast<double> (iterations) / elapsed_seconds.count () / 1000 };
        return results;
    }

public:
    /**
     * SimulateMicroTest default constructor.
     */
    SimulateMicroTest () = default;

    /**
     * SimulateMicroTest parameterized constructor.
     * This constructor will set the PAIO environment variable 'paio_env' to 'env_value'.
     */
    explicit SimulateMicroTest (const std::string_view& env_value)
    {
        // set environment variable to paio_env
        this->set_env (options::option_paio_environment_variable_env, env_value);
    }

    /**
     * SimulateMicroTest default destructor.
     */
    ~SimulateMicroTest () = default;

    /**
     * initialize: Initialize the PAIO data plane stage.
     */
    void initialize (const int& num_channels,
        const bool& object_creation,
        const std::string& stage_name,
        const std::string& hsk_file,
        const std::string& dif_file,
        const std::string& enf_file,
        const bool& execute_on_receive)
    {
        std::cout << "\n\n-------------------------------------------------------\n";
        // create shared ptr to a PaioStage
        this->m_stage_ptr = std::make_shared<paio::PaioStage> (num_channels,
            object_creation,
            stage_name,
            hsk_file,
            dif_file,
            enf_file,
            execute_on_receive);

        this->m_instance_ptr = std::make_unique<paio::PaioInstance> (this->m_stage_ptr);
    }

    /**
     * test_to_string: write to stdout StageInfo in string-based format of the stage_ptr object.
     */
    void test_to_string ()
    {
        std::cout << "\n-------------------------------------------------------" << std::endl;
        std::cout << "Print StageInfo and PaioInstance content:" << std::endl;
        std::cout << "-------------------------------------------------------" << std::endl;
        std::cout << this->m_stage_ptr->stage_info_to_string () << std::endl;
        std::cout << this->m_instance_ptr->to_string () << std::endl;
        std::cout << "-------------------------------------------------------\n" << std::endl;
    }

    /**
     * execute_job: 
     * @param run_id Unique identifier of the current run.
     * ...
     * @return Returns a MergedResults object with the results of the stress test.
     */
    MergedResults execute_job (int run_id, int num_workers, int iterations, long workflow_id, int operation_type, int operation_context, bool detailed_debug)
    {
        // create object to store cumulative performance results
        MergedResults results { run_id, {}, 0 };

        std::mutex lock;

        // lambda function to execute worker
        auto func = ([&lock,&results, this, workflow_id, operation_type, operation_context, detailed_debug] (int iterations) {
            
            // execute stress test for worker
            ThreadResults thread_results = this->spawn_worker (iterations, workflow_id, operation_type, operation_context, detailed_debug);
            
            {
                std::unique_lock<std::mutex> unique_lock (lock);
                record_stress_test_results (&results, thread_results);
            }

        });

        int per_worker_iterations = iterations / num_workers;

        // spawn workers
        std::vector<std::thread> workers;
        for (int i = 0; i < num_workers; i++) {
            // spawn and emplace thread
            workers.emplace_back (func, per_worker_iterations);
            std::cerr << "Starting worker thread #" << i << " (" << workers[i].get_id () << ") ..." << std::endl;
        }

        // join workers
        for (auto& worker : workers) {
            std::thread::id joining_thread_id = worker.get_id ();
            // join worker thread
            worker.join ();
            std::cerr << "Joining worker thread #" << joining_thread_id << " ..." << std::endl;
        }

        return results;
    }

};
} // namespace padll::tests

using namespace padll::tests;
using namespace padll::options;

void print_header ()
{
    std::this_thread::sleep_for (std::chrono::seconds (1));
    std::cout << "\n\n-------------------------------------------------------\n";
    std::cout << "PADLL - PAIO Data Plane Stage Integration Test\n";
    std::cout << "-------------------------------------------------------\n";
}

int main ()
{

    // set stage name
    std::string stage_name { padll::options::option_default_stage_name };
    // set channel creation options
    int num_channels { padll::options::option_default_stage_channels };
    bool default_object_creation { padll::options::option_default_stage_object_creation };
    // set path to rules files
    std::string housekeeping_rules_path { padll::options::main_path ().string ()
        + "hsk-micro-2" };
    std::string differentiation_rules_path { "" };
    std::string enforcement_rules_path { "" };
    // set rule execution options
    bool execute_on_receive { true };
    // set environment variables for the stage
    std::string_view stage_env { padll::options::option_paio_environment_variable_env };
    std::string_view stage_env_value { "testing-environment" };

    // print header to stdout
    print_header ();

    // create testing object
    SimulateMicroTest stage_test { stage_env_value };

    // initialize PAIO stage
    stage_test.initialize (num_channels,
        default_object_creation,
        stage_name,
        housekeeping_rules_path,
        differentiation_rules_path,
        enforcement_rules_path,
        execute_on_receive);

    // std::this_thread::sleep_for (std::chrono::seconds (1));

    // check content in PaioStage's StageInfo and PaioInstance
    stage_test.test_to_string ();
    
    // benchmark setup
    std::vector<MergedResults> run_results;
    int num_workers { 1 };
    int iterations { 10000000 };
    long workflow_id { 1000 };
    int operation_type { static_cast<int> (paio::core::POSIX::close) };
    int operation_context { static_cast<int> (paio::core::POSIX_META::meta_op) };
    bool debug { false };
    FILE* fd { stdout };
    uint32_t runs = 1;
    int wait_time = 5;

    for (uint32_t i = 1; i <= runs; i++) {
        // execute test
        auto results = stage_test.execute_job (i, num_workers, iterations, workflow_id, operation_type, operation_context, debug);

        // log results to file or stdout
        log_results (fd, results, debug);
        // store MergedResults object in container
        run_results.emplace_back (results);
        
        // sleep before going to the next run
        std::this_thread::sleep_for (std::chrono::seconds (wait_time));
    }

    // merge final performance results
    SetupResults final_results = merge_final_results (run_results);
    // record final results in a given file (file or stdout)
    log_final_results (fd, final_results, "PADLL::PAIO microbenchmark simulation test");

    std::cout << "\n-------------------------------------------------------\n\n";

    return 0;
}