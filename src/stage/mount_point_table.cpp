/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#include <padll/stage/mount_point_table.hpp>

namespace padll::stage {

// MountPointTable default constructor.
MountPointTable::MountPointTable () :
    m_log { std::make_shared<Log> (option_default_enable_debug_level,
        false,
        std::string { option_default_log_path }) }
{
    // logging message
    std::stringstream stream;
    stream << "MountPointTable default constructor ";
    stream << "(" << static_cast<void*> (this->m_log.get ()) << ")";
    this->m_log->log_info (stream.str ());

    // initialize mount point table with predefined workflows
    this->initialize ();
}

// MountPointTable parameterized constructor.
MountPointTable::MountPointTable (std::shared_ptr<Log> log_ptr) : m_log { log_ptr }
{
    // logging message
    std::stringstream stream;
    stream << "MountPointTable parameterized constructor ";
    stream << "(" << static_cast<void*> (this->m_log.get ()) << ")";
    this->m_log->log_info (stream.str ());

    // initialize mount point table with predefined workflows
    this->initialize ();
}

// MountPointTable default destructor.
MountPointTable::~MountPointTable ()
{
    // create logging message
    this->m_log->log_info ("MountPointTable default destructor.");
}

// is_file_descriptor_valid call. (...)
bool MountPointTable::is_file_descriptor_valid (const int& fd) const
{
    if (fd <= 2) {
        this->m_log->log_error (
            "Accessing special (or inexistent) file descriptor (" + std::to_string (fd) + ").");
        return false;
    }
    return true;
}

// is_file_pointer_valid call. (...)
bool MountPointTable::is_file_pointer_valid (const FILE* fptr) const
{
    if (fptr == nullptr || fptr == stdin || fptr == stdout || fptr == stderr) {
        std::stringstream stream;
        stream << "Accessing special (or inexistent) file pointer (";
        if (fptr != nullptr) {
            stream << fptr;
        }
        stream << ").";

        this->m_log->log_error (stream.str ());
        return false;
    }
    return true;
}

// initialize call. (...)
void MountPointTable::initialize ()
{
    // if mount point differentiation is enabled, register local and remote workflows
    if (option_mount_point_differentiation_enabled) {
        // FIXME: Needing refactor or cleanup -@gsd at 4/13/2022, 2:14:55 PM
        // Do not consider right now differentiation between local and remote mount points.

        // register local mount point workflows
        // this->register_mount_point_type (MountPoint::kLocal,
        //     this->m_default_workflows.default_local_mount_point_workflows);
        // register remote mount point workflows

        this->register_mount_point_type (MountPoint::kRemote,
            this->m_default_workflows.default_remote_mount_point_workflows);
    } else {
        // register all mount point workflows
        this->register_mount_point_type (MountPoint::kNone,
            this->m_default_workflows.default_mount_point_workflows);
    }
}

// create_mount_point_entry call. (...)
bool MountPointTable::create_mount_point_entry (const int& fd,
    const std::string& path,
    const MountPoint& mount_point,
    const uint32_t& metadata_server_unit)
{
    // check if key is a reserved or inexistent file descriptor
    if (!this->is_file_descriptor_valid (fd)) {
        return false;
    }

    // lock_guard over shared_timed_mutex (write_lock)
    std::lock_guard write_lock (this->m_fd_shared_lock);

    // create entry for the 'fd' file descriptor
    auto [iter, inserted] = this->m_file_descriptors_table.emplace (std::make_pair (fd,
        std::make_unique<MountPointEntry> (path, mount_point, metadata_server_unit)));

    // check if the insertion was successful
    if (!inserted) {
        // replace key-value pair in file_descriptors_table
        this->m_file_descriptors_table.at (fd)
            = std::make_unique<MountPointEntry> (path, mount_point, metadata_server_unit);
// submit error message to the logging facility
#if OPTION_DETAILED_LOGGING
        this->m_log->log_debug ("Replacing value at file descriptor " + std::to_string (fd) + ".");
#endif
    }

    return inserted;
}

// create_mount_point_entry call. (...)
bool MountPointTable::create_mount_point_entry (FILE* file_ptr,
    const std::string& path,
    const MountPoint& mount_point,
    const uint32_t& metadata_server_unit)
{
    // check if key is a reserved or inexistent file descriptor
    if (!this->is_file_pointer_valid (file_ptr)) {
        return false;
    }

    // lock_guard over shared_timed_mutex (write_lock)
    std::lock_guard write_lock (this->m_fptr_shared_lock);

    // create entry for the 'file_ptr' file pointer
    auto [iter, inserted] = this->m_file_ptr_table.emplace (std::make_pair (file_ptr,
        std::make_unique<MountPointEntry> (path, mount_point, metadata_server_unit)));

    // check if the insertion was successful
    if (!inserted) {
        // replace key-value pair in file_ptr_table
        this->m_file_ptr_table.at (file_ptr)
            = std::make_unique<MountPointEntry> (path, mount_point, metadata_server_unit);

// submit error message to the logging facility
#if OPTION_DETAILED_LOGGING
        std::stringstream stream;
        stream << "Replacing value at file pointer " << file_ptr << ".";
        // submit error message to the logging facility
        this->m_log->log_debug (stream.str ());
#endif
    }

    return inserted;
}

// get_mount_point_entry call. (...)
std::pair<bool, MountPointEntry*> MountPointTable::get_mount_point_entry (const int& key)
{
    // check if key is a reserved or inexistent file descriptor
    if (!this->is_file_descriptor_valid (key)) {
        return std::make_pair (false, nullptr);
    }

    // shared_lock over shared_timed_mutex (read_lock)
    std::shared_lock read_lock (this->m_fd_shared_lock);

    // get the entry for the 'key' file descriptor
    auto iterator = this->m_file_descriptors_table.find (key);
    // check if the entry exists
    if (iterator == this->m_file_descriptors_table.end ()) {
        this->m_log->log_error ("Mount point entry does not exist (" + std::to_string (key) + ").");
        return std::make_pair (false, nullptr);
    } else {
        // return pointer to entry's value
        return std::make_pair (true, iterator->second.get ());
    }
}

// get_mount_point_entry call. (...)
std::pair<bool, MountPointEntry*> MountPointTable::get_mount_point_entry (FILE* key)
{
    // check if key is a reserved or inexistent file descriptor
    if (!this->is_file_pointer_valid (key)) {
        return std::make_pair (false, nullptr);
    }

    // shared_lock over shared_time_mutex (read_lock)
    std::shared_lock read_lock (this->m_fptr_shared_lock);

    // get the entry for the 'key' file pointer
    auto iterator = this->m_file_ptr_table.find (key);
    // check if the entry exists
    if (iterator == this->m_file_ptr_table.end ()) {
        this->m_log->log_error ("Mount point entry does not exist.");
        return std::make_pair (false, nullptr);
    } else {
        // return pointer to entry's value
        return std::make_pair (true, iterator->second.get ());
    }
}

// remove_mount_point_entry call. (...)
bool MountPointTable::remove_mount_point_entry (const int& key)
{
    // check if key is a reserved or inexistent file descriptor
    if (!this->is_file_descriptor_valid (key)) {
        return false;
    }

    // lock_guard over shared_timed_mutex (write_lock)
    std::lock_guard write_lock (this->m_fd_shared_lock);

    // remove entry for the 'key' file descriptor and check if the removal was successful
    if (this->m_file_descriptors_table.erase (key) == 0) {
        // submit error message to the logging facility
        this->m_log->log_error (
            "File descriptor " + std::to_string (key) + " could not be removed.");

        return false;
    }

    return true;
}

// remove_mount_point_entry call. (...)
bool MountPointTable::remove_mount_point_entry (FILE* key)
{
    // check if key is a reserved or inexistent file descriptor
    if (!this->is_file_pointer_valid (key)) {
        return false;
    }

    // lock_guard over shared_timed_mutex (write_lock)
    std::lock_guard write_lock (this->m_fptr_shared_lock);

    // check if the removal was successful
    if (this->m_file_ptr_table.erase (key) == 0) {
        std::stringstream stream;
        stream << "File pointer " << key << " could not be removed.";
        // submit error message to the logging facility
        this->m_log->log_error (stream.str ());

        return false;
    }

    return true;
}

// replace_file_descriptor call. (...)
bool MountPointTable::replace_file_descriptor (const int& old_fd, const int& new_fd)
{
    // check if old_fd and new_fd belong to reserved or inexistent file descriptors
    if (!this->is_file_descriptor_valid (old_fd) || !this->is_file_descriptor_valid (new_fd)) {
        return false;
    }

    // lock_guard over shared_timed_mutex (write_lock)
    std::lock_guard write_lock (this->m_fd_shared_lock);

    // get the entry for the 'old_fd' file descriptor
    auto iterator = this->m_file_descriptors_table.find (old_fd);

    // check if the entry exists
    if (iterator == this->m_file_descriptors_table.end ()) {
        this->m_log->log_error ("Cannot replace file descriptor " + std::to_string (old_fd) + ".");
        return false;
    } else {
        // extract key-value pair from file_descriptors_table
        auto node_entry = this->m_file_descriptors_table.extract (iterator);
        // replace node_entry's key with new_fd
        node_entry.key () = new_fd;
        // insert node_entry into file_descriptors_table
        this->m_file_descriptors_table.insert (std::move (node_entry));

        return true;
    }
}

// register_mount_point_type call. (...)
void MountPointTable::register_mount_point_type (const MountPoint& type,
    const std::vector<uint32_t>& workflows)
{
    // find the mount point type
    auto iterator = this->m_mount_point_workflows.find (type);

    // if the mount point type does not exist, create it
    if (iterator == this->m_mount_point_workflows.end ()) {
        // create the mount point type
        auto [iter, inserted] = this->m_mount_point_workflows.try_emplace (type, workflows);

        // validate if <MountPoint, Workflows> entry was emplaced
        if (!inserted) {
            std::stringstream stream;
            stream << "Mount point type " << padll::options::mount_point_to_string (type)
                   << " could not be registered.";
            // submit error message to the logging facility
            this->m_log->log_error (stream.str ());
        }

    } else {
        iterator->second = workflows;
    }
}

// extract_mount_point call. (...)
MountPoint MountPointTable::extract_mount_point (const std::string_view& path) const
{
    // get namespace type
    return (!option_mount_point_differentiation_enabled)
        ? MountPoint::kNone
        : this->extract_mount_point_from_path (path);
}

// pick_workflow_id call. (...)
std::pair<MountPoint, uint32_t> MountPointTable::pick_workflow_id (const std::string_view& path)
{
    // extract mount point of the given path
    auto namespace_type = (!option_mount_point_differentiation_enabled)
        ? MountPoint::kNone
        : this->extract_mount_point_from_path (path);

    // select workflow identifier
    auto workflow_id = (option_select_workflow_by_metadata_unit)
        ? this->select_workflow_from_metadata_unit (path)
        : this->select_workflow_from_mountpoint (namespace_type);

    // verify if the workflow identifier was not found
    if (workflow_id == static_cast<uint32_t> (-1)) {
        this->m_log->log_error ("Error while selecting workflow id.");
    }

    return std::make_pair (namespace_type, workflow_id);
}

// pick_workflow_id call. (...)
uint32_t MountPointTable::pick_workflow_id (const int& fd)
{
    auto workflow_id = static_cast<uint32_t> (-1);
    // get MountPointEntry of the given file descriptor
    auto [return_value, entry_ptr] = this->get_mount_point_entry (fd);

    // check if the mount point entry was found
    if (return_value) {
        // BUG: Reported defects -@rgmacedo at 4/12/2022, 1:38:18 PM
        // This will only work if all file descriptors are registered ...; in the future support the
        // option to no 'LD_PRELOAD' open calls, but register the MountPointEntry for 'LD_PRELOADED'
        // read and write calls get mount_point
        // select workflow-id from Mount Point or Metadata Server unit
        workflow_id = (option_select_workflow_by_metadata_unit)
            ? this->select_workflow_from_metadata_unit (entry_ptr->get_metadata_server_unit ())
            : this->select_workflow_from_mountpoint (entry_ptr->get_mount_point ());

        // verify if the workflow identifier was not found
        if (workflow_id == static_cast<uint32_t> (-1)) {
            this->m_log->log_error ("Error while selecting workflow id.");
        }
    }

    return workflow_id;
}

// pick_workflow_id_by_force call. This operation is to be used on 'close' syscall, when the regular
// pick_workflow_id does not work.
uint32_t MountPointTable::pick_workflow_id_by_force ()
{
    // define MountPoint value
    auto mount_point
        = option_mount_point_differentiation_enabled ? MountPoint::kRemote : MountPoint::kNone;

    // select workflow-id from MountPoint
    auto workflow_id = this->select_workflow_from_mountpoint (mount_point);

    // verify if the workflow identifier was not found
    if (workflow_id == static_cast<uint32_t> (-1)) {
        this->m_log->log_error ("Error while selecting workflow id.");
    }

    return workflow_id;
}

// pick_workflow_id call. (...)
uint32_t MountPointTable::pick_workflow_id (FILE* file_ptr)
{
    auto workflow_id = static_cast<uint32_t> (-1);
    // get MountPointEntry of the give file pointer
    auto [return_value, entry_ptr] = this->get_mount_point_entry (file_ptr);

    // check if the mount point entry was found
    if (return_value) {
        // select workflow-id from Mount Point or Metadata Server unit
        workflow_id = (option_select_workflow_by_metadata_unit)
            ? this->select_workflow_from_metadata_unit (entry_ptr->get_metadata_server_unit ())
            : this->select_workflow_from_mountpoint (entry_ptr->get_mount_point ());

        // verify if the workflow identifier was not found
        if (workflow_id == static_cast<uint32_t> (-1)) {
            this->m_log->log_error ("Error while selecting workflow id.");
        }
    }

    return workflow_id;
}

// FIXME: Needing refactor or cleanup -@gsd at 4/13/2022, 2:18:55 PM
// Do not consider right now differentiation between local and remote mountpoints.
// compare_first_with_local_mount_point call. (...)
// MountPoint compare_first_with_local_mount_point (const std::string_view& path)
// {
//     auto return_value = MountPoint::kNone;

//     // check if the path is in local mount point
//     if (path.find (option_default_local_mount_point) != std::string::npos) {
//         return_value = MountPoint::kLocal;

//     } else if (path.find (option_default_remote_mount_point) != std::string::npos) {
//         return_value = MountPoint::kRemote;
//     }

//     return return_value;
// }

// compare_first_with_remote_mount_point call. (...)
MountPoint compare_first_with_remote_mount_point (const std::string_view& path)
{
    auto return_value = MountPoint::kNone;

    // check if the path is in remote mount point
    if (path.find (option_default_remote_mount_point) != std::string::npos) {
        return_value = MountPoint::kRemote;
    }
    // FIXME: Needing refactor or cleanup -@gsd at 4/13/2022, 2:19:53 PM
    // Do not consider right now differentiation between local and remote mountpoints.
    // else if (path.find (option_default_local_mount_point) != std::string::npos) {
    //     return_value = MountPoint::kLocal;
    // }

    return return_value;
}

// parse_path call. (...)
MountPoint MountPointTable::extract_mount_point_from_path (const std::string_view& path) const
{
    auto return_value = MountPoint::kNone;

    if (option_mount_point_differentiation_enabled) {
        return_value = compare_first_with_remote_mount_point (path);
        // FIXME: Needing refactor or cleanup -@gsd at 4/13/2022, 2:20:39 PM
        // Do not consider right now differentiation between local and remote mountpoints.
        // return_value = option_check_local_mount_point_first
        //     ? compare_first_with_local_mount_point (path)
        //     : compare_first_with_remote_mount_point (path);

        // if the mount point is not found, create debug message
        if (return_value == MountPoint::kNone) {
            this->m_log->log_error ("Extracted path does not belong to any defined mountpoint.");
        }
    }

    return return_value;
}

// select_workflow_from_mountpoint call. (...)
uint32_t MountPointTable::select_workflow_from_mountpoint (const MountPoint& namespace_name)
{
    // get iterator to the mount point entry
    auto iterator = this->m_mount_point_workflows.find (namespace_name);
    auto return_value = static_cast<uint32_t> (-1);

    // if the namespace exists, pick a random workflow
    if (iterator != this->m_mount_point_workflows.end ()) {
        // generate random item to pick
        auto random_item = static_cast<int> (this->m_prng () % iterator->second.size ());
        // return workflow identifier
        return_value = iterator->second.begin ()[random_item];
    }

    // if the namespace entry is not found, return -1
    return return_value;
}

// select_workflow_from_metadata_unit call. (...)
uint32_t MountPointTable::select_workflow_from_metadata_unit (const uint32_t& metadata_unit) const
{
    // TODO: work-in-progress
    std::cerr << __func__ << "(" << metadata_unit << "): Not implemented yet." << std::endl;
    return static_cast<uint32_t> (-1);
}

// select_workflow_from_metadata_unit call. (...)
uint32_t MountPointTable::select_workflow_from_metadata_unit (const std::string_view& path) const
{
    // TODO: work-in-progress
    std::cerr << __func__ << "(" << path << "): Not implemented yet." << std::endl;
    return static_cast<uint32_t> (-1);
}

// to_string call. (...)
std::string MountPointTable::to_string () const
{
    std::stringstream stream;
    stream << "MountPointTable: " << std::endl;

    for (auto const& [entry_mountpoint, entry_workflows] : this->m_mount_point_workflows) {
        stream << "  ";
        stream << padll::options::mount_point_to_string (entry_mountpoint);
        stream << ": ";

        for (auto const& workflow : entry_workflows) {
            stream << workflow << " ";
        }
        stream << std::endl;
    }
    return stream.str ();
}

// fd_table_to_string call. (...)
std::string MountPointTable::fd_table_to_string ()
{
    // shared_lock over shared_timed_mutex (read_lock)
    std::shared_lock read_lock (this->m_fd_shared_lock);

    std::stringstream stream;
    stream << "FileDescriptorTable: " << std::endl;
    for (auto const& [entry_fd, mnt_entry_ptr] : this->m_file_descriptors_table) {
        stream << "  " << entry_fd << ": ";
        stream << mnt_entry_ptr->to_string () << std::endl;
    }

    return stream.str ();
}

// fp_table_to_string call. (...)
std::string MountPointTable::fp_table_to_string ()
{
    // shared_lock over shared_timed_mutex (read_lock)
    std::shared_lock read_lock (this->m_fd_shared_lock);

    std::stringstream stream;
    stream << "FilePtrTable: " << std::endl;
    for (auto const& [entry_file_ptr, mnt_entry_ptr] : this->m_file_ptr_table) {
        stream << "  " << entry_file_ptr << ": ";
        stream << mnt_entry_ptr->to_string () << std::endl;
    }

    return stream.str ();
}

// get_default_workflows call. (...)
const MountPointWorkflows& MountPointTable::get_default_workflows () const
{
    return this->m_default_workflows;
}

} // namespace padll::stage
