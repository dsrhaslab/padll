/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef PADLL_DATA_PLANE_STAGE_H
#define PADLL_DATA_PLANE_STAGE_H

#include <padll/utils/options.hpp>
#include <paio/interface/posix_layer.hpp>
#include <paio/stage/paio_stage.hpp>
#include <padll/utils/logging.hpp>

namespace padll {

class DataPlaneStage {

private:
    std::mutex m_lock;
    std::atomic<bool> m_stage_initialized { false };
    std::shared_ptr<paio::PaioStage> m_stage { nullptr };
    std::unique_ptr<paio::PosixLayer> m_posix_instance { nullptr };
    const long m_workflow_id { 1000 };
    std::shared_ptr<Logging> m_logging { nullptr };

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
     * DataPlaneStage (explicit) parameterized constructor.
     * @param logging
     */
    explicit DataPlaneStage (std::shared_ptr<Logging> logging);

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
    void enforce_request (const long& workflow_id,
        const int& operation_type,
        const int& operation_context,
        const uint64_t& operation_size);
};
} // namespace padll
#endif // PADLL_DATA_PLANE_STAGE_H
