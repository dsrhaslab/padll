/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
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
 * Complete me ...
 *  - m_metadata_server_unit: is used to identify to which MDS/MDT server the MountPointEntry (and
 * the file) belongs.
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
     * @param path
     * @param mount_point
     */
    MountPointEntry (const std::string& path, const MountPoint& mount_point);

    /**
     * MountPointEntry parameterized constructor.
     * @param path
     * @param mount_point
     * @param metadata_instance
     */
    MountPointEntry (const std::string& path,
        const MountPoint& mount_point,
        const uint32_t& metadata_instance);

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
     * get_metadata_server_unit:
     * @return
     */
    [[nodiscard]] const uint32_t& get_metadata_server_unit () const;

    /**
     * to_string:
     * @return
     */
    [[nodiscard]] std::string to_string () const;
};

} // namespace padll::stage
#endif // PADLL_NAMESPACE_ENTRY_H
