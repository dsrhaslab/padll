/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#include <padll/stage/data_plane_stage.hpp>

namespace padll::stage {

// DataPlaneStage default constructor.
DataPlaneStage::DataPlaneStage () :
    m_log { std::make_shared<Log> (option_default_enable_debug_level,
        option_default_enable_debug_with_ld_preload,
        std::string { option_default_log_path }) },
    m_stage { std::make_shared<paio::PaioStage> () }
{
    // create logging message
    std::stringstream stream;
    stream << "DataPlaneStage initialized with default values ";
    stream << "(" << static_cast<void*> (this->m_log.get ()) << ")";

    // write debug logging message
    this->m_log->log_info (stream.str ());

    // set initialization status to true (stage is ready to receive requests)
    this->set_stage_initialized (true);
}

// DataPlaneStage parameterized constructor.
DataPlaneStage::DataPlaneStage (std::shared_ptr<Log> log_ptr,
    const int& num_channels,
    const bool& default_object_creation,
    const std::string& stage_name,
    const std::string& hsk_rules_path,
    const std::string& dif_rules_path,
    const std::string& enf_rules_path,
    const bool& execute_on_receive) :
    m_log { log_ptr }
{
    // create logging message
    std::stringstream stream;
    stream << "DataPlaneStage parameterized constructor [w/o controller] ";
    stream << "(" << static_cast<void*> (this->m_log.get ()) << ")";

    // unique_lock over mutex
    std::unique_lock lock (this->m_lock);

    // initialize local data plane stage
    this->m_stage = { std::make_shared<paio::PaioStage> (num_channels,
        default_object_creation,
        stage_name,
        hsk_rules_path,
        dif_rules_path,
        enf_rules_path,
        execute_on_receive) 
    };
    
    // initialize PosixLayer instance
    this->m_posix_instance = std::make_unique<paio::PosixLayer> (this->m_stage);

    // set initialization status to true (stage is ready to receive requests)
    this->set_stage_initialized (true);

    // write debug logging message
    this->m_log->log_info (stream.str ());
}

// DataPlaneStage parameterized constructor.
DataPlaneStage::DataPlaneStage (std::shared_ptr<Log> log_ptr,
    const int& num_channels,
    const bool& default_object_creation,
    const std::string& stage_name) :
    m_log { log_ptr }
{
    // create logging message
    std::stringstream stream;
    stream << "DataPlaneStage parameterized constructor [w/ controller] ";
    stream << "(" << static_cast<void*> (this->m_log.get ()) << ")";

    // unique_lock over mutex
    std::unique_lock lock (this->m_lock);

    // initialize data plane stage that connects to local controller
    this->m_stage = { std::make_shared<paio::PaioStage> (num_channels, 
        default_object_creation, 
        stage_name, 
        this->m_communication_type, 
        this->m_local_controller_address, 
        this->m_local_controller_port) 
    };

    // initialize PosixLayer instance
    this->m_posix_instance = std::make_unique<paio::PosixLayer> (this->m_stage);

    // set initialization status to true (stage is ready to receive requests)
    this->set_stage_initialized (true);

    // write debug logging message
    this->m_log->log_info (stream.str ());
}

// DataPlaneStage default destructor.
DataPlaneStage::~DataPlaneStage ()
{
    this->m_log->log_info ("DataPlaneStage destructor.\n");
}

// set_stage_initialized call. (...)
void DataPlaneStage::set_stage_initialized (const bool& status)
{
    this->m_stage_initialized.store (status);
}

// enforce_request call. (...)
void DataPlaneStage::enforce_request (const uint32_t& workflow_id,
    const int& operation_type,
    const int& operation_context,
    const uint64_t& operation_size)
{

    // create Context object
    auto context_obj = this->m_posix_instance->build_context_object (workflow_id,
        operation_type,
        operation_context,
        operation_size,
        1);

    // submit request through posix_base
    this->m_posix_instance->posix_base (nullptr, operation_size, context_obj);

    // create debug message
#if OPTION_DETAILED_LOGGING
    std::stringstream message;
    message << __func__ << "(" << workflow_id << ", " << operation_type << ", " << operation_context
            << ", " << operation_size << ")";
    this->m_log->log_debug (message.str ());
#endif
}

// set_local_connection_address call. Set data plane stage connection address (to local control plane).
std::string DataPlaneStage::set_local_connection_address () const
{
    // get environment variable for data plane stage
    auto address_value = std::getenv (padll::options::option_default_connection_address_env.data ());

    if (address_value != nullptr) {
        // log message
        Logging::log_warn ("Cheferd local connection address is `" + std::string (address_value) + "`.");

        // return fetched connection address
        return std::string (address_value);
    } else {
        // log message
        Logging::log_warn ("Inaccessible environment variable ("
            + std::string (padll::options::option_default_connection_address_env) + ") value: using default connection address.");

        // return paio default connection address
        return paio::options::option_default_socket_name ();
    }
}

} // namespace padll::stage