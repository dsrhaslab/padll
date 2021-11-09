/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef PADDL_OPTIONS_HPP
#define PADDL_OPTIONS_HPP

#include <string>

namespace padll {

/***************************************************************************************************
 * PADLL default configurations
 **************************************************************************************************/

const std::string option_library_name { "libc.so.6" };

/**
 * option_default_statistic_collection:
 */
constexpr bool option_default_statistic_collection { true };

/***************************************************************************************************
 * Logging configuration
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
const std::string option_default_log_path { "/tmp/padll-info.log" };

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
const std::string option_default_stage_name { "tensorflow-" };

} // namespace padll

#endif // PADDL_OPTIONS_HPP
