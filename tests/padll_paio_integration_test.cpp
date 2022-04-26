#include <paio/interface/paio_instance.hpp>
#include <paio/stage/paio_stage.hpp>
#include <padll/options/options.hpp>

namespace padll::tests {

class StageIntegrationTest {

public:
    StageIntegrationTest () = default;

    ~StageIntegrationTest () = default;

    std::shared_ptr<paio::PaioStage> initialize (const int& num_channels, 
        const bool& object_creation, 
        const std::string& stage_name,
        const std::string& hsk_file,
        const std::string& dif_file,
        const std::string& enf_file,
        const bool& execute_on_receive) 
    {
        std::cout << "\n\n-------------------------------------------------------\n";
        return std::make_shared<paio::PaioStage> (num_channels, object_creation, stage_name, hsk_file, dif_file, enf_file, execute_on_receive);
    }


    void test_stage_info_to_string (std::shared_ptr<paio::PaioStage> stage_ptr) {
        std::cout << stage_ptr->stage_info_to_string () << std::endl;
    }
    
    /**
     * set_env: Set environment variable with name 'env_name' and value 'env_value'.
     * @param env_name Name of the environment variable.
     * @param env_value Value to be set in the environment variable.
     * @return Returns true if the environment variable was successfully set; false otherwise.
     */
    bool set_env (const std::string_view& env_name, const std::string_view& env_value)
    {
        auto return_value = ::setenv (env_name.data (), env_value.data (), 1);
        return (return_value == 0);
    }

};
}

using namespace padll::tests;
using namespace padll::options;

int main () {

    // set stage name
    std::string stage_name { padll::options::option_default_stage_name };
    // set channel creation options
    int num_channels { padll::options::option_default_stage_channels };
    bool default_object_creation { padll::options::option_default_stage_object_creation };
    // set path to rules files
    std::string housekeeping_rules_path { padll::options::main_path ().string () + "hsk-rules-local" };
    std::string differentiation_rules_path { "" };
    std::string enforcement_rules_path { "" };
    // set rule execution options
    bool execute_on_receive { true };
    // set environment variables for the stage
    std::string_view stage_env { padll::options::option_paio_environment_variable_env };
    std::string_view stage_env_value { "testing-environment" };
    
    std::cout << "-------------------------------------------------------\n\n";

    StageIntegrationTest test {};
    std::this_thread::sleep_for (std::chrono::seconds (2));

    
    test.set_env (stage_env, stage_env_value);

    std::shared_ptr<paio::PaioStage> stage_ptr = test.initialize (num_channels, default_object_creation, stage_name, housekeeping_rules_path, differentiation_rules_path, enforcement_rules_path, execute_on_receive);

    std::this_thread::sleep_for (std::chrono::seconds (1));

    test.test_stage_info_to_string (stage_ptr);

    std::unique_ptr<paio::PaioInstance> instance_ptr = std::make_unique<paio::PaioInstance> (stage_ptr);



    std::cout << "\n-------------------------------------------------------\n\n";

    return 0;
}