/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef PADDL_OPTIONS_HPP
#define PADDL_OPTIONS_HPP

namespace padll {

/**
 * option_default_statistic_collection:
 */
const bool option_default_statistic_collection { true };

// *************************************************************************************************
//  Logging configuration
// *************************************************************************************************

/**
 * option_default_enable_debug_level:
 */
const bool option_default_enable_debug_level { true };

/**
 * option_default_enable_debug_with_ld_preload:
 */
const bool option_default_enable_debug_with_ld_preload { true };

/**
 * option_default_log_path:
 */
const std::string option_default_log_path { "/tmp/padll-info.log" };

/**
 * option_default_detailed_logging:
 */
const bool option_default_detailed_logging { false };

/**
 * option_default_table_format:
 */
const bool option_default_table_format { false };

// *************************************************************************************************
//  Default PAIO data plane stage configuration
// *************************************************************************************************

/**
 * option_default_stage_channels:
 */
const int option_default_stage_channels { 1 };

/**
 * option_default_stage_object_creation:
 */
const bool option_default_stage_object_creation { true };

/**
 * option_default_stage_name:
 */
const std::string option_default_stage_name { "tensorflow-" };

} // namespace padll

#endif // PADDL_OPTIONS_HPP
