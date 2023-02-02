/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#include <cassert>
#include <cinttypes>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <numeric>
#include <sstream>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

// Struct to store temporary results of each worker thread.
struct ThreadResults {
    double m_iops;
    double m_throughput;
};

// Struct that stores the cumulative IOPS and throughput results (of all worker threads) of a given
// run.
struct MergedResults {
    uint32_t m_run_id;
    std::vector<double> m_iops;
    std::vector<double> m_throughput;
    double m_cumulative_iops;
    double m_cumulative_throughput;
};

// Struct that stores the average and standard deviation of IOPS and throughput results of all runs.
struct SetupResults {
    double m_avg_cumulative_iops;
    double m_stdev_cumulative_iops;
    double m_avg_cumulative_throughput;
    double m_stdev_cumulative_throughput;
};

/**
 * stress_test: continuously submit operation to the PADLL data plane stage in a close-loop.
 * @param fd file descriptor to store logging messages
 * @param pathname sample file to be opened
 * @param operation_size Size of the operation to be generated and submitted.
 * @param total_ops Number of operations to be submitted in the execution.
 * @param print_report Boolean that defines if the execution report is to be printed to the stdout.
 * @return Returns a ThreadResults object containing the performance results, namely IOPS and
 * throughput, of the execution.
 */
ThreadResults stress_test (FILE* fd,
    const std::string& pathname,
    const ssize_t& operation_size,
    const uint64_t& total_ops,
    bool print_report)
{
    // allocate memory for the request's buffer
    auto* message = new unsigned char[operation_size];
    if (operation_size == 0) {
        message = nullptr;
    }

    auto start = std::chrono::high_resolution_clock::now ();

    // cycle of syscall submission
    for (uint64_t i = 1; i <= total_ops; i++) {
        ::open (pathname.c_str (), O_RDONLY, 0666);
    }
    // calculate elapsed time
    auto end = std::chrono::high_resolution_clock::now ();
    std::chrono::duration<double> elapsed_seconds = end - start;

    // free message buffer
    delete[] message;

    // store performance results of the worker thread
    ThreadResults perf_result {};
    perf_result.m_iops = static_cast<double> (total_ops) / elapsed_seconds.count () / 1000;
    perf_result.m_throughput = (static_cast<double> (total_ops)
                                   * (static_cast<double> (operation_size) / 1024 / 1024 / 1024))
        / elapsed_seconds.count ();

    // print to stdout the execution report
    if (print_report) {
        std::fprintf (fd, "\n------------------------------------------------------------------\n");
        std::fprintf (fd, "\n PADLL || Scalability Test\n");
        std::fprintf (fd, "\n------------------------------------------------------------------\n");

        std::fprintf (fd,
            "Ops:\t%" PRIu64 "\t\tDuration:%f\n",
            total_ops,
            elapsed_seconds.count ());
        std::fprintf (fd,
            "IOPS:\t%lf.3 KOps/s\n",
            static_cast<double> (total_ops) / elapsed_seconds.count () / 1000);
        std::fprintf (fd, "------------------------------------------------------------------\n");

        std::fprintf (fd,
            "Thr:\t%lf.3 MiB/s\t%lf.3 GiB/s\n",
            (static_cast<double> (total_ops) * (static_cast<double> (operation_size) / 1024 / 1024))
                / elapsed_seconds.count (),
            (static_cast<double> (total_ops)
                * (static_cast<double> (operation_size) / 1024 / 1024 / 1024))
                / elapsed_seconds.count ());
        std::fprintf (fd,
            "Bw:\t%lf.3 MiB\t%lf.3 GiB\n",
            (static_cast<double> (total_ops)
                * (static_cast<double> (operation_size) / 1024 / 1024)),
            (static_cast<double> (total_ops)
                * (static_cast<double> (operation_size) / 1024 / 1024 / 1024)));
        std::fprintf (fd, "------------------------------------------------------------------\n\n");
    }

    return perf_result;
}

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
    results->m_throughput.emplace_back (threaded_results.m_throughput);
    results->m_cumulative_iops += threaded_results.m_iops;
    results->m_cumulative_throughput += threaded_results.m_throughput;
}

/**
 * stdout_results: Print performance report of the MergedResults object to a given file (including
 * stdout). If detailed flag is enabled, the method also logs the performance results (IOPS and
 * throughput) of each worker.
 * @param fd Pointer to a FILE object to write the performance report.
 * @param results Performance results to be logged.
 * @param print_detailed Flag that defines if the performance results of each worker thread should
 * be also printed to file.
 */
void log_results (FILE* fd, const MergedResults& merged_results, bool print_detailed)
{
    std::fprintf (fd, "Run: %u\n", merged_results.m_run_id);
    std::fprintf (fd, "\tIOPS (KOps/s):\t%.3lf\n", merged_results.m_cumulative_iops);
    std::fprintf (fd, "\tThr (GiB/s):\t%.3lf\n", merged_results.m_cumulative_throughput);
    std::fprintf (fd, "----------------------------------\n");

    // log performance results of each worker thread
    if (print_detailed) {
        for (unsigned int i = 0; i < merged_results.m_iops.size (); i++) {
            std::fprintf (fd, "Thread-%d:\t", i);
            std::fprintf (fd,
                "%.3lf KOps/s; %.3lf GiB/s\n",
                merged_results.m_iops[i],
                merged_results.m_throughput[i]);
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
    std::fprintf (fd, "\tThr (GiB/s):\t%.3lf\n", results.m_avg_cumulative_throughput);
    std::fprintf (fd, "\tstdev-iops:\t%.3lf\n", results.m_stdev_cumulative_iops);
    std::fprintf (fd, "\tstdev-thr:\t%.3lf\n", results.m_stdev_cumulative_throughput);
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
    double cumulative_throughput = 0;
    std::vector<double> iops_sample_stdev {};
    std::vector<double> throughput_sample_stdev {};

    // compute cumulative IOPS and throughput
    for (int i = 0; i < total_runs; i++) {
        cumulative_iops += results[i].m_cumulative_iops;
        cumulative_throughput += results[i].m_cumulative_throughput;
        iops_sample_stdev.push_back (results[i].m_cumulative_iops);
        throughput_sample_stdev.push_back (results[i].m_cumulative_throughput);
    }

    // compute average and standard deviation values and store them in the SetupResults object
    final_results.m_avg_cumulative_iops = (cumulative_iops / total_runs);
    final_results.m_avg_cumulative_throughput = (cumulative_throughput / total_runs);
    final_results.m_stdev_cumulative_iops = compute_stdev (iops_sample_stdev);
    final_results.m_stdev_cumulative_throughput = compute_stdev (throughput_sample_stdev);

    return final_results;
}

/**
 * execute_run: method for performing the actual operations over the data plane stage. The method
 * spawns as much threads as the number of channels (parameter), each of which invokes the
 * stress_test call and stores the results in a ThreadResults shared object.
 * @param fd Pointer to a FILE object to write the performance report.
 * @param run_id Unique identifier of the current run.
 * @param total_operations Total number of operations to be performed by each worker thread.
 * @param operation_size Size of each operation.
 * @return Returns a MergedResults object with the results of the stress test.
 */
MergedResults execute_run (FILE* fd,
    uint32_t run_id,
    uint32_t num_threads,
    const std::string& pathname,
    uint64_t total_ops,
    ssize_t op_size)
{
    // create object to store cumulative performance results
    MergedResults results {};
    results.m_run_id = run_id + 1;
    results.m_iops = {};
    results.m_throughput = {};
    results.m_cumulative_iops = { 0 };
    results.m_cumulative_throughput = { 0 };

    std::thread workers[num_threads];
    std::mutex lock;

    // lambda function for each thread to execute
    auto func = ([&lock, &results] (FILE* fd,
                     const std::string& pathname,
                     const ssize_t& op_size,
                     const long& total_ops,
                     bool print) {
        // execute stress test
        ThreadResults thread_results = stress_test (fd, pathname, op_size, total_ops, print);
        {
            std::unique_lock<std::mutex> unique_lock (lock);
            record_stress_test_results (&results, thread_results);
        }
    });

    // spawn worker threads
    for (unsigned int i = 1; i <= num_threads; i++) {
        workers[i - 1] = std::thread (func, fd, pathname, op_size, total_ops, false);
        std::cerr << "Starting worker thread #" << i << " (" << workers[i - 1].get_id () << ") ..."
                  << std::endl;
    }

    // join worker threads
    for (unsigned int i = 1; i <= num_threads; i++) {
        std::thread::id joining_thread_id = workers[i - 1].get_id ();
        workers[i - 1].join ();
        std::cerr << "Joining worker thread #" << i << " (" << joining_thread_id << ") ..."
                  << std::endl;
    }

    return results;
}

/**
 * trim: auxiliary method for trimming a string.
 * @param value String to be trimmed.
 * @return Returns the trimmed string.
 */
std::string trim (std::string value)
{
    unsigned int start = 0;

    while (start < value.size () && isspace (value[start])) {
        start++;
    }
    auto limit = static_cast<unsigned int> (value.size ());
    while (limit > start && isspace (value[limit - 1])) {
        limit--;
    }
    return { value.data () + start, limit - start };
}

/**
 * print_server_info: Print benchmark header message to a given file.
 * @param fd File descriptor to print to.
 */
void print_server_info (FILE* fd)
{
    std::stringstream time;
    const std::time_t t_c
        = std::chrono::system_clock::to_time_t (std::chrono::system_clock::now ());
    time << "Date:      " << std::put_time (std::localtime (&t_c), "%F %T");
    std::fprintf (fd, "%s\n", time.str ().c_str ());

#if defined(__linux)
    FILE* cpuinfo = fopen ("/proc/cpuinfo", "r");
    if (cpuinfo != nullptr) {
        char line[1000];
        int num_cpus = 0;
        std::string cpu_type;
        std::string cache_size;

        while (std::fgets (line, sizeof (line), cpuinfo) != nullptr) {
            const char* sep = std::strchr (line, ':');
            if (sep == nullptr) {
                continue;
            }

            std::string key = trim (std::string (line, sep - 1 - line));
            std::string val = trim (std::string (sep + 1, std::strlen (sep + 1)));
            if (key == "model name") {
                ++num_cpus;
                cpu_type = val;
            } else if (key == "cache size") {
                cache_size = val;
            }
        }

        std::fclose (cpuinfo);
        std::fprintf (fd, "CPU:       %d * %s\n", num_cpus, cpu_type.c_str ());
        std::fprintf (fd, "CPUCache:  %s\n", cache_size.c_str ());
        std::fprintf (fd, "------------------------------------\n");
    }
#endif
}

/**
 * Notes:
 *  - Options header:
 *    - (paio) option_default_context_type = ContextType::POSIX;
 *    - (paio) option_default_channel_differentiation_workflow = true
 *    - (paio) option_default_channel_differentiation_operation_type = false
 *    - (paio) option_default_channel_differentiation_operation_context = false
 * - Command:
 *  export padll_workflows=<total-workflows>; ./padll_scalability_bench <runs> <threads> <operations>
 */
int main (int argc, char** argv)
{

    // print header and node information to stdout
    print_server_info (stdout);

    if (argc < 4) {
        std::fprintf (stdout, "Error: missing arguments (runs -- threads -- ops) \n");
        return 1;
    } else {
        std::fprintf (stdout,
            "Executing %s: %s runs -- %s threads -- %s ops\n",
            argv[0],
            argv[1],
            argv[2],
            argv[3]);
    }

    uint32_t wait_time { 5 };
    bool store_run_perf_report { false };
    bool store_perf_report { true };
    std::string result_path { "/tmp/padll-scalability-results/" };
    std::string syscall_pathname { "/tmp/sample-file" };

    // benchmark setup
    std::vector<MergedResults> run_results;
    bool print_detailed = false;

    uint32_t num_runs { static_cast<uint32_t> (std::stoul (argv[1])) };
    uint32_t num_threads { static_cast<uint32_t> (std::stoul (argv[2])) };
    long num_ops { std::stol (argv[3]) };
    long operation_size { 0 };
    // int num_stages { std::stoi (argv[4]) };

    // create directory to store performance results
    if (store_perf_report && !result_path.empty ()) {
        fs::path path = result_path;

        // verify if directory already exists
        if (!fs::exists (path)) {
            bool mkdir_value = fs::create_directories (path);

            if (!mkdir_value) {
                std::cerr << "Error while creating " << path
                          << " directory: " << std::strerror (errno) << "\n";
            }
        }

        result_path = path.string ();
    }

    // File name
    fs::path filename;
    if (!result_path.empty ()) {
        filename = (result_path + "scale-perf-results-" + std::to_string (num_threads) + "-"
            + std::to_string (operation_size) + "-" + std::to_string (::getpid ()));
    }

    for (uint32_t run = 0; run < static_cast<uint32_t> (num_runs); run++) {
        FILE* fd_run_report;
        fs::path filename_run_perf_report = filename.string () + "-" + std::to_string (run + 1);

        // open file to store the performance report of the current run
        if (store_run_perf_report) {
            fd_run_report = std::fopen (filename_run_perf_report.string ().c_str (), "w");

            if (fd_run_report == nullptr) {
                std::cerr << "Error on open (" << filename_run_perf_report
                          << "): " << std::strerror (errno) << "\n";
                std::cerr << "Writing to stdout ..." << std::endl;
                fd_run_report = stdout;
            }
        } else {
            fd_run_report = stdout;
        }

        // execute run
        MergedResults results = execute_run (fd_run_report,
            run,
            static_cast<uint32_t> (num_threads),
            syscall_pathname,
            static_cast<uint64_t> (num_ops),
            static_cast<uint64_t> (operation_size));

        // log results to file or stdout
        log_results (fd_run_report, results, print_detailed);

        // store MergedResults object in container
        run_results.emplace_back (results);

        // close file descriptor of the performance report file of the current run
        if (store_run_perf_report) {
            int close_value = std::fclose (fd_run_report);

            if (close_value != 0) {
                std::cerr << "Error on close (" << filename_run_perf_report
                          << "): " << std::strerror (errno) << std::endl;
            }
        }

        // sleep before going to the next run
        std::this_thread::sleep_for (std::chrono::seconds (wait_time));
    }

    FILE* fd_perf_report;
    // merge final performance results
    SetupResults final_results = merge_final_results (run_results);

    // open file to store the final performance report (file or stdout)
    if (store_perf_report) {
        fd_perf_report = std::fopen (filename.string ().c_str (), "w");
        if (fd_perf_report == nullptr) {
            std::cerr << "Error on open (" << filename << "): " << std::strerror (errno) << "\n";
            std::cerr << "Writing to stdout ..." << std::endl;
            fd_perf_report = stdout;
        }
    } else {
        fd_perf_report = stdout;
    }

    // record final results in a given file (file or stdout)
    log_final_results (fd_perf_report, final_results, filename.string ());

    // close file descriptor of the performance report file
    if (store_perf_report) {
        int close_value = std::fclose (fd_perf_report);

        if (close_value != 0) {
            std::cerr << "Error on close (" << filename << "): " << std::strerror (errno)
                      << std::endl;
        }
    }
}
