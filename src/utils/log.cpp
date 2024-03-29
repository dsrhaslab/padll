/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#include <padll/utils/log.hpp>

namespace padll::utils::log {

// Log default constructor.
Log::Log ()
{
    // initialize log object
    this->initialize ();
    std::stringstream stream;
    stream << "Log default constructor (" << static_cast<void*> (this) << ").";
    this->log_info (stream.str ());
}

// Log parameterized constructor.
Log::Log (const bool& enable_debug, const bool& debug_with_ldpreload, const std::string& log_file) :
    m_debug_enabled { enable_debug },
    m_is_ld_preloaded { debug_with_ldpreload },
    m_log_file_path { this->create_file_name (log_file) }
{
    // initialize log object
    this->initialize ();
    // write debug message
    std::stringstream stream;
    stream << "Log parameterized constructor (" << static_cast<void*> (this) << ").";
    this->log_info (stream.str ());
}

// Log default destructor.
Log::~Log ()
{
    // write debug message
    std::stringstream stream;
    stream << "Log default destructor (" << static_cast<void*> (this) << ").";
    this->log_info (stream.str ());
    // cleanup log object
    this->cleanup ();
    // close library linking
    if (this->m_dl_handle != nullptr) {
        ::dlclose (this->m_dl_handle);
    }
}

// create_file_name call. (...)
std::string Log::create_file_name (const std::string& file_name) const
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
    // lock_guard over mutex
    std::lock_guard guard (this->m_lock);

    // enable debug level at spdlog
    if (this->m_debug_enabled) {
        spdlog::set_level (spdlog::level::debug);
    }

    // bypass spdlog to prevent recursive dependency and/or null pointers to libc functions
    if (this->m_is_ld_preloaded) {
        // point m_dl_handle to libc
        this->m_dl_handle = ::dlopen ("libc.so.6", RTLD_LAZY);

        // validate if pointer was successfully attributed
        if (this->m_dl_handle == nullptr) {
            // write message to stderr
            std::fprintf (stderr,
                "%s: Error while loading libc.so ... assigning RTLD_NEXT\n",
                __func__);
            // assign RTLD_NEXT to m_dl_handle
            this->m_dl_handle = RTLD_NEXT;
        }

        if (!this->m_log_file_path.empty ()) {
            // open file using dlsym'ed close
            this->m_fd = ((libc_open_variadic_t)dlsym (this->m_dl_handle,
                "open")) (m_log_file_path.c_str (), O_CREAT | O_WRONLY | O_APPEND, 0666);

            // verify file descriptor result
            if (this->m_fd == -1) {
                perror ("Log::Error in initialize");
                this->m_fd = STDOUT_FILENO;
            }
        }
    }
}

// Log cleanup call.
void Log::cleanup ()
{
    // lock_guard over mutex
    std::lock_guard guard (this->m_lock);
    if (this->m_fd != STDOUT_FILENO && this->m_basic_logger.use_count () == 0) {
        // close file descriptor using dlsym'ed close
        auto return_value = ((libc_close_t)dlsym (this->m_dl_handle, "close")) (this->m_fd);

        // verify return_value
        if (return_value < 0) {
            perror ("Log::Error on cleanup");
        }
    }
}

// create_formatted_message call.
std::string Log::create_formatted_message (const std::string& message,
    const std::string& level) const
{
    std::time_t current_time = std::time (nullptr);
    struct std::tm time_info;
    localtime_r (&current_time, &time_info);

    std::stringstream formatted_message;
    formatted_message << "[" << std::put_time (&time_info, "%F %T") << "] ";
    formatted_message << level;
    formatted_message << message << "\n";

    return formatted_message.str ();
}

// create_formatted_info_message call.
std::string Log::create_formatted_info_message (const std::string& message) const
{
    return this->create_formatted_message (message, "[info] ");
}

// create_formatted_error_message call.
std::string Log::create_formatted_error_message (const std::string& message) const
{
    return this->create_formatted_message (message, "[error] ");
}

// create_formatted_debug_message call.
std::string Log::create_formatted_debug_message (const std::string& message) const
{
    return this->create_formatted_message (message, "[debug] ");
}

// dlsym_write_message call.
ssize_t Log::dlsym_write_message (int fd, const std::string& message) const
{
#ifdef __linux__
    return (
        (libc_write_t)dlsym (this->m_dl_handle, "write")) (fd, message.c_str (), message.size ());
#else
    return ((libc_write_t)dlsym (RTLD_NEXT, "write")) (fd, message.c_str (), message.size ());
#endif
}

// log_info call. Log message with INFO qualifier.
void Log::log_info (const std::string& message) const
{
    if (this->m_is_ld_preloaded) {
        // generate formatted info message
        auto msg = this->create_formatted_info_message (message);
        // write log message to file descriptor using dlsym'ed write
        auto return_value = this->dlsym_write_message (this->m_fd, msg);

        // verify return value of write operation
        if (return_value < 0) {
            perror ("Log::Error on log_info");
        }
    } else {
        spdlog::info (message);
    }
}

// log_error call. Log message with ERROR qualifier.
void Log::log_error (const std::string& message) const
{
    if (this->m_is_ld_preloaded) {
        // generate formatted error message
        auto msg = this->create_formatted_error_message (message);
        // write log message to file descriptor using dlsym'ed write
        auto return_value = this->dlsym_write_message (this->m_fd, msg);

        // verify return value of write operation
        if (return_value < 0) {
            perror ("Log::Error on log_error");
        }
    } else {
        spdlog::error (message);
    }
}

// log_debug call. Log message with DEBUG qualifier.
void Log::log_debug (const std::string& message) const
{
    if (this->m_is_ld_preloaded) {
        // generate formatted debug message
        auto msg = this->create_formatted_debug_message (message);
        // write log message to file descriptor using dlsym'ed write
        auto return_value = this->dlsym_write_message (this->m_fd, msg);

        // verify return value of write operation
        if (return_value < 0) {
            perror ("Log::Error on log_debug");
        }
    } else {
        spdlog::debug (message);
    }
}

// create_routine_log_message call. (...)
void Log::create_routine_log_message (const char* routine_name, const std::string_view& arg) const
{
    // create logging message
    std::string message;
    message.append (routine_name);
    message.append (" (").append ((arg.empty ()) ? "<empty>" : arg).append (")");

    // submit message to debug logging routine
    this->log_debug (message);
}

// create_routine_log_message call. (...)
void Log::create_routine_log_message (const char* routine_name,
    const std::string_view& arg1,
    const std::string_view& arg2) const
{
    // create logging message
    std::string message;
    message.append (routine_name);
    message.append (" (").append ((arg1.empty ()) ? "<empty>" : arg1).append (", ");
    message.append ((arg2.empty ()) ? "<empty>" : arg2).append (")");

    // submit message to debug logging routine
    this->log_debug (message);
}

} // namespace padll::utils::log
