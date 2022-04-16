/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
 **/

#ifndef PADLL_OPTIONS_HPP
#define PADLL_OPTIONS_HPP

#include <filesystem>
#include <string>
#include <vector>

/**
 * TODO: create Options class and initialize as a shared_ptr; otherwise we can have initialization
 * issues in MountPointTable::initialize()
 */
namespace padll::options {

/**
 * MountPoint enum class.
 * Defines the available mount points of which PADLL will intercept requests to/from.
 *  - kNone: no mount point differentiation is made; all requests will be interpreted as if destined
 * to the same mount point;
 *  - kLocal: only requests destined towards a local mount point (mount point deployed on the same
 * host as PADLL) will be handled by PADLL;
 *  - kRemote: only requests destined towards a remote mount point (e.g., Lustre, NFS server, etc.)
 * will be handled by PADLL.
 */
enum class MountPoint { kNone = 0, kLocal = 1, kRemote = 2 };

/**
 * mount_point_to_string: auxiliary method that converts a MountPoint enum value to a string.
 * @param mount_point_value
 * @return constexpr std::string_view
 */
constexpr std::string_view mount_point_to_string (const MountPoint& mount_point)
{
    switch (mount_point) {
        case MountPoint::kNone:
            return "kNone";
        // FIXME: Needing refactor or cleanup -@gsd at 4/13/2022, 2:39:45 PM
        // Do not consider right now differentiation between local and remote mount points.
        // case MountPoint::kLocal:
        //     return "kLocal";
        case MountPoint::kRemote:
            return "kRemote";
        default:
            return "unknown";
    }
}

/***************************************************************************************************
 * PADLL default configurations
 **************************************************************************************************/

/**
 * option_library_name:
 */
constexpr std::string_view option_library_name { "libc.so.6" };

/**
 * option_default_statistic_collection:
 */
constexpr bool option_default_statistic_collection { true };

/**
 * option_mount_point_differentiation_enabled:
 *  all operations are considered with the same set of workflow identifiers
 */
constexpr bool option_mount_point_differentiation_enabled { true };

/**
 * option_check_local_mount_point_first:
 *  if option_mount_point_differentiation = true, first check if the path to be extracted is in the
 *  local mount point. If not, check if it is in the remote mount point.
 * FIXME: Needing refactor or cleanup -@gsd at 4/13/2022, 2:39:45 PM
 * Do not consider right now differentiation between local and remote mount points.
 */
// constexpr bool option_check_local_mount_point_first { true };

/**
 * option_default_local_mount_point:
 *  operations will pick from a selected set of workflow identifiers
 * FIXME: Needing refactor or cleanup -@gsd at 4/13/2022, 2:16:25 PM
 * Do not consider right now differentiation between local and remote mount points.
 */
// constexpr std::string_view option_default_local_mount_point { "/local" };
// constexpr std::string_view option_default_local_mount_point { "/tmp" };

/**
 * option_default_remote_mount_point:
 *  operations will pick from a selected set of workflow identifiers
 */
// constexpr std::string_view option_default_remote_mount_point { "/home" };
constexpr std::string_view option_default_remote_mount_point { "/" };

/**
 * option_hard_remove:
 * Document this ...
 */
constexpr bool option_hard_remove { false };

/***************************************************************************************************
 * Log configuration
 **************************************************************************************************/

/**
 * option_default_enable_debug_level:
 */
constexpr bool option_default_enable_debug_level { true };

/**
 * option_default_enable_debug_with_ld_preload:
 */
constexpr bool option_default_enable_debug_with_ld_preload { true };

/**
 * option_default_log_path:
 */
constexpr std::string_view option_default_log_path { "/tmp/padll-info" };

/**
 * option_default_detailed_logging:
 */
// constexpr bool option_default_detailed_logging { true };
#define OPTION_DETAILED_LOGGING true

/**
 * option_default_table_format:
 */
constexpr bool option_default_table_format { false };

/**
 * option_deafault_save_statistics_report:
 */ 
constexpr bool option_default_save_statistics_report { true };

/**
 * option_default_statistics_report_path:
 */
constexpr std::string_view option_default_statistics_report_path { "/tmp" };

// *************************************************************************************************
//  Default PAIO data plane stage configuration
// *************************************************************************************************

/**
 * option_default_stage_channels:
 */
constexpr int option_default_stage_channels { 1 };

/**
 * option_default_stage_object_creation:
 */
constexpr bool option_default_stage_object_creation { true };

/**
 * option_default_stage_name:
 */
constexpr std::string_view option_default_stage_name { "tensorflow-" };

} // namespace padll::options

#endif // PADLL_OPTIONS_HPP
