/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#ifndef PADLL_NAMESPACE_ENTRY_H
#define PADLL_NAMESPACE_ENTRY_H

#include <mutex>
#include <padll/options/options.hpp>
#include <sstream>
#include <string>

using namespace padll::options;

namespace padll::stage {

/**
 * MountPointEntry class.
 * This class is used to register the mountpoints to be considered by the PADLL data plane.
 */
class MountPointEntry {

private:
    std::string m_path {};
    MountPoint m_mount_point {};
    uint32_t m_metadata_server_unit { static_cast<uint32_t> (-1) };
    std::mutex m_lock;

public:
    /**
     * MountPointEntry default constructor.
     */
    MountPointEntry ();

    /**
     * MountPointEntry parameterized constructor.
     * @param path Path name to be registered as a new MountPointEntry.
     * @param mount_point Enum that defines if the path is a local or remote mountpoint.
     */
    MountPointEntry (const std::string& path, const MountPoint& mount_point);

    /**
     * MountPointEntry parameterized constructor.
     * @param path Path name to be registered as a new MountPointEntry.
     * @param mount_point Enum that defines if the path is a local or remote mountpoint.
     * @param metadata_instance Work-in-progress.
     */
    MountPointEntry (const std::string& path,
        const MountPoint& mount_point,
        const uint32_t& metadata_instance);

    /**
     * MountPointEntry default destructor.
     */
    ~MountPointEntry ();

    /**
     * get_path: get pathname of the MountPointEntry object.
     * @return Returns a const value of m_path.
     */
    [[nodiscard]] const std::string& get_path () const;

    /**
     * get_mount_point: get the mountpoint type of the MountPointEntry object.
     * @return Returns a const value of m_mount_point.
     */
    [[nodiscard]] const MountPoint& get_mount_point () const;

    /**
     * get_metadata_server_unit: work-in-progress.
     * @return
     */
    [[nodiscard]] const uint32_t& get_metadata_server_unit () const;

    /**
     * to_string: create a string with the MountPointEntry object data.
     * @return Returns the information of the MountPointEntry in string-based format.
     */
    [[nodiscard]] std::string to_string () const;
};

} // namespace padll::stage
#endif // PADLL_NAMESPACE_ENTRY_H
