/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
 **/

#ifndef PADDL_LOG_HPP
#define PADDL_LOG_HPP

#include <ctime>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <padll/library_headers/libc_headers.hpp>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <unistd.h>

using namespace padll::headers;

namespace padll::utils::log {

/**
 * Logging class.
 * This class contains the primitives to write logging messages of the data plane stage. Currently,
 * the class supports log messages of INFO, ERROR, and DEBUG qualifiers. Log messages can also be
 * written to stdout or file.
 */
class Log {

private:
    std::string m_logger_name { "basic_logger" };
    std::shared_ptr<spdlog::logger> m_basic_logger {};
    std::mutex m_lock;
    bool m_debug_enabled { true };
    bool m_is_ld_preloaded { true };
    int m_fd { STDOUT_FILENO };
    std::string m_log_file_path {};

    std::string create_file_name (const std::string& file_name);

    /**
     * initialize:
     */
    void initialize ();

    /**
     * cleanup:
     */
    void cleanup ();

    /**
     * create_formatted_message
     * @param message
     * @param level
     * @return
     */
    std::string create_formatted_message (const std::string& message, const std::string& level);

    /**
     * create_formatted_info_message:
     * @param message
     * @return
     */
    std::string create_formatted_info_message (const std::string& message);

    /**
     * create_formatted_error_message:
     * @param message
     * @return
     */
    std::string create_formatted_error_message (const std::string& message);

    /**
     * create_formatted_debug_message:
     * @param message
     * @return
     */
    std::string create_formatted_debug_message (const std::string& message);

    /**
     * dlsym_write_message:
     * @param fd
     * @param message
     * @return
     */
    ssize_t dlsym_write_message (int fd, const std::string& message);

public:
    /**
     * Log default constructor.
     */
    Log ();

    /**
     * Log parameterized constructor.
     * If @param debug is true, the logging mode is set to debug.
     * @param debug Boolean value that defines if the debug is enabled or disabled.
     */
    Log (const bool& enable_debug, const bool& debug_with_ldpreload, const std::string& log_file);

    /**
     * Log default destructor.
     */
    ~Log ();

    /**
     * log_info: Log a message with the INFO qualifier.
     * @param message Log message.
     */
    void log_info (const std::string& message);

    /**
     * log_error: Log a message with the ERROR qualifier.
     * @param message Log message.
     */
    void log_error (const std::string& message);

    /**
     * log_debug: Log a message with the DEBUG qualifier.
     * @param message Log message.
     */
    void log_debug (const std::string& message);
};
} // namespace padll::utils::log

#endif // PADDL_LOG_HPP
