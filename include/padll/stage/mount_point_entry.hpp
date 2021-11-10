/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#ifndef PADLL_NAMESPACE_ENTRY_H
#define PADLL_NAMESPACE_ENTRY_H

#include <string>
#include <mutex>

namespace padll {

class MountPointEntry {

private:
    std::string m_path {};
    std::string m_mount_point {};
    std::mutex m_lock;

public:

    /**
     * MountPointEntry default constructor.
     */
    MountPointEntry ();

    /**
     * MountPointEntry parameterized constructor.
     * @param path
     * @param mountpoint
     */
    MountPointEntry (std::string path, std::string mount_point);

    /**
     * MountPointEntry default destructor.
     */
    ~MountPointEntry ();

    /**
     * get_path:
     * @return
     */
    const std::string& get_path () const;

    /**
     * get_mount_point:
     * @return
     */
    const std::string& get_mount_point () const;

    /**
     * to_string:
     * @return
     */
    std::string to_string () const;
};

}
#endif //PADLL_NAMESPACE_ENTRY_H
