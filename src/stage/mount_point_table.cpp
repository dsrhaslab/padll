/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <padll/stage/mount_point_table.hpp>

namespace padll::stage {

// MountPointTable default constructor.
MountPointTable::MountPointTable () : m_logging { std::make_shared<Log> () }
{
    // logging message
    this->m_logging->log_info ("MountPointTable default constructor.\n");
    // initialize mount point table with predefined workflows
    this->initialize ();
}

// MountPointTable parameterized constructor.
MountPointTable::MountPointTable (std::shared_ptr<Log> log_ptr, const std::string& value) :
    m_logging { std::move (log_ptr) }
{
    // logging message
    this->m_logging->log_info ("MountPointTable parameterized constructor: " + value + " ("
        + std::to_string (this->m_logging.use_count ()) + ")");
    // initialize mount point table with predefined workflows
    this->initialize ();
}

// MountPointTable default destructor.
MountPointTable::~MountPointTable ()
{
    // logging message
    this->m_logging->log_debug ("MountPointTable destructor.");
}

// initialize call. (...)
void MountPointTable::initialize ()
{
    // if mount point differentiation is enabled, register local and remote workflows
    if (option_mount_point_differentiation) {
        // register local mount point workflows
        this->register_mount_point_type (MountPoint::kLocal,
            this->m_default_workflows.default_local_mount_point_workflows);
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
    const MountPoint& mount_point)
{
    std::lock_guard<std::shared_timed_mutex> write_lock (this->m_fd_shared_lock);

    auto iterator = this->m_file_descriptors_table.find (fd);
    if (iterator != this->m_file_descriptors_table.end ()) {
        std::stringstream stream;
        stream << "File descriptor " << fd << " already exists.";
        // submit error message to the logging facility
        this->m_logging->log_error (stream.str ());

        return false;
    }

    // create entry for the 'fd' file descriptor
    auto emplace_return_value = this->m_file_descriptors_table.emplace (
        std::make_pair (fd, std::make_unique<MountPointEntry> (path, mount_point)));

    // check if the insertion was successful
    if (!emplace_return_value.second) {
        std::stringstream stream;
        stream << "File descriptor " << fd << " could not be inserted.";
        // submit error message to the logging facility
        this->m_logging->log_error (stream.str ());

        return false;
    }

    return true;
}

// create_mount_point_entry call. (...)
bool MountPointTable::create_mount_point_entry (FILE* file_ptr,
    const std::string& path,
    const MountPoint& mount_point)
{
    std::lock_guard<std::shared_timed_mutex> write_lock (this->m_fptr_shared_lock);

    auto iterator = this->m_file_ptr_table.find (file_ptr);
    if (iterator != this->m_file_ptr_table.end ()) {
        std::stringstream stream;
        stream << "File descriptor " << file_ptr << " already exists.";
        // submit error message to the logging facility
        this->m_logging->log_error (stream.str ());

        return false;
    }

    // create entry for the 'file_ptr' file pointer
    auto emplace_return_value = this->m_file_ptr_table.emplace (
        std::make_pair (file_ptr, std::make_unique<MountPointEntry> (path, mount_point)));

    // check if the insertion was successful
    if (!emplace_return_value.second) {
        std::stringstream stream;
        stream << "File pointer " << file_ptr << " could not be inserted.";
        // submit error message to the logging facility
        this->m_logging->log_error (stream.str ());

        return false;
    }

    return true;
}

// get_mount_point_entry call. (...)
const MountPointEntry* MountPointTable::get_mount_point_entry (const int& key)
{
    std::shared_lock<std::shared_timed_mutex> read_lock (this->m_fd_shared_lock);

    // get the entry for the 'key' file descriptor
    auto iterator = this->m_file_descriptors_table.find (key);
    // check if the entry exists
    if (iterator == this->m_file_descriptors_table.end ()) {
        this->m_logging->log_error ("Mount point entry does not exist.");
        return nullptr;
    } else {
        // return pointer to entry's value
        return iterator->second.get ();
    }
}

// get_mount_point_entry call. (...)
const MountPointEntry* MountPointTable::get_mount_point_entry (FILE* key)
{
    std::shared_lock<std::shared_timed_mutex> read_lock (this->m_fptr_shared_lock);

    // get the entry for the 'key' file pointer
    auto iterator = this->m_file_ptr_table.find (key);
    // check if the entry exists
    if (iterator == this->m_file_ptr_table.end ()) {
        this->m_logging->log_error ("Mount point entry does not exist.");
        return nullptr;
    } else {
        // return pointer to entry's value
        return iterator->second.get ();
    }
}

// remove_mount_point_entry call. (...)
// fixme: for "unlink", this method does not work, since it only remove files until the last file
//  descriptor referring to it is closed
bool MountPointTable::remove_mount_point_entry (const int& key)
{
    std::lock_guard<std::shared_timed_mutex> write_lock (this->m_fd_shared_lock);

    // remove the entry for the 'key' file descriptor
    auto return_value = this->m_file_descriptors_table.erase (key);

    // check if the removal was successful
    if (return_value) {
        std::stringstream stream;
        stream << "File descriptor " << key << " could not be removed.";
        // submit error message to the logging facility
        this->m_logging->log_error (stream.str ());

        return false;
    }

    return true;
}

// remove_mount_point_entry call. (...)
// fixme: for "unlink", this method does not work, since it only remove files until the last file
//  descriptor referring to it is closed
bool MountPointTable::remove_mount_point_entry (FILE* key)
{
    std::lock_guard<std::shared_timed_mutex> write_lock (this->m_fptr_shared_lock);

    // remove the entry for the 'key' file pointer
    auto return_value = this->m_file_ptr_table.erase (key);

    // check if the removal was successful
    if (return_value) {
        std::stringstream stream;
        stream << "File pointer " << key << " could not be removed.";
        // submit error message to the logging facility
        this->m_logging->log_error (stream.str ());

        return false;
    }

    return true;
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
        this->m_mount_point_workflows.emplace (
            std::pair<MountPoint, std::vector<uint32_t>> (type, workflows));
    } else {
        iterator->second = workflows;
    }
}

// extract_mount_point call. (...)
MountPoint MountPointTable::extract_mount_point (const std::string_view& path) const
{
    // get namespace type
    return (!option_mount_point_differentiation) ? MountPoint::kNone
                                                 : this->extract_mount_point_from_path (path);
}

// pick_workflow_id call. (...)
uint32_t MountPointTable::pick_workflow_id (const std::string_view& path)
{
    // get namespace type
    auto namespace_type = (!option_mount_point_differentiation)
        ? MountPoint::kNone
        : this->extract_mount_point_from_path (path);

    // select  workflow identifier
    auto workflow_id = this->select_workflow_id (namespace_type);

    // verify if the workflow identifier was not found
    if (workflow_id == -1) {
        this->m_logging->log_error ("Error while selecting workflow id.");
    }

    return workflow_id;
}

// pick_workflow_id call. (...)
uint32_t MountPointTable::pick_workflow_id (const int& fd)
{
    auto mount_point = this->get_mount_point_entry (fd)->get_mount_point ();

    // select  workflow identifier
    auto workflow_id = this->select_workflow_id (mount_point);

    // verify if the workflow identifier was not found
    if (workflow_id == -1) {
        this->m_logging->log_error ("Error while selecting workflow id.");
    }

    return workflow_id;
}

// pick_workflow_id call. (...)
uint32_t MountPointTable::pick_workflow_id (FILE* file_ptr)
{
    auto mount_point = this->get_mount_point_entry (file_ptr)->get_mount_point ();

    // select  workflow identifier
    auto workflow_id = this->select_workflow_id (mount_point);

    // verify if the workflow identifier was not found
    if (workflow_id == -1) {
        this->m_logging->log_error ("Error while selecting workflow id.");
    }

    return workflow_id;
}

// compare_first_with_local_mount_point call. (...)
MountPoint compare_first_with_local_mount_point (const std::string_view& path)
{
    auto return_value = MountPoint::kNone;

    // check if the path is in local mount point
    if (path.find (option_default_local_mount_point) != std::string::npos) {
        return_value = MountPoint::kLocal;

    } else if (path.find (option_default_remote_mount_point) != std::string::npos) {
        return_value = MountPoint::kRemote;
    }

    return return_value;
}

// compare_first_with_remote_mount_point call. (...)
MountPoint compare_first_with_remote_mount_point (const std::string_view& path)
{
    auto return_value = MountPoint::kNone;

    // check if the path is in remote mount point
    if (path.find (option_default_remote_mount_point) != std::string::npos) {
        return_value = MountPoint::kRemote;
    } else if (path.find (option_default_local_mount_point) != std::string::npos) {
        return_value = MountPoint::kLocal;
    }

    return return_value;
}

// parse_path call. (...)
MountPoint MountPointTable::extract_mount_point_from_path (const std::string_view& path) const
{
    auto return_value = MountPoint::kNone;

    if (option_mount_point_differentiation) {
        return_value = (option_check_local_mount_point_first)
            ? compare_first_with_local_mount_point (path)
            : compare_first_with_remote_mount_point (path);

        // if the mount point is not found, create debug message
        if (return_value == MountPoint::kNone) {
            this->m_logging->log_debug ("Extracted path does not belong no neither mount point.");
        }
    }

    return return_value;
}

// select_workflow_id call. (...)
uint32_t MountPointTable::select_workflow_id (const MountPoint& namespace_name) const
{
    // get iterator to the mount point entry
    auto iterator = this->m_mount_point_workflows.find (namespace_name);
    uint32_t return_value = -1;

    // if the namespace exists, pick a random workflow
    if (iterator != this->m_mount_point_workflows.end ()) {
        // generate random item to pick
        // todo: we should use a better random number generator (e.g. mt19937)
        auto random_item = static_cast<int> (random () % iterator->second.size ());
        // return workflow identifier
        return iterator->second.begin ()[random_item];
    }

    // if the namespace entry is not found, return -1
    return return_value;
}

// to_string call. (...)
std::string MountPointTable::to_string () const
{
    std::stringstream stream;
    stream << "MountPointTable: " << std::endl;
    for (auto const& entry : this->m_mount_point_workflows) {
        stream << "  ";
        stream << ((entry.first == MountPoint::kLocal)
                ? "local"
                : (entry.first == MountPoint::kRemote ? "remote" : "none"));
        stream << ": ";

        for (auto const& workflow : entry.second) {
            stream << workflow << " ";
        }
        stream << std::endl;
    }
    return stream.str ();
}

// fd_table_to_string call. (...)
std::string MountPointTable::fd_table_to_string ()
{
    std::shared_lock<std::shared_timed_mutex> read_lock (this->m_fd_shared_lock);

    std::stringstream stream;
    stream << "FileDescriptorTable: " << std::endl;
    for (auto const& entry : this->m_file_descriptors_table) {
        stream << "  " << entry.first << ": ";
        stream << entry.second->to_string () << std::endl;
    }

    return stream.str ();
}

// fp_table_to_string call. (...)
std::string MountPointTable::fp_table_to_string ()
{
    std::shared_lock<std::shared_timed_mutex> read_lock (this->m_fd_shared_lock);

    std::stringstream stream;
    stream << "FilePtrTable: " << std::endl;
    for (auto const& entry : this->m_file_ptr_table) {
        stream << "  " << entry.first << ": ";
        stream << entry.second->to_string () << std::endl;
    }

    return stream.str ();
}

// get_default_workflows call. (...)
const MountPointWorkflows& MountPointTable::get_default_workflows () const
{
    return this->m_default_workflows;
}

} // namespace padll::stage