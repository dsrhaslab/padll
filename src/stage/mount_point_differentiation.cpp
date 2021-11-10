/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <padll/stage/mount_point_differentiation.hpp>

namespace padll {

// MountPointTable default constructor.
MountPointTable::MountPointTable () = default;

// MountPointTable default destructor.
MountPointTable::~MountPointTable () = default;

// create_mount_point_entry call. (...)
bool MountPointTable::create_mount_point_entry (const int& fd, std::string path, MountPoint mount_point)
{
    std::lock_guard<std::shared_timed_mutex> write_lock (this->m_fd_shared_lock);
    // here we can use map[] to check if the namespace already exists
    return false;
}

// create_mount_point_entry call. (...)
bool MountPointTable::create_mount_point_entry (FILE* file_ptr, std::string path, MountPoint mount_point)
{
    std::lock_guard<std::shared_timed_mutex> write_lock (this->m_fptr_shared_lock);
    // here we can use map[] to check if the namespace already exists
    return false;
}

// get_mount_point_entry call. (...)
MountPointEntry* MountPointTable::get_mount_point_entry (const int& key)
{
    std::shared_lock<std::shared_timed_mutex> read_lock (this->m_fd_shared_lock);
    // here we can use map.at() to get the namespace entry
    return nullptr;
}

// get_mount_point_entry call. (...)
MountPointEntry* MountPointTable::get_mount_point_entry (FILE* key)
{
    std::shared_lock<std::shared_timed_mutex> read_lock (this->m_fptr_shared_lock);
    // here we can use map.at() to get the namespace entry
    return nullptr;
}

// remove_mount_point_entry call. (...)
bool MountPointTable::remove_mount_point_entry (const int& key)
{
    std::lock_guard<std::shared_timed_mutex> write_lock (this->m_fd_shared_lock);
    return false;
}

// remove_mount_point_entry call. (...)
bool MountPointTable::remove_mount_point_entry (FILE* key)
{
    std::lock_guard<std::shared_timed_mutex> write_lock (this->m_fptr_shared_lock);
    return false;
}

// register_mount_point_type call. (...)
void MountPointTable::register_mount_point_type (const MountPoint &type, std::vector<uint32_t> workflows) {
    // TODO: implement this
}

// parse_path call. (...)
MountPoint MountPointTable::parse_path (const std::string& path)
{
    // TODO: implement this
    // verify if path is valid
    // compare with local path
    // compare with remote path
}

// select_workflow_id call. (...)
uint32_t MountPointTable::select_workflow_id (const MountPoint& namespace_name) const
{
    // get iterator to the mount point entry
    auto iterator = this->m_mount_point_workflows.find (namespace_name);
    uint32_t return_value = -1;

    // if the namespace entry is not found, return -1
    if (iterator != this->m_mount_point_workflows.end ()) {
        // generate random item to pick
        // todo: we should use a better random number generator (e.g. mt19937)
        auto random_item = random () % iterator->second.size ();
        // return workflow identifier
        return iterator->second.begin ()[random_item];
    }

    return return_value;
}

// pick_workflow_id call. (...)
uint32_t MountPointTable::pick_workflow_id (const std::string &path) {

    // get namespace type
    auto namespace_type = (!option_mount_point_differentiation)
            ? MountPoint::kNone
            : this->parse_path (path);

    auto workflow_id = this->select_workflow_id (namespace_type);

    if (workflow_id == -1) {
        std::printf ("Error while selecting workflow id.\n");
        return -1;
    }
}


}