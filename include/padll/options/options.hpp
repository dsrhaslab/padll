/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#ifndef PADLL_OPTIONS_HPP
#define PADLL_OPTIONS_HPP

#include <filesystem>
#include <string>
#include <vector>

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
 * option_library_name: targeted dynamic library to LD_PRELOAD
 */
constexpr std::string_view option_library_name { "libc.so.6" };

/**
 * option_default_statistic_collection: option to enable/disable collection of LD_PRELOADED and
 * passthrough POSIX operations.
 */
constexpr bool option_default_statistic_collection { true };

/**
 * option_mount_point_differentiation_enabled: option to enable/disable mountpoint differentiation
 * and further selection of workflow identifiers (workflow-id to be submitted to the PAIO data
 * plane). All operations are considered with the same set of workflow identifiers
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
 * option_default_remote_mount_point: set the default main path of the remote mountpoint registry.
 * Operations will pick from a selected set of workflow identifiers.
 */
constexpr std::string_view option_default_remote_mount_point { "/tmp" };

/**
 * option_hard_remove: option to remove file descriptors from LdPreloadedPosix's m_mount_point_table
 * on ::close, even if the original fd was not registered due to process-based operations.
 */
constexpr bool option_hard_remove { false };

/**
 * option_default_metadata_server_unit: option to enable/disable the selection of a workflow-id for
 * a given MDS or MDT. This feature is still work-in-progress.
 */
constexpr bool option_select_workflow_by_metadata_unit { false };

/**
 * option_padll_workflows: get the number of internal workflows used by the PADLL data plane stage.
 * @return: returns the number of workflows to set in the data plane stage.
 *  May throw runtime_error exceptions if the padll_workflows are not set or are invalid values (<
 * 0).
 */
inline int option_padll_workflows ()
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
 * option_default_enable_debug_level: option to enable/disable DEBUG level logging (i.e., log_debug
 * messages).
 */
constexpr bool option_default_enable_debug_level { false };

/**
 * option_default_enable_debug_with_ld_preload:
 */
constexpr bool option_default_enable_debug_with_ld_preload { false };

/**
 * option_default_log_path: default path (and file extension) for PADLL logging files.
 */
constexpr std::string_view option_default_log_path { "/tmp/padll-info" };

/**
 * option_default_detailed_logging: option to enable/disable detailed logging. Recommended only for
 * debugging/instrumentation.
 */
#define OPTION_DETAILED_LOGGING false

/**
 * option_default_table_format: option to enable/disable visualization of statistics in tabular
 * format.
 */
constexpr bool option_default_table_format { false };

/**
 * option_default_save_statistics_report: option to enable/disable saving in a file the ldpreloaded
 * and passthrough statistics.
 */
constexpr bool option_default_save_statistics_report { true };

/**
 * option_default_statistics_report_path: main path to store the statistics files.
 */
constexpr std::string_view option_default_statistics_report_path { "/tmp" };

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
    return "/path/to/padll/files/";
}

/**
 * option_default_stage_channels: number of channels to create/consider in the PAIO data plane
 * stage. The -1 value will consider all channels specified in the HousekeepingRule file.
 */
constexpr int option_default_stage_channels { -1 };

/**
 * option_default_stage_object_creation: option to create default object (which will be of type
 * passthrough/noop). Default value will consider that all object are explicitly created through the
 * HousekeepingRule specification.
 */
constexpr bool option_default_stage_object_creation { false };

/**
 * option_default_stage_name: defaule name of the data plane stage.
 */
constexpr std::string_view option_default_stage_name { "padll-stage" };

/**
 * option_default_stage_name_env: environment variable to set the data plane stage name. If not set,
 * PADLL will consider the option_default_stage_name value. $ export paio_name="your-data-plane";
 */
constexpr std::string_view option_default_stage_name_env { "paio_name" };

/**
 * option_paio_environment_variable_env: environment variable to set additional information for the
 * stage (e.g., name of the application it is controlling, instance number when under multi-tenant
 * settings, ...). Only used in tests. $ export paio_env="app-tensorflow";
 */
constexpr std::string_view option_paio_environment_variable_env { "paio_env" };

/**
 * option_default_connection_address_env: environment variable to set the address to connect with
 * the control plane's local controller. If not set, PADLL will consider
 * paio::options::option_default_socket_name (). $ export cheferd_local_address="";
 */
constexpr std::string_view option_default_connection_address_env { "cheferd_local_address" };

/**
 * option_default_hsk_rules_file: path to housekeeping rules file, which will define the channels
 * and enforcement objects configurations. Only used when PADLL is executing in standalone mode (not
 * connected to a controller).
 */
inline std::filesystem::path option_default_hsk_rules_file ()
{
    return main_path ().append ("hsk-simple-test");
}

/**
 * option_default_dif_rules_file: path to differentiation rules file, which will define the channel
 * and enforcement objects classification and differentiation. Only used when PADLL is executing in
 * standalone mode (not connected to a controller).
 */
inline std::filesystem::path option_default_dif_rules_file ()
{
    return "";
}

/**
 * option_default_enf_rules_file: path to enforcement rules file, which will define the enforcement
 * objects settings to be ajusted. Only used when PADLL is executing in standalone mode (not
 * connected to a controller).
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
