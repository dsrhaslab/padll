/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
 **/

#include <padll/stage/data_plane_stage.hpp>

namespace padll::stage {

// TODO: Tasks pending completion -@ricardomacedo at 4/11/2022, 4:10:17 PM
// Initialize data plane stage in constructor (connect to control plane)
DataPlaneStage::DataPlaneStage () :
    m_log { std::make_shared<Log> (option_default_enable_debug_level,
        option_default_enable_debug_with_ld_preload,
        std::string { option_default_log_path }) }
{
    // create logging message
    std::stringstream stream;
    stream << "DataPlaneStage default constructor ";
    stream << "(" << static_cast<void*> (this->m_log.get ()) << ")";

    // write debug logging message
    this->m_log->log_info (stream.str ());
}

// TODO: Tasks pending completion -@ricardomacedo at 4/11/2022, 4:10:45 PM
// Initialize data plane stage in constructor (connect to control plane)
DataPlaneStage::DataPlaneStage (std::shared_ptr<Log> log_ptr) : m_log { log_ptr }
{
    // create logging message
    std::stringstream stream;
    stream << "DataPlaneStage parameterized constructor ";
    stream << "(" << static_cast<void*> (this->m_log.get ()) << ")";

    // write debug logging message
    this->m_log->log_info (stream.str ());
}

// TODO: Implement the DataPlaneStage destructor
DataPlaneStage::~DataPlaneStage ()
{
    this->m_log->log_info ("DataPlaneStage destructor.\n");
}

// initialize_stage call. (...)
void DataPlaneStage::initialize_stage ()
{
    // unique_lock over mutex
    std::unique_lock lock (this->m_lock);

    // initialize PAIO structures (stage and instance-interface)
    this->m_stage = { std::make_shared<paio::PaioStage> (option_default_stage_channels,
        option_default_stage_object_creation,
        std::string (option_default_stage_name)) };
    this->m_posix_instance = std::make_unique<paio::PosixLayer> (this->m_stage);

    // fixme: remove; this is only temporary ...
    std::this_thread::sleep_for (std::chrono::seconds (1));

    // update initialization status
    this->m_stage_initialized.store (true);
}

// enforce_request call. (...)
void DataPlaneStage::enforce_request (const uint32_t& workflow_id,
    const int& operation_type,
    const int& operation_context,
    const uint64_t& operation_size)
{
    // Note: temporary just to try out tests w/ and w/o data plane stage
    if (this->m_enforce) {
        // initialize data plane stage
        // TODO: Tasks pending completion -@ricardomacedo at 4/11/2022, 4:04:55 PM
        // adjust this to not call m_stage_initialized on every single call
        if (!m_stage_initialized.load (std::memory_order_relaxed)) {
            this->initialize_stage ();
            std::cout << this->m_stage->stage_info_to_string () << "\n";
        }

        // missing: validate workflow-id ...

        // create Context object
        auto context_obj = this->m_posix_instance->build_context_object (workflow_id,
            operation_type,
            operation_context,
            operation_size,
            1);

        // submit request through posix_base
        this->m_posix_instance->posix_base (nullptr, operation_size, context_obj);
    }
}

} // namespace padll::stage