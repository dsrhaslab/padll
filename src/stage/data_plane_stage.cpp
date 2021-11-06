/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <padll/stage/data_plane_stage.hpp>

namespace padll {

// TODO: Implement the DataPlaneStage constructor
DataPlaneStage::DataPlaneStage () = default;

// TODO: Implement the DataPlaneStage destructor
DataPlaneStage::~DataPlaneStage () = default;

// initialize_stage call. (...)
void DataPlaneStage::initialize_stage ()
{
    std::unique_lock<std::mutex> lock (this->m_lock);

    // initialize PAIO structures (stage and instance-interface)
    this->m_stage = { std::make_shared<paio::PaioStage> (option_default_stage_channels,
        option_default_stage_object_creation,
        option_default_stage_name) };
    this->m_posix_instance = paio::make_unique<paio::PosixLayer> (this->m_stage);

    // temporary ...
    std::this_thread::sleep_for (std::chrono::seconds (5));

    // update initialization status
    this->m_stage_initialized.store (true);
}

// enforce_request call. (...)
void DataPlaneStage::enforce_request (const long& workflow_id,
    const int& operation_type,
    const int& operation_context,
    const uint64_t& operation_size)
{
    // todo: implement logic of validating data plane stage initialization

    // create Context object
    auto context_obj = this->m_posix_instance->build_context_object (workflow_id,
        operation_type,
        operation_context,
        operation_size,
        1);

    // submit request through posix_noop
    this->m_posix_instance->posix_noop (nullptr, operation_size, context_obj);
}

} // namespace padll