/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef PADLL_MOUNT_POINT_TABLE_HPP
#define PADLL_MOUNT_POINT_TABLE_HPP

#include <iostream>
#include <map>
#include <padll/options/options.hpp>
#include <padll/stage/mount_point_entry.hpp>
#include <shared_mutex>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace padll::stage {

class MountPointTable {
    friend class MountPointDifferentiationTest;

private:
    std::shared_timed_mutex m_fd_shared_lock;
    std::shared_timed_mutex m_fptr_shared_lock;
    std::unordered_map<int, std::unique_ptr<MountPointEntry>> m_file_descriptors_table {};
    std::unordered_map<FILE*, std::unique_ptr<MountPointEntry>> m_file_ptr_table {};
    std::map<MountPoint, std::vector<uint32_t>> m_mount_point_workflows {};

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
    MountPoint extract_mount_point_from_path (const std::string& path);

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
     * Fixme: temporary; only for debugging purposes.
     */
    explicit MountPointTable (const std::string& value);

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
     * pick_workflow_id:
     * @param path
     * @return
     */
    uint32_t pick_workflow_id (const std::string& path);

    /**
     * pick_workflow_id:
     * @param fd
     * @return
     */
    uint32_t pick_workflow_id (const int& fd);

    /**
     * pick_workflow_id
     * @param file_ptr
     * @return
     */
    uint32_t pick_workflow_id (FILE* file_ptr);

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
};

} // namespace padll::stage

#endif // PADLL_MOUNT_POINT_TABLE_HPP
