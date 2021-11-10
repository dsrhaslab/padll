/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#include <padll/stage/mount_point_entry.hpp>

namespace padll {

    // MountPointEntry default constructor.
    MountPointEntry::MountPointEntry() = default;

    // MountPointEntry parameterized constructor.
    MountPointEntry::MountPointEntry (std::string path, std::string mount_point) :
        m_path { std::move (path) },
        m_mount_point { std::move (mount_point) }
    {}

    // MountPointEntry default destructor.
    MountPointEntry::~MountPointEntry() = default;

    // get_path call. (...)
    const std::string& MountPointEntry::get_path () const
    {
        return this->m_path;
    }

    // get_mount_point call. (...)
    const std::string& MountPointEntry::get_mount_point () const
    {
        return this->m_mount_point;
    }

    // to_string call. (...)
    std::string MountPointEntry::to_string() const
    {
        return "MountPointEntry: {" + this->m_path + ", " + this->m_mount_point + "}";
    }

}
