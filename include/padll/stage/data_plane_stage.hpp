/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
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

class DataPlaneStage {

private:
    std::mutex m_lock;
    std::shared_ptr<Log> m_log { nullptr };
    std::atomic<bool> m_stage_initialized { false };
    std::shared_ptr<paio::PaioStage> m_stage { nullptr };
    std::unique_ptr<paio::PosixLayer> m_posix_instance { nullptr };

    /**
     * set_stage_initialized:
     * @param status
     */
    void set_stage_initialized (const bool& status);

public:
    /**
     * DataPlaneStage default constructor.
     */
    DataPlaneStage ();

    /**
     * DataPlaneStage parameterized constructor.
     * @param log
     * @param hsk_rules_path
     * @param dif_rules_path
     * @param enf_rules_path
     * @param execute_on_receive
     */
    DataPlaneStage (std::shared_ptr<Log> log_ptr,
        const std::string& hsk_rules_path,
        const std::string& dif_rules_path,
        const std::string& enf_rules_path,
        const bool& execute_on_receive);

    /**
     * DataPlaneStage default destructor.
     */
    ~DataPlaneStage ();

    /**
     * enforce_request:
     * @param workflow_id
     * @param operation_type
     * @param operation_context
     * @param operation_size
     */
    void enforce_request (const uint32_t& workflow_id,
        const int& operation_type,
        const int& operation_context,
        const uint64_t& operation_size);
};
} // namespace padll::stage
#endif // PADLL_DATA_PLANE_STAGE_H
