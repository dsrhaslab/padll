/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#ifndef PADLL_DATA_PLANE_STAGE_H
#define PADLL_DATA_PLANE_STAGE_H

#include <padll/options/options.hpp>
#include <padll/utils/log.hpp>
#include <paio/interface/posix_layer.hpp>
#include <paio/stage/paio_stage.hpp>

using namespace padll::options;
using namespace padll::utils::log;

namespace padll::stage {

/**
 * DataPlaneStage class.
 * This class handles all logic to submit requests to the PAIO data plane stage to be enforced (rate
 * limited).
 */
class DataPlaneStage {

private:
    std::mutex m_lock;
    std::shared_ptr<Log> m_log { nullptr };
    std::atomic<bool> m_stage_initialized { false };
    paio::options::CommunicationType m_communication_type {
        paio::options::CommunicationType::_unix
    };
    std::string m_local_controller_address { this->get_local_connection_address () };
    int m_local_controller_port { paio::options::option_default_port };
    std::shared_ptr<paio::PaioStage> m_stage { nullptr };
    std::unique_ptr<paio::PosixLayer> m_posix_instance { nullptr };

    /**
     * set_stage_initialized: mark data plane stage as initialized.
     * @param status Value to be set for stage initialization.
     */
    void set_stage_initialized (const bool& status);

    /**
     * get_local_connection_address: Get address for the data plane stage connection with local
     * controller.
     */
    std::string get_local_connection_address () const;

public:
    /**
     * DataPlaneStage default constructor.
     */
    DataPlaneStage ();

    /**
     * DataPlaneStage parameterized constructor.
     * This constructor is used when executing without control plane.
     * @param log Shared pointer to a Logging object.
     * @param num_channels Number of channels to be set in the data plane.
     * @param default_object_creation Enable or disable default enforcement object creation, upon
     * channel creation.
     * @param stage_name Name of the data plane stage.
     * @param hsk_rules_path Path to the housekeeping rules file.
     * @param dif_rules_path Path to the differentiation rules file.
     * @param enf_rules_path Path to the enforcement rules file.
     * @param execute_on_receive Boolean to define if rules should be applied upon parsing.
     */
    DataPlaneStage (std::shared_ptr<Log> log_ptr,
        const int& num_channels,
        const bool& default_object_creation,
        const std::string& stage_name,
        const std::string& hsk_rules_path,
        const std::string& dif_rules_path,
        const std::string& enf_rules_path,
        const bool& execute_on_receive);

    /**
     * DataPlaneStage parameterized constructor.
     * This constructor is used when executing with control plane.
     * @param log Shared pointer to a Logging object.
     * @param num_channels Number of channels to be set in the data plane.
     * @param default_object_creation Enable or disable default enforcement object creation, upon
     * channel creation.
     * @param stage_name Name of the data plane stage.
     */
    DataPlaneStage (std::shared_ptr<Log> log_ptr,
        const int& num_channels,
        const bool& default_object_creation,
        const std::string& stage_name);

    /**
     * DataPlaneStage default destructor.
     */
    ~DataPlaneStage ();

    /**
     * enforce_request: submit request to be enforced at the PAIO data plane stage.
     * @param workflow_id Workflow identifier (used for channel selection).
     * @param operation_type Operation type of the handled POSIX operation.
     * @param operation_context Context of the handled POSIX operation (data, metadata, extended
     * attributes, ...).
     * @param operation_size Size of the operation (will be used to determine the cost).
     */
    void enforce_request (const uint32_t& workflow_id,
        const int& operation_type,
        const int& operation_context,
        const uint64_t& operation_size);
};
} // namespace padll::stage
#endif // PADLL_DATA_PLANE_STAGE_H
