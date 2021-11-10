/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef PADLL_MOUNT_POINT_DIFFERENTIATION_HPP
#define PADLL_MOUNT_POINT_DIFFERENTIATION_HPP

#include <shared_mutex>
#include <unordered_map>
#include <map>
#include <vector>
#include <padll/stage/mount_point_entry.hpp>
#include <padll/utils/options.hpp>

namespace padll {

class MountPointTable {

private:
    std::shared_timed_mutex m_fd_shared_lock;
    std::shared_timed_mutex m_fptr_shared_lock;
    //Note: needs CC
    std::unordered_map<int, std::unique_ptr<MountPointEntry>> m_file_descriptors_table {};
    std::unordered_map<FILE*, std::unique_ptr<MountPointEntry>> m_file_ptr_table {};
    std::map<MountPoint, std::vector<uint32_t>> m_mount_point_workflows {};


    void register_mount_point_type (const MountPoint& type, std::vector<uint32_t> workflows);

    /**
     * parse_path:
     * @param path
     * @return
     */
    MountPoint parse_path (const std::string& path);

    /**
     * pick_workflow_id:
     * @param path
     * @return
     */
    uint32_t pick_workflow_id (const std::string& path);

    /**
     * select_workflow_id
     * @param mount_point
     * @return
     */
    uint32_t select_workflow_id (const MountPoint& mount_point) const;

public:

    /**
     * MountPointTable default constructor.
     */
    MountPointTable ();

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
    bool create_mount_point_entry (const int& fd, std::string path, MountPoint mount_point);

    /**
     * create_mount_point_entry:
     * @param file_ptr
     * @param path
     * @param mount_point
     * @return
     */
    bool create_mount_point_entry (FILE* file_ptr, std::string path, MountPoint mount_point);

    /**
     * get_mount_point_fd_entry:
     * @param key
     * @return
     */
    MountPointEntry* get_mount_point_entry (const int& key);


    /**
     * get_mount_point_entry:
     * @param key
     * @return
     */
    MountPointEntry* get_mount_point_entry (FILE* key);

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
};

}

#endif // PADLL_MOUNT_POINT_DIFFERENTIATION_HPP
