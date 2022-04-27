#include <padll/options/options.hpp>
#include <paio/interface/paio_instance.hpp>
#include <paio/stage/paio_stage.hpp>
#include <random>

namespace padll::tests {

class StageIntegrationTest {

private:
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
     * generate_context_object: Generate a context object.
     */
    paio::core::Context generate_context_object (const int& workflow_range, const int& constant, const int& operation_type_range, const int& operation_context_range)
    {
        auto workflow_id = static_cast<long> (random () % workflow_range) * constant;
        auto operation_type = static_cast<int> (random () % operation_type_range);
        auto operation_context = static_cast<int> (random () % operation_context_range);

        return paio::core::Context { workflow_id, operation_type, operation_context, 1, 1 };
    }

    /**
     * submit_request:
     */
    void submit_request (int workflow_range, int constant, int operation_type_range, int operation_context_range, bool detailed_debug)
    {
        auto context_object = this->generate_context_object (workflow_range, constant, operation_type_range, operation_context_range);
        
        if (detailed_debug) {
            std::cout << context_object.to_string () << std::endl;
        }
        
        Result result {};
        this->m_instance_ptr->enforce (context_object, result);

        // validate ResultStatus
        if (result.get_result_status () != paio::enforcement::ResultStatus::success) {
            std::cerr << __func__  << ": enforce failed." << std::endl;
        }
        
        if (detailed_debug) {
            std::cout << "Result: " << result.to_string () << std::endl;
        }
    }

    /**
     * execute_worker:
     */
    void execute_worker (int worker_id, int iterations, int workflow_range, int constant, int operation_type_range, int operation_context_range, bool detailed_debug)
    {
        for (int i = 0; i < iterations; i++) {
            this->submit_request (workflow_range, constant, operation_type_range, operation_context_range, detailed_debug);
        }
    }

public:
    /**
     * StageIntegrationTest default constructor.
     */
    StageIntegrationTest () = default;

    /**
     * StageIntegrationTest parameterized constructor.
     * This constructor will set the PAIO environment variable 'paio_env' to 'env_value'.
     */
    explicit StageIntegrationTest (const std::string_view& env_value)
    {
        // set environment variable to paio_env
        this->set_env (options::option_paio_environment_variable_env, env_value);
    }

    /**
     * StageIntegrationTest default destructor.
     */
    ~StageIntegrationTest () = default;

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
        std::cout << this->m_stage_ptr->stage_info_to_string () << std::endl;
        std::cout << this->m_instance_ptr->to_string () << std::endl;
    }

    void spawn_workers (int num_workers, int iterations, int workflow_range, int constant, int operation_type_range, int operation_context_range, bool detailed_debug)
    {
        std::vector<std::thread> workers;
        for (int i = 0; i < num_workers; i++) {
            workers.emplace_back (std::thread (std::bind (&StageIntegrationTest::execute_worker, this, i, iterations, workflow_range, constant, operation_type_range, operation_context_range, detailed_debug)));
        }

        for (auto& worker : workers) {
            worker.join ();
        }
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
        + "hsk-rules-local" };
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
    StageIntegrationTest stage_test { stage_env_value };

    // initialize PAIO stage
    stage_test.initialize (num_channels,
        default_object_creation,
        stage_name,
        housekeeping_rules_path,
        differentiation_rules_path,
        enforcement_rules_path,
        execute_on_receive);

    std::this_thread::sleep_for (std::chrono::seconds (1));

    stage_test.test_to_string ();

    int num_workers = 1;
    int iterations = 10;
    int workflow_range = 4;
    int constant = 1000;
    int operation_type_range = paio::core::posix_size;
    int operation_context_range = paio::core::posix_meta_size;
    bool debug = false;

    stage_test.spawn_workers (num_workers, iterations, workflow_range, constant, operation_type_range, operation_context_range, debug);

    std::cout << "\n-------------------------------------------------------\n\n";

    return 0;
}