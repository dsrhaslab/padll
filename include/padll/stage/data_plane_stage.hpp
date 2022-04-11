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
    std::atomic<bool> m_stage_initialized { false };
    std::shared_ptr<paio::PaioStage> m_stage { nullptr };
    std::unique_ptr<paio::PosixLayer> m_posix_instance { nullptr };
    // const long m_workflow_id { 1000 };
    const bool m_enforce { false }; // todo: temporary

    std::shared_ptr<Log> m_log { nullptr };

    /**
     * initialize_stage:
     */
    void initialize_stage ();

public:
    /**
     * DataPlaneStage default constructor.
     */
    DataPlaneStage ();

    /**
     * DataPlaneStage parameterized constructor.
     * @param log
     */
    explicit DataPlaneStage (std::shared_ptr<Log> log_ptr);

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
