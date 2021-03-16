/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef LDPAIO_OPTIONS_HPP
#define LDPAIO_OPTIONS_HPP

namespace ldpaio {

/**
 * option_default_statistic_collection:
 */
const bool option_default_statistic_collection { true };

/**
 * option_default_detailed_logging:
 */
const bool option_default_detailed_logging { false };

/**
 * option_default_table_format:
 */
const bool option_default_table_format { false };

/**
 * option_default_intercepted_lib:
 */
const std::string option_default_intercepted_lib { "libc.so.6" };


} // namespace ldpaio

#endif // LDPAIO_OPTIONS_HPP
