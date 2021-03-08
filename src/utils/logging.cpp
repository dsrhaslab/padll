/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <ldpaio/utils/logging.hpp>

namespace ldpaio {

// static variable debug_enabled
bool Logging::m_debug_enabled = false;

// Logging default constructor.
Logging::Logging () = default;

// Logging parameterized constructor.
Logging::Logging (bool debug)
{
    if (debug) {
        set_debug ();
    }
}

// Logging default destructor.
Logging::~Logging () = default;

// set_debug call. Enabled debug messages.
void Logging::set_debug ()
{
    spdlog::set_level (spdlog::level::debug);
    m_debug_enabled = true;
}

// log_info call. Log message with INFO qualifier.
void Logging::log_info (const std::string& message)
{
    spdlog::info (message);
}

// log_error call. Log message with ERROR qualifier.
void Logging::log_error (const std::string& message)
{
    spdlog::error (message);
}

// log_debug call. Log message with DEBUG qualifier.
void Logging::log_debug (const std::string& message)
{
    spdlog::debug (message);
}

// is_debug_enabled call. Validate if debug messages are enabled.
bool Logging::is_debug_enabled ()
{
    return m_debug_enabled;
}

} // namespace paio
