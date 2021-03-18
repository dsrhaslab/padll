/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef PADDL_LOGGING_HPP
#define PADDL_LOGGING_HPP

#include <iostream>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace padll {

/**
 * Logging class.
 * This class contains the primitives to write logging messages of the data plane stage. Currently,
 * the class supports log messages of INFO, ERROR, and DEBUG qualifiers. Log messages can also be
 * written to stdout or file.
 * TODO: currently, the logger only reports to stdout. Persist logging messages as future work.
 */
class Logging {

private:
    std::string m_logger_name { "basic_logger" };
    std::string m_log_file_path { "/tmp/padll_info.txt" };
    std::shared_ptr<spdlog::logger> m_logger {};

    /**
     * Enable logging debug messages.
     */
    void set_debug ();

public:
    static bool m_debug_enabled;

    /**
     * Logging default constructor.
     */
    Logging ();

    /**
     * Logging parameterized constructor.
     * If @param debug is true, the logging mode is set to debug.
     * @param debug Boolean value that defines if the debug is enabled or disabled.
     */
    explicit Logging (bool debug);

    /**
     * Logging default destructor.
     */
    ~Logging ();

    /**
     * log_info: Log a message with the INFO qualifier.
     * @param message Log message.
     */
    static void log_info (const std::string& message);

    /**
     * log_error: Log a message with the ERROR qualifier.
     * @param message Log message.
     */
    static void log_error (const std::string& message);

    /**
     * log_debug: Log a message with the DEBUG qualifier.
     * @param message Log message.
     */
    static void log_debug (const std::string& message);

    /**
     * is_debug_enabled: Validate if debugging is enabled (i.e., writing messages with the DEBUG
     * qualifier). This is useful for preventing the construction of debug messages, which can incur
     * high overhead if called for each I/O request.
     * @return Returns a boolean that defines whether debug is enabled or not.
     */
    static bool is_debug_enabled ();
};
} // namespace padll

#endif // PADDL_LOGGING_HPP
