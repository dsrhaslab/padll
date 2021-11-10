/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#include <padll/stage/namespace_entry.hpp>

namespace padll {

    // NamespaceEntry default constructor.
    NamespaceEntry::NamespaceEntry() = default;

    // NamespaceEntry parameterized constructor.
    NamespaceEntry::NamespaceEntry (std::string path, std::string mount_point) :
        m_path { std::move (path) },
        m_mount_point { std::move (mount_point) }
    {}

    // NamespaceEntry default destructor.
    NamespaceEntry::~NamespaceEntry() = default;

    // get_path call. (...)
    const std::string& NamespaceEntry::get_path () const
    {
        return this->m_path;
    }

    // get_mount_point call. (...)
    const std::string& NamespaceEntry::get_mount_point () const
    {
        return this->m_mount_point;
    }

    // to_string call. (...)
    std::string NamespaceEntry::to_string() const
    {
        return "NamespaceEntry: {" + this->m_path + ", " + this->m_mount_point + "}";
    }

}
