/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
 **/

#include <padll/utils/log.hpp>

namespace padll::utils::log {

// Log default constructor.
Log::Log ()
{
    this->initialize ();
}

// Log parameterized constructor.
Log::Log (const bool& enable_debug, const bool& debug_with_ldpreload, const std::string& log_file) :
    m_debug_enabled { enable_debug },
    m_is_ld_preloaded { debug_with_ldpreload },
    m_log_file_path { this->create_file_name (log_file) }
{
    this->initialize ();
}

// Log default destructor.
Log::~Log ()
{
    this->cleanup ();
}

std::string Log::create_file_name (const std::string& file_name)
{
    std::string name {};
    if (!file_name.empty ()) {
        name = file_name + "-" + std::to_string (::getpid ()) + ".log";
    }

    return name;
}

// Log initialize call.
void Log::initialize ()
{
    std::lock_guard<std::mutex> guard (this->m_lock);

    // enable debug level at spdlog
    if (this->m_debug_enabled) {
        spdlog::set_level (spdlog::level::debug);
    }

    // bypass spdlog to prevent recursive dependency and/or null pointers to libc functions
    if (this->m_is_ld_preloaded) {
        if (!this->m_log_file_path.empty ()) {
            // open file using dlsym'ed close
            this->m_fd = ((libc_open_variadic_t)dlsym (RTLD_NEXT,
                "open")) (m_log_file_path.c_str (), O_CREAT | O_WRONLY | O_APPEND, 0666);

            // verify file descriptor result
            if (this->m_fd == -1) {
                perror ("Error in Log::initialize");
                this->m_fd = STDOUT_FILENO;
            }
        }
    }
}

// Log cleanup call.
void Log::cleanup ()
{
    std::lock_guard<std::mutex> guard (this->m_lock);
    if (this->m_fd != STDOUT_FILENO) {
        if (this->m_basic_logger.use_count () == 0) {
            // close file descriptor using dlsym'ed close
            auto return_value = ((libc_close_t)dlsym (RTLD_NEXT, "close")) (this->m_fd);

            // verify return_value
            if (return_value < 0) {
                perror ("Error on cleanup");
            }
        }
    }
}

// create_formatted_message call.
std::string Log::create_formatted_message (const std::string& message, const std::string& level)
{
    std::time_t current_time = std::time (nullptr);
    std::tm time_info = *std::localtime (&current_time);
    std::stringstream formatted_message;
    formatted_message << "[" << std::put_time (&time_info, "%F %T") << "] ";
    formatted_message << level;
    formatted_message << message << "\n";

    return formatted_message.str ();
}

// create_formatted_info_message call.
std::string Log::create_formatted_info_message (const std::string& message)
{
    return this->create_formatted_message (message, "[info] ");
}

// create_formatted_error_message call.
std::string Log::create_formatted_error_message (const std::string& message)
{
    return this->create_formatted_message (message, "[error] ");
}

// create_formatted_debug_message call.
std::string Log::create_formatted_debug_message (const std::string& message)
{
    return this->create_formatted_message (message, "[debug] ");
}

// dlsym_write_message call.
ssize_t Log::dlsym_write_message (int fd, const std::string& message)
{
    return ((libc_write_t)dlsym (RTLD_NEXT, "write")) (fd, message.c_str (), message.size ());
}

// log_info call. Log message with INFO qualifier.
void Log::log_info (const std::string& message)
{
    if (this->m_is_ld_preloaded) {
        // generate formatted info message
        auto msg = this->create_formatted_info_message (message);
        // write log message to file descriptor using dlsym'ed write
        auto return_value = this->dlsym_write_message (this->m_fd, msg);

        // verify return value of write operation
        if (return_value < 0) {
            perror ("Error on log_info");
        }
    } else {
        spdlog::info (message);
    }
}

// log_error call. Log message with ERROR qualifier.
void Log::log_error (const std::string& message)
{
    if (this->m_is_ld_preloaded) {
        // generate formatted error message
        auto msg = this->create_formatted_error_message (message);
        // write log message to file descriptor using dlsym'ed write
        auto return_value = this->dlsym_write_message (this->m_fd, msg);

        // verify return value of write operation
        if (return_value < 0) {
            perror ("Error on log_error");
        }
    } else {
        spdlog::error (message);
    }
}

// log_debug call. Log message with DEBUG qualifier.
void Log::log_debug (const std::string& message)
{
    if (this->m_is_ld_preloaded) {
        // generate formatted debug message
        auto msg = this->create_formatted_debug_message (message);
        // write log message to file descriptor using dlsym'ed write
        auto return_value = this->dlsym_write_message (this->m_fd, msg);

        // verify return value of write operation
        if (return_value < 0) {
            perror ("Error on log_debug");
        }
    } else {
        spdlog::debug (message);
    }
}

} // namespace padll::utils::log