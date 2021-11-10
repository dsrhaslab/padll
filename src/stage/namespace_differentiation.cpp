/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <padll/stage/namespace_differentiation.hpp>

namespace padll {

// NamespaceTable default constructor.
NamespaceTable::NamespaceTable () = default;

// NamespaceTable default destructor.
NamespaceTable::~NamespaceTable () = default;

// create_namespace_entry call. (...)
bool NamespaceTable::create_namespace_entry (std::string path, std::string mount_point)
{
    std::lock_guard<std::shared_timed_mutex> write_lock (this->m_shared_lock);
    // here we can use map[] to check if the namespace already exists
    return false;
}

// get_namespace_entry call. (...)
NamespaceEntry* NamespaceTable::get_namespace_entry (const int& key)
{
    std::shared_lock<std::shared_timed_mutex> read_lock (this->m_shared_lock);
    // here we can use map.at() to get the namespace entry
    return nullptr;
}

// remove_namespace_entry call. (...)
bool NamespaceTable::remove_namespace_entry (const int& key)
{
    std::lock_guard<std::shared_timed_mutex> write_lock (this->m_shared_lock);
    return false;
}

}