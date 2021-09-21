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

/**
 * option_default_detailed_logging:
 */
const bool option_default_detailed_logging { true };

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
