/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef PADLL_NAMESPACE_DIFFERENTIATION_HPP
#define PADLL_NAMESPACE_DIFFERENTIATION_HPP

#include <shared_mutex>
#include <unordered_map>
#include <map>
#include <vector>
#include <padll/stage/namespace_entry.hpp>

namespace padll {

class NamespaceTable {

private:
    std::shared_timed_mutex m_shared_lock;
    //Note: needs CC
    std::unordered_map<int, std::unique_ptr<NamespaceEntry>> m_file_descriptors_table {};
    //Note: does not need CC; is read-only;
    std::map<std::string, std::vector<uint32_t>> m_namespace_workflows {};

public:

    /**
     * NamespaceTable default constructor.
     */
    NamespaceTable ();

    /**
     * NamespaceTable default destructor.
     */
    ~NamespaceTable ();

    // todo: use write lock
    /**
     * create_namespace_entry:
     * @param path
     * @param mount_point
     * @return
     */
    bool create_namespace_entry (std::string path, std::string mount_point);

    // todo: use read lock
    /**
     * get_namespace_entry:
     * @param key
     * @return
     */
    NamespaceEntry* get_namespace_entry (const int& key);

    // todo: use write lock
    /**
     * remove_namespace_entry
     * @param key
     * @return
     */
    bool remove_namespace_entry (const int& key);
};

}

#endif // PADLL_NAMESPACE_DIFFERENTIATION_HPP
