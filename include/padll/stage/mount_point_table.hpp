/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#ifndef PADLL_MOUNT_POINT_TABLE_HPP
#define PADLL_MOUNT_POINT_TABLE_HPP

#include <iostream>
#include <map>
#include <padll/options/options.hpp>
#include <padll/stage/mount_point_entry.hpp>
#include <padll/third_party/xoshiro.hpp>
#include <padll/utils/log.hpp>
#include <shared_mutex>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace padll::utils::log;
using namespace XoshiroCpp;

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
    // const std::vector<uint32_t> default_remote_mount_point_workflows { 1000 };
    // FIXME: Needing refactor or cleanup -@gsd at 4/13/2022, 2:14:08 PM
    // Do not consider right now differentiation between local and remote mount points.
    // const std::vector<uint32_t> default_local_mount_point_workflows { 4000, 5000, 6000 };

    std::vector<uint32_t> default_remote_mount_point_workflows {};

    MountPointWorkflows (const int& num_workflows)
    {
        for (int i = 1; i <= num_workflows; i++) {
            this->default_remote_mount_point_workflows.push_back (i * 1000);
        }
    }
};

/**
 * MountPointTable class.
 * This class registers all mountpoints to be considered for intercepted requests. Further, it
 * manages all file descriptor and file pointer based operations, by registering in the
 * m_file_descriptors_table and m_file_ptr_table upon path-based requests (e.g., open, fopen, ...).
 */
class MountPointTable {

private:
    std::shared_timed_mutex m_fd_shared_lock;
    std::shared_timed_mutex m_fptr_shared_lock;
    int m_num_workflows { padll::options::option_padll_workflows () };
    MountPointWorkflows m_default_workflows { this->m_num_workflows };
    std::unordered_map<int, std::unique_ptr<MountPointEntry>> m_file_descriptors_table {};
    std::unordered_map<FILE*, std::unique_ptr<MountPointEntry>> m_file_ptr_table {};
    std::map<MountPoint, std::vector<uint32_t>> m_mount_point_workflows {};

    std::shared_ptr<Log> m_log { std::make_shared<Log> () };
    Xoshiro128StarStar m_prng { static_cast<uint64_t> (::getpid ()) };

    /**
     * initialize: initialize and register workflows for considered mountpoint types.
     */
    void initialize ();

    /**
     * register_mount_point_type: assign a given set of workflows for a given mountpoint type.
     * @param type MountPoint type to be considered; can be of types ::none, ::local, or ::remote.
     * @param workflows Container that holds all workflows to be registered for that given
     * MountPoint type.
     */
    void register_mount_point_type (const MountPoint& type, const std::vector<uint32_t>& workflows);

    /**
     * extract_mount_point_from_path: identify the targeted mountpoint of a given path.
     * @param path Path to be considered.
     * @return Returns the respective MountPoint associated with path.
     */
    [[nodiscard]] MountPoint extract_mount_point_from_path (const std::string_view& path) const;

    /**
     * select_workflow_from_mountpoint: select a workflow from the MountPoint set. For load
     * balancing, workflows are assigned in random fashion with a Xoshiro128StarStar hashing scheme.
     * @param mount_point MountPoint to select the workflow.
     * @return Returns the identifier of the selected workflow.
     */
    [[nodiscard]] uint32_t select_workflow_from_mountpoint (const MountPoint& mount_point);

    /**
     * select_workflow_from_metadata_unit: work-in-progress.
     * @return
     */
    [[nodiscard]] uint32_t select_workflow_from_metadata_unit (const uint32_t& metadata_unit) const;

    /**
     * select_workflow_from_metadata_unit: work-in-progress.
     * @param path
     */
    [[nodiscard]] uint32_t select_workflow_from_metadata_unit (const std::string_view& path) const;

    /**
     * is_file_descriptor_valid: validates if a given file descriptor is valid. Currently, we
     * consider that a given file descriptor is valid if greater than 2 (>2); since negative fds are
     * errors, 0 to stdin, 1 to stdout, and 2 to stderr.
     * TODO: as future work, intercept dup/dup2 system calls to prevent false invalidations.
     * @param fd File descriptor to be verified.
     * @return Returns a boolean stating if the FD is valid or not.
     */
    [[nodiscard]] bool is_file_descriptor_valid (const int& fd) const;

    /**
     * is_file_pointer_valid: validates if a given file pointer is valid. Currently, we consider
     * that a given file pointer is invalid if it's a nullptr, or equal to stdin, stdout, or stderr.
     * @param fptr File pointer to be verified.
     * @return Returns a boolean stating if the FILE* is valid or not.
     */
    [[nodiscard]] bool is_file_pointer_valid (const FILE* fptr) const;

public:
    /**
     * MountPointTable default constructor.
     */
    MountPointTable ();

    /**
     * MountPointTable parameterized constructor.
     * @param log_ptr Shared pointer to a Logging object.
     */
    explicit MountPointTable (std::shared_ptr<Log> log_ptr);

    /**
     * MountPointTable default destructor.
     */
    ~MountPointTable ();

    /**
     * create_mount_point_entry: create a entry at the m_file_descriptor_table container.
     * @param fd File descriptor to be inserted.
     * @param path Path that the file descriptor respects to.
     * @param mount_point Targeted mountpoint.
     * @param metadata_server_unit work-in-progress
     * @return Returns a boolean that defines if the entry was successfully created (true) or not
     * (false).
     */
    bool create_mount_point_entry (const int& fd,
        const std::string& path,
        const MountPoint& mount_point,
        const uint32_t& metadata_server_unit);

    /**
     * create_mount_point_entry: create a entry at the m_file_pointer_table container.
     * @param file_ptr File pointer to be inserted.
     * @param path Path that the file pointer respects to.
     * @param mount_point Targeted mountpoint.
     * @param metadata_server_unit work-in-progress.
     * @return Returns a boolean that defines if the entry was successfully created (true) or not
     * (false).
     */
    bool create_mount_point_entry (FILE* file_ptr,
        const std::string& path,
        const MountPoint& mount_point,
        const uint32_t& metadata_server_unit);

    /**
     * extract_mount_point: extract mountpoint from a given path. This method uses
     * extract_mount_point_from_path.
     * @param path Path to be considered.
     * @return Returns the respective MountPoint associated with path.
     */
    [[nodiscard]] MountPoint extract_mount_point (const std::string_view& path) const;

    /**
     * pick_workflow_id: select a workflow id to enforce a request destined towards a given path.
     * @param path Path to considered.
     * @return Returns a pair of a MountPoint and the selected workflow identifier.
     */
    [[nodiscard]] std::pair<MountPoint, uint32_t> pick_workflow_id (const std::string_view& path);

    /**
     * pick_workflow_id: select a workflow id to a enforce a request destined towards a given file
     * descriptor.
     * @param fd File descriptor to be considered.
     * @return Returns a workflow identifier.
     */
    [[nodiscard]] uint32_t pick_workflow_id (const int& fd);

    /**
     * pick_workflow_id:
     * FIXME: this method is a temporary hack to overcome the limitation of unregistered file
     * descriptors. Currently, this is only being used on
     * LdPreloadedPosix::ld_preloaded_posix_close.
     */
    [[nodiscard]] uint32_t pick_workflow_id_by_force ();

    /**
     * pick_workflow_id: select a workflow id to a enforce a request destined towards a given file
     * pointer.
     * @param file_ptr File pointer to be considered.
     * @return Returns a workflow identifier.
     */
    [[nodiscard]] uint32_t pick_workflow_id (FILE* file_ptr);

    /**
     * get_mount_point_entry: get the mountpoint entry of the m_file_descriptor_table.
     * @param key The key corresponds to the registered file descriptor.
     * @return Returns a pair with the a boolean, which defines if the operation was successful, and
     * a pointer to the mountpoint entry.
     */
    [[nodiscard]] std::pair<bool, MountPointEntry*> get_mount_point_entry (const int& key);

    /**
     * get_mount_point_entry: get the mountpoint entry of the m_file_pointer_table.
     * @param key The key corresponds to the registered file pointer.
     * @return Returns a pair with the a boolean, which defines if the operation was successful, and
     * a pointer to the mountpoint entry.
     */
    [[nodiscard]] std::pair<bool, MountPointEntry*> get_mount_point_entry (FILE* key);

    /**
     * remove_mount_point_entry: remove entry from the m_file_descriptor_table.
     * @param key The key corresponds to the registered filed descriptor.
     * @return Returns a boolean that defines if the entry was removed (true) or not (false).
     */
    bool remove_mount_point_entry (const int& key);

    /**
     * remove_mount_point_entry: remove entry from the m_file_pointer_table.
     * @param key The key corresponds to the registered filed pointer.
     * @return Returns a boolean that defines if the entry was removed (true) or not (false).
     */
    bool remove_mount_point_entry (FILE* key);

    /**
     * replace_file_descriptor: replace one file descriptor for a new one. This is used to handle
     * ::fcntl functions for the F_DUPFD and F_DUPFD_CLOEXEC flags.
     * @param old_fd Old file descriptor, with an entry in the m_file_descriptor_table.
     * @param new_fd New file descriptor to be replaced.
     * @return Returns a boolean that defines if the entry was replaced (true) or not (false).
     */
    [[nodiscard]] bool replace_file_descriptor (const int& old_fd, const int& new_fd);

    /**
     * to_string: returns in string-based format all registered workflows in
     * m_mount_point_workflows.
     */
    [[nodiscard]] std::string to_string () const;

    /**
     * fd_table_to_string: returns in string-based format all entries of the
     * m_file_descriptor_table.
     */
    [[nodiscard]] std::string fd_table_to_string ();

    /**
     * fd_table_to_string: returns in string-based format all entries of the m_file_pointer_table.
     */
    [[nodiscard]] std::string fp_table_to_string ();

    /**
     * get_default_workflows: get all workflows registered in m_default_workflows.
     * @return Returns a const reference to the m_default_workflows container.
     */
    [[nodiscard]] const MountPointWorkflows& get_default_workflows () const;
};

} // namespace padll::stage

#endif // PADLL_MOUNT_POINT_TABLE_HPP
