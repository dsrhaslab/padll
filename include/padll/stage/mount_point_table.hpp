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
 */
class MountPointWorkflows {
    // TODO: Tasks pending completion -@ricardomacedo at 4/7/2022, 10:19:02 AM
    // The mountpoint workflows should be defined by the controller, not by the data plane stage.
public:
    const std::vector<uint32_t> default_mount_point_workflows { 1000,
        2000,
        3000,
        4000,
        5000,
        6000 };
    const std::vector<uint32_t> default_remote_mount_point_workflows { 1000, 2000, 3000 };
    // FIXME: Needing refactor or cleanup -@gsd at 4/13/2022, 2:14:08 PM
    // Do not consider right now differentiation between local and remote mount points.
    // const std::vector<uint32_t> default_local_mount_point_workflows { 4000, 5000, 6000 };
};

class MountPointTable {

private:
    std::shared_timed_mutex m_fd_shared_lock;
    std::shared_timed_mutex m_fptr_shared_lock;
    MountPointWorkflows m_default_workflows {};
    std::unordered_map<int, std::unique_ptr<MountPointEntry>> m_file_descriptors_table {};
    std::unordered_map<FILE*, std::unique_ptr<MountPointEntry>> m_file_ptr_table {};
    std::map<MountPoint, std::vector<uint32_t>> m_mount_point_workflows {};

    std::shared_ptr<Log> m_log { nullptr };

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

    /**
     * is_file_descriptor_valid:
     * @param fd
     */
    [[nodiscard]] bool is_file_descriptor_valid (const int& fd) const;

    /**
     * is_file_pointer_valid:
     * @param fptr
     */
    [[nodiscard]] bool is_file_pointer_valid (const FILE* fptr) const;

public:
    /**
     * MountPointTable default constructor.
     */
    MountPointTable ();

    /**
     * MountPointTable parameterized constructor.
     * @param log_ptr
     */
    explicit MountPointTable (std::shared_ptr<Log> log_ptr);

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
    [[nodiscard]] std::pair<MountPoint, uint32_t> pick_workflow_id (
        const std::string_view& path) const;

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
    [[nodiscard]] std::pair<bool, MountPointEntry*> get_mount_point_entry (const int& key);

    /**
     * get_mount_point_entry:
     * @param key
     * @return
     */
    [[nodiscard]] std::pair<bool, MountPointEntry*> get_mount_point_entry (FILE* key);

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
     * replace_file_descriptor:
     * @param old_fd
     * @oaram new_fd
     * @return
     */
    [[nodiscard]] bool replace_file_descriptor (const int& old_fd, const int& new_fd);

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
