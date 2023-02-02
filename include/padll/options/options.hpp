/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
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
constexpr std::string_view option_default_remote_mount_point { "/tmp" };

/**
 * option_hard_remove:
 * Document this ...
 */
constexpr bool option_hard_remove { false };

/**
 * option_default_metadata_server_unit:
 */
constexpr bool option_select_workflow_by_metadata_unit { false };

/**
 * option_padll_workflows: get the number of internal workflows used by the PADLL data plane stage.
 * @return: returns the number of workflows to set in the data plane stage. 
 *  May throw runtime_error exceptions if the padll_workflows are not set or are invalid values (< 0).
 */
inline const int option_padll_workflows () 
{
    // get value from environment variable
    auto workflows_env = std::getenv ("padll_workflows");
    
    // validate if variable was set
    if (workflows_env != nullptr) {
        auto workflows = std::stoi (workflows_env);

        // validate total of workflows
        if (workflows > 0) {
            return workflows;
        } else {
            throw std::runtime_error ("Invalid amount of workflows ('padll_workflows').");
        } 
    } else {
        throw std::runtime_error ("Environment variable 'padll_workflows' not set.");
    }
}

/***************************************************************************************************
 * Log configuration
 **************************************************************************************************/

/**
 * option_default_enable_debug_level:
 */
constexpr bool option_default_enable_debug_level { false };

/**
 * option_default_enable_debug_with_ld_preload:
 */
constexpr bool option_default_enable_debug_with_ld_preload { false };

/**
 * option_default_log_path:
 */
constexpr std::string_view option_default_log_path { "/tmp/padll-info" };

/**
 * option_default_detailed_logging:
 */
// constexpr bool option_default_detailed_logging { true };
#define OPTION_DETAILED_LOGGING false

/**
 * option_default_table_format:
 */
constexpr bool option_default_table_format { false };

/**
 * option_default_save_statistics_report:
 */
constexpr bool option_default_save_statistics_report { true };

/**
 * option_default_statistics_report_path:
 */
// constexpr std::string_view option_default_statistics_report_path { "/tmp" };
constexpr std::string_view option_default_statistics_report_path { "/home1/07853/rgmacedo/padll-results" };

// *************************************************************************************************
//  Default PAIO data plane stage configuration
// *************************************************************************************************

/**
 * main_path: Default main path.
 * This parameter defines the main path for the rules files (housekeeping, differentiation, and
 * enforcement).
 */
inline std::filesystem::path main_path ()
{
    // return "/home/gsd/hpdc22/padll/files/";
    return "/home1/07853/rgmacedo/padll/files/";
}

/**
 * option_default_stage_channels:
 */
constexpr int option_default_stage_channels { -1 };

/**
 * option_default_stage_object_creation:
 */
constexpr bool option_default_stage_object_creation { false };

/**
 * option_default_stage_name:
 */
constexpr std::string_view option_default_stage_name { "padll-stage" };

/**
 * option_default_stage_name_env:
 */
constexpr std::string_view option_default_stage_name_env { "paio_name" };

/**
 * option_paio_environment_variable_env:
 */
constexpr std::string_view option_paio_environment_variable_env { "paio_env" };

/**
 * option_default_connection_address_env:
 */
constexpr std::string_view option_default_connection_address_env { "cheferd_local_address" };

/**
 * option_default_hsk_rules_file:
 */
inline std::filesystem::path option_default_hsk_rules_file ()
{
    return main_path ().append ("hsk-scale-test");
}

/**
 * option_default_dif_rules_file:
 */
inline std::filesystem::path option_default_dif_rules_file ()
{
    return "";
}

/**
 * option_default_enf_rules_file:
 */
inline std::filesystem::path option_default_enf_rules_file ()
{
    return "";
}

/**
 * option_sync_with_controller: defines if the data plane stage should run without controller, or
 * if should connect to a local controller.
 */
constexpr bool option_sync_with_controller { false };

/**
 * option_execute_on_receive: defines if rules (mainly housekeeping) should execute on receive, or
 * if should be stored to be latter applied.
 */
constexpr bool option_execute_on_receive { true };

} // namespace padll::options

#endif // PADLL_OPTIONS_HPP
