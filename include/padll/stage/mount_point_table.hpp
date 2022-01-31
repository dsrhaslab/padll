/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
 **/

#ifndef PADLL_MOUNT_POINT_TABLE_HPP
#define PADLL_MOUNT_POINT_TABLE_HPP

#include <iostream>
#include <map>
#include <padll/options/options.hpp>
#include <padll/stage/mount_point_entry.hpp>
#include <padll/utils/log.hpp>
#include <shared_mutex>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace padll::utils::log;

namespace padll::stage {

/**
 * MountPointWorkflows class.
 * fixme: this needs to be adjusted (later); the controller should specify these ...
 */
class MountPointWorkflows {

public:
    const std::vector<uint32_t> default_mount_point_workflows { 1000,
        2000,
        3000,
        4000,
        5000,
        6000 };
    const std::vector<uint32_t> default_remote_mount_point_workflows { 4000, 5000, 6000 };
    const std::vector<uint32_t> default_local_mount_point_workflows { 1000, 2000, 3000 };
};

class MountPointTable {

private:
    std::shared_timed_mutex m_fd_shared_lock;
    std::shared_timed_mutex m_fptr_shared_lock;
    MountPointWorkflows m_default_workflows {};
    std::unordered_map<int, std::unique_ptr<MountPointEntry>> m_file_descriptors_table {};
    std::unordered_map<FILE*, std::unique_ptr<MountPointEntry>> m_file_ptr_table {};
    std::map<MountPoint, std::vector<uint32_t>> m_mount_point_workflows {};
    std::shared_ptr<Log> m_logging {};

    /**
     * initialize:
     */
    void initialize ();

    /**
     * register_mount_point_type:
     * @param type
     * @param workflows
     */
    void register_mount_point_type (const MountPoint& type, const std::vector<uint32_t>& workflows);

    /**
     * extract_mount_point_from_path:
     * @param path
     * @return
     */
    [[nodiscard]] MountPoint extract_mount_point_from_path (const std::string_view& path) const;

    /**
     * select_workflow_id
     * @param mount_point
     * @return
     */
    [[nodiscard]] uint32_t select_workflow_id (const MountPoint& mount_point) const;

public:
    /**
     * MountPointTable default constructor.
     */
    MountPointTable ();

    /**
     * MountPointTable parameterized constructor.
     * @param value
     * Fixme: remove value parameter; only for debugging purposes.
     */
    [[maybe_unused]] MountPointTable (std::shared_ptr<Log> log_ptr, const std::string& value);

    /**
     * MountPointTable default destructor.
     */
    ~MountPointTable ();

    /**
     * create_mount_point_entry:
     * @param fd
     * @param path
     * @param mount_point
     * @return
     */
    bool create_mount_point_entry (const int& fd,
        const std::string& path,
        const MountPoint& mount_point);

    /**
     * create_mount_point_entry:
     * @param file_ptr
     * @param path
     * @param mount_point
     * @return
     */
    bool create_mount_point_entry (FILE* file_ptr,
        const std::string& path,
        const MountPoint& mount_point);

    /**
     * extract_mount_point:
     * @param path
     * @return
     */
    [[nodiscard]] MountPoint extract_mount_point (const std::string_view& path) const;

    /**
     * pick_workflow_id:
     * @param path
     * @return
     */
    [[nodiscard]] uint32_t pick_workflow_id (const std::string_view& path);

    /**
     * pick_workflow_id:
     * @param fd
     * @return
     */
    [[nodiscard]] uint32_t pick_workflow_id (const int& fd);

    /**
     * pick_workflow_id
     * @param file_ptr
     * @return
     */
    [[nodiscard]] uint32_t pick_workflow_id (FILE* file_ptr);

    /**
     * get_mount_point_fd_entry:
     * @param key
     * @return
     */
    [[nodiscard]] const MountPointEntry* get_mount_point_entry (const int& key);

    /**
     * get_mount_point_entry:
     * @param key
     * @return
     */
    [[nodiscard]] const MountPointEntry* get_mount_point_entry (FILE* key);

    /**
     * remove_mount_point_entry:
     * @param key
     * @return
     */
    bool remove_mount_point_entry (const int& key);

    /**
     * remove_mount_point_entry:
     * @param key
     * @return
     */
    bool remove_mount_point_entry (FILE* key);

    /**
     * to_string:
     * @return
     */
    [[nodiscard]] std::string to_string () const;

    /**
     * fd_table_to_string:
     * @return
     */
    [[nodiscard]] std::string fd_table_to_string ();

    /**
     * fp_table_to_string:
     * @return
     */
    [[nodiscard]] std::string fp_table_to_string ();

    /**
     * get_default_workflows:
     * @return
     */
    [[nodiscard]] const MountPointWorkflows& get_default_workflows () const;
};

} // namespace padll::stage

#endif // PADLL_MOUNT_POINT_TABLE_HPP
