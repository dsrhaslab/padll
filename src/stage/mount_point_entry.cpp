/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
 **/

#include <padll/stage/mount_point_entry.hpp>

namespace padll::stage {

// MountPointEntry default constructor.
MountPointEntry::MountPointEntry () = default;

// MountPointEntry parameterized constructor.
MountPointEntry::MountPointEntry (const std::string& path, const MountPoint& mount_point) :
    m_path { path },
    m_mount_point { mount_point }
{ }

// MountPointEntry parameterized constructor.
MountPointEntry::MountPointEntry (const std::string& path,
    const MountPoint& mount_point,
    const uint32_t& metadata_instance) :
    m_path { path },
    m_mount_point { mount_point },
    m_metadata_server_unit { metadata_instance }
{ }

// MountPointEntry default destructor.
MountPointEntry::~MountPointEntry () = default;

// get_path call. (...)
const std::string& MountPointEntry::get_path () const
{
    return this->m_path;
}

// get_mount_point call. (...)
const MountPoint& MountPointEntry::get_mount_point () const
{
    return this->m_mount_point;
}

// get_metadata_server_unit call. (...)
const uint32_t& MountPointEntry::get_metadata_server_unit () const
{
    return this->m_metadata_server_unit;
}

// to_string call. (...)
std::string MountPointEntry::to_string () const
{
    std::stringstream stream;
    stream << "{" << this->m_path << ", ";
    stream << padll::options::mount_point_to_string (this->m_mount_point) << ", ";
    stream << this->m_metadata_server_unit << "}";

    return stream.str ();
}

} // namespace padll::stage
