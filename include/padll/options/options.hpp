/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
 **/

#ifndef PADDL_OPTIONS_HPP
#define PADDL_OPTIONS_HPP

#include <filesystem>
#include <string>
#include <vector>

/**
 * TODO: create Options class and initialize as a shared_ptr; otherwise we can have initialization
 *  issues in MountPointTable::initialize()
 */
namespace padll::options {

enum class MountPoint { kNone = 0, kLocal = 1, kRemote = 2 };

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
 * option_mount_point_differentiation:
 *  all operations are considered with the same set of workflow identifiers
 */
constexpr bool option_mount_point_differentiation { true };

/**
 * option_check_local_mount_point_first:
 *  if option_mount_point_differentiation = true, first check if the path to be extracted is in the
 *  local mount point. If not, check if it is in the remote mount point.
 */
constexpr bool option_check_local_mount_point_first { true };

/**
 * option_default_local_mount_point:
 *  operations will pick from a selected set of workflow identifiers
 */
// constexpr std::string_view option_default_local_mount_point { "/local" };
constexpr std::string_view option_default_local_mount_point { "/tmp" };

/**
 * option_default_remote_mount_point:
 *  operations will pick from a selected set of workflow identifiers
 */
constexpr std::string_view option_default_remote_mount_point { "/home" };

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
constexpr bool option_default_detailed_logging { true };

/**
 * option_default_table_format:
 */
constexpr bool option_default_table_format { false };

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

#endif // PADDL_OPTIONS_HPP
