/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef PADLL_NAMESPACE_ENTRY_H
#define PADLL_NAMESPACE_ENTRY_H

#include <mutex>
#include <padll/options/options.hpp>
#include <string>

using namespace padll::options;

namespace padll::stage {

class MountPointEntry {

private:
    std::string m_path {};
    MountPoint m_mount_point {};
    std::mutex m_lock;

public:
    /**
     * MountPointEntry default constructor.
     */
    MountPointEntry ();

    /**
     * MountPointEntry parameterized constructor.
     * @param path
     * @param mount_point
     */
    MountPointEntry (std::string path, const MountPoint& mount_point);

    /**
     * MountPointEntry default destructor.
     */
    ~MountPointEntry ();

    /**
     * get_path:
     * @return
     */
    [[nodiscard]] const std::string& get_path () const;

    /**
     * get_mount_point:
     * @return
     */
    [[nodiscard]] const MountPoint& get_mount_point () const;

    /**
     * to_string:
     * @return
     */
    [[nodiscard]] std::string to_string () const;
};

} // namespace padll::stage
#endif // PADLL_NAMESPACE_ENTRY_H
