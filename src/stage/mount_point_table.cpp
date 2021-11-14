/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <padll/stage/mount_point_table.hpp>

namespace padll::stage {

// MountPointTable default constructor.
MountPointTable::MountPointTable ()
{
    std::cout << "MountPointTable default constructor." << std::endl;
    this->initialize ();
}

// MountPointTable parameterized constructor.
MountPointTable::MountPointTable (const std::string& value)
{
    std::cout << "MountPointTable: " << value << std::endl;
    this->initialize ();
}

// MountPointTable default destructor.
MountPointTable::~MountPointTable ()
{
    std::cout << this->to_string () << std::endl;
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
        std::cerr << "ERROR: File descriptor " << fd << " already exists." << std::endl;
        return false;
    }

    // create entry for the 'fd' file descriptor
    auto emplace_return_value = this->m_file_descriptors_table.emplace (
        std::make_pair (fd, std::make_unique<MountPointEntry> (path, mount_point)));

    // check if the insertion was successful
    if (!emplace_return_value.second) {
        std::cerr << "ERROR: File descriptor " << fd << " could not be inserted." << std::endl;
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
        std::cerr << "ERROR: File descriptor " << file_ptr << " already exists." << std::endl;
        return false;
    }

    // create entry for the 'file_ptr' file pointer
    auto emplace_return_value = this->m_file_ptr_table.emplace (
        std::make_pair (file_ptr, std::make_unique<MountPointEntry> (path, mount_point)));

    // check if the insertion was successful
    if (!emplace_return_value.second) {
        std::cerr << "ERROR: File pointer " << file_ptr << " could not be inserted." << std::endl;
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
        // fixme: change message to logging class
        std::cerr << "ERROR: File descriptor " << key << " could not be removed." << std::endl;
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
        // fixme: change message to logging class
        std::cerr << "ERROR: File pointer " << key << " could not be removed." << std::endl;
        return false;
    }

    return true;
}

// register_mount_point_type call. (...)
void MountPointTable::register_mount_point_type (const MountPoint& type,
    const std::vector<uint32_t>& workflows)
{
    // fixme: remove debugging
    std::cout << "registering mount point type <";
    std::cout << (type == MountPoint::kLocal ? "local"
                                             : (type == MountPoint::kRemote ? "remote" : "none"));
    std::cout << "> with workflows <";
    for (auto workflow : workflows) {
        std::cout << workflow << " ";
    }
    std::cout << ">" << std::endl;
    // ---
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

// pick_workflow_id call. (...)
uint32_t MountPointTable::pick_workflow_id (const std::string& path)
{
    // get namespace type
    auto namespace_type = (!option_mount_point_differentiation)
        ? MountPoint::kNone
        : this->extract_mount_point_from_path (path);

    // select  workflow identifier
    auto workflow_id = this->select_workflow_id (namespace_type);

    // verify if the workflow identifier was not found
    if (workflow_id == -1) {
        std::printf ("Error while selecting workflow id.\n");
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
        std::printf ("Error while selecting workflow id.\n");
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
        std::printf ("Error while selecting workflow id.\n");
    }

    return workflow_id;
}

// parse_path call. (...)
MountPoint MountPointTable::extract_mount_point_from_path (const std::string& path)
{
    // TODO: implement this
    // verify if path is valid
    // compare with local path
    // compare with remote path
    return MountPoint::kNone;
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

// get_default_workflows call. (...)
const MountPointWorkflows& MountPointTable::get_default_workflows () const
{
    return this->m_default_workflows;
}

} // namespace padll::stage