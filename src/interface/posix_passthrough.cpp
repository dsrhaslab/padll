/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <padll/interface/posix_passthrough.hpp>

namespace padll {

// PosixPassthrough default constructor.
PosixPassthrough::PosixPassthrough ()
{
    // initialize library handle pointer
    this->initialize ();
}

// PosixPassthrough parameterized constructor.
PosixPassthrough::PosixPassthrough (const std::string& lib, bool stat_collection) :
    m_lib_name { lib },
    m_collect { stat_collection }
{
    // validate if 'lib' is valid
    if (lib.empty ()) {
        Logging::log_error ("Library not valid.");
        return;
    }

    // initialize library handle pointer
    this->initialize ();
}

// PosixPassthrough default destructor.
PosixPassthrough::~PosixPassthrough ()
{
    Logging::log_info ("PosixPassthrough default destructor.");

    // validate if library handle is valid and close dynamic linking
    if (this->m_lib_handle != nullptr) {
        // close dynamic linking to intercepted library.
        // It decrements the reference count on the dynamically loaded shared object, referred to
        // by handle m_lib_handle. If the reference count drops to zero, then the object is
        // unloaded. All shared objects that were automatically loaded when dlopen () was invoked
        // on the object referred to by handle are recursively closed in the same manner.
        int dlclose_result = ::dlclose (this->m_lib_handle);

        // validate result from dlclose
        if (dlclose_result != 0) {
            Logging::log_error (
                "Error while closing dynamic link (" + std::to_string (dlclose_result) + ").");
        }
    }

    if (option_default_table_format) {
        // print to stdout metadata-based statistics in tabular format
        this->m_metadata_stats.tabulate ();
        std::cout << "\n";

        // print to stdout data-based statistics in tabular format
        this->m_data_stats.tabulate ();
        std::cout << "\n";

        // print to stdout directory-based statistics in tabular format
        this->m_dir_stats.tabulate ();
        std::cout << "\n";

        // print to stdout extended attributes based statistics in tabular format
        this->m_ext_attr_stats.tabulate ();
        std::cout << "\n";

        // print to stdout file modes based statistics in tabular format
        this->m_file_mode_stats.tabulate ();
        std::cout << "\n";
    } else {
        Logging::log_debug (this->to_string ());
    }
}

// dlopen_library_handle call. (...)
bool PosixPassthrough::dlopen_library_handle ()
{
    std::unique_lock<std::mutex> unique_lock (this->m_lock);
    // Dynamic loading of the libc library (referred to as 'libc.so.6').
    // loads the dynamic shared object (shared library) file named by the null-terminated string
    // filename and returns an opaque "handle" for the loaded object.
    this->m_lib_handle = ::dlopen (this->m_lib_name.data (), RTLD_LAZY);

    // return true if the m_lib_handle is valid, and false otherwise.
    return (this->m_lib_handle != nullptr);
}

// initialize call. (...)
void PosixPassthrough::initialize ()
{
    // open library and assign pointer to m_lib_handle
    bool open_lib_handle = this->dlopen_library_handle ();

    // validate library pointer
    if (!open_lib_handle) {
        Logging::log_error ("Error while dlopen'ing " + this->m_lib_name + ".");
        return;
    }
}

// set_statistic_collection call. (...)
void PosixPassthrough::set_statistic_collection (bool value)
{
    this->m_collect.store (value);
}

// get_statistic_entry call.
StatisticEntry PosixPassthrough::get_statistic_entry (const OperationType& operation_type,
    const int& operation_entry)
{
    switch (operation_type) {
        case OperationType::metadata_calls:
            return this->m_metadata_stats.get_statistic_entry (operation_entry);

        case OperationType::data_calls:
            return this->m_data_stats.get_statistic_entry (operation_entry);

        case OperationType::directory_calls:
            return this->m_dir_stats.get_statistic_entry (operation_entry);

        case OperationType::ext_attr_calls:
            return this->m_ext_attr_stats.get_statistic_entry (operation_entry);

        case OperationType::file_mode_calls:
            return this->m_file_mode_stats.get_statistic_entry (operation_entry);

        default:
            return StatisticEntry {};
    }
}

// to_string call. (...)
std::string PosixPassthrough::to_string ()
{
    std::stringstream stream;
    stream << "PosixPassthrough {\n";
    stream << "\t" << this->m_metadata_stats.to_string () << "\n";
    stream << "\t" << this->m_data_stats.to_string () << "\n";
    stream << "\t" << this->m_dir_stats.to_string () << "\n";
    stream << "\t" << this->m_ext_attr_stats.to_string () << "\n";
    stream << "\t" << this->m_file_mode_stats.to_string () << "\n";
    stream << "}";

    return stream.str ();
}

// passthrough_read call.
ssize_t PosixPassthrough::passthrough_read (int fd, void* buf, ssize_t counter)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-read (" + std::to_string (fd) + ")");
    }

    // validate function and library handle pointers
    if (!m_data_operations.m_read && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_data_operations.m_read = (libc_read_t)dlsym (this->m_lib_handle, "read")
            : m_data_operations.m_read = (libc_read_t)dlsym (RTLD_NEXT, "read");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_data_operations.m_read) {
        m_data_operations.m_read = (libc_read_t)dlsym (this->m_lib_handle, "read");
    }

    // perform original POSIX read operation
    ssize_t result = m_data_operations.m_read (fd, buf, counter);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::read), 1, result);
        } else {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::read), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_write call.
ssize_t PosixPassthrough::passthrough_write (int fd, const void* buf, ssize_t counter)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-write (" + std::to_string (fd) + ")");
    }

    // validate function and library handle pointers
    if (!m_data_operations.m_write && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_data_operations.m_write = (libc_write_t)dlsym (this->m_lib_handle, "write")
            : m_data_operations.m_write = (libc_write_t)dlsym (RTLD_NEXT, "write");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_data_operations.m_write) {
        m_data_operations.m_write = (libc_write_t)dlsym (this->m_lib_handle, "write");
    }

    // perform original POSIX write operation
    ssize_t result = m_data_operations.m_write (fd, buf, counter);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::write), 1, result);
        } else {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::write), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_pread call.
ssize_t PosixPassthrough::passthrough_pread (int fd, void* buf, ssize_t counter, off_t offset)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-pread (" + std::to_string (fd) + ")");
    }

    // validate function and library handle pointers
    if (!m_data_operations.m_pread && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_data_operations.m_pread = (libc_pread_t)dlsym (this->m_lib_handle, "pread")
            : m_data_operations.m_pread = (libc_pread_t)dlsym (RTLD_NEXT, "pread");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_data_operations.m_pread) {
        m_data_operations.m_pread = (libc_pread_t)dlsym (this->m_lib_handle, "pread");
    }

    // perform original POSIX pread operation
    ssize_t result = m_data_operations.m_pread (fd, buf, counter, offset);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::pread), 1, result);
        } else {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::pread), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_pwrite call.
ssize_t
PosixPassthrough::passthrough_pwrite (int fd, const void* buf, ssize_t counter, off_t offset)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-pwrite (" + std::to_string (fd) + ")");
    }

    // validate function and library handle pointers
    if (!m_data_operations.m_pwrite && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_data_operations.m_pwrite = (libc_pwrite_t)dlsym (this->m_lib_handle, "pwrite")
            : m_data_operations.m_pwrite = (libc_pwrite_t)dlsym (RTLD_NEXT, "pwrite");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_data_operations.m_pwrite) {
        m_data_operations.m_pwrite = (libc_pwrite_t)dlsym (this->m_lib_handle, "pwrite");
    }

    // perform original POSIX pwrite operation
    ssize_t result = m_data_operations.m_pwrite (fd, buf, counter, offset);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::pwrite), 1, result);
        } else {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::pwrite), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_fread call. (...)
size_t PosixPassthrough::passthrough_fread (void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-fpread");
    }

    // validate function and library handle pointers
    if (!m_data_operations.m_fread && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_data_operations.m_fread = (libc_fread_t)dlsym (this->m_lib_handle, "fread")
            : m_data_operations.m_fread = (libc_fread_t)dlsym (RTLD_NEXT, "fread");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_data_operations.m_fread) {
        m_data_operations.m_fread = (libc_fread_t)dlsym (this->m_lib_handle, "fread");
    }

    // perform original POSIX fread operation
    ssize_t result = m_data_operations.m_fread (ptr, size, nmemb, stream);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::fread), 1, result);
        } else {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::fread), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_fwrite call. (...)
size_t
PosixPassthrough::passthrough_fwrite (const void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-fwrite");
    }

    // validate function and library handle pointers
    if (!m_data_operations.m_fwrite && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_data_operations.m_fwrite = (libc_fwrite_t)dlsym (this->m_lib_handle, "fwrite")
            : m_data_operations.m_fwrite = (libc_fwrite_t)dlsym (RTLD_NEXT, "fwrite");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_data_operations.m_fwrite) {
        m_data_operations.m_fwrite = (libc_fwrite_t)dlsym (this->m_lib_handle, "fwrite");
    }

    // perform original POSIX fwrite operation
    ssize_t result = m_data_operations.m_fwrite (ptr, size, nmemb, stream);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::fwrite), 1, result);
        } else {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::fwrite), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_open call.
int PosixPassthrough::passthrough_open (const char* path, int flags, mode_t mode)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-open-variadic (" + std::string (path) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_open_var && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_open_var
            = (libc_open_variadic_t)dlsym (this->m_lib_handle, "open")
            : m_metadata_operations.m_open_var = (libc_open_variadic_t)dlsym (RTLD_NEXT, "open");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_open_var) {
        m_metadata_operations.m_open_var = (libc_open_variadic_t)dlsym (this->m_lib_handle, "open");
    }

    // perform original POSIX open operation
    int result = m_metadata_operations.m_open_var (path, flags, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (
                static_cast<int> (Metadata::open_variadic),
                1,
                0);
        } else {
            this->m_metadata_stats
                .update_statistic_entry (static_cast<int> (Metadata::open_variadic), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_open call.
int PosixPassthrough::passthrough_open (const char* path, int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-open (" + std::string (path) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_open && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_open = (libc_open_t)dlsym (this->m_lib_handle, "open")
            : m_metadata_operations.m_open = (libc_open_t)dlsym (RTLD_NEXT, "open");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_open) {
        m_metadata_operations.m_open = (libc_open_t)dlsym (this->m_lib_handle, "open");
    }

    // perform original POSIX open operation
    int result = m_metadata_operations.m_open (path, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::open), 1, 0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::open),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_creat call.
int PosixPassthrough::passthrough_creat (const char* path, mode_t mode)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-creat (" + std::string (path) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_creat && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_creat = (libc_creat_t)dlsym (this->m_lib_handle, "creat")
            : m_metadata_operations.m_creat = (libc_creat_t)dlsym (RTLD_NEXT, "creat");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_creat) {
        m_metadata_operations.m_creat = (libc_creat_t)dlsym (this->m_lib_handle, "creat");
    }

    // perform original POSIX creat operation
    int result = m_metadata_operations.m_creat (path, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::creat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::creat),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_openat call.
int PosixPassthrough::passthrough_openat (int dirfd, const char* path, int flags, mode_t mode)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-openat-variadic (" + std::to_string (dirfd) + ", "
            + std::string (path) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_openat_var && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ()) ? m_metadata_operations.m_openat_var
            = (libc_openat_variadic_t)dlsym (this->m_lib_handle, "openat")
                                         : m_metadata_operations.m_openat_var
            = (libc_openat_variadic_t)dlsym (RTLD_NEXT, "openat");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_openat_var) {
        m_metadata_operations.m_openat_var
            = (libc_openat_variadic_t)dlsym (this->m_lib_handle, "openat");
    }

    // perform original POSIX openat operation
    int result = m_metadata_operations.m_openat_var (dirfd, path, flags, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (
                static_cast<int> (Metadata::openat_variadic),
                1,
                0);
        } else {
            this->m_metadata_stats
                .update_statistic_entry (static_cast<int> (Metadata::openat_variadic), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_openat call.
int PosixPassthrough::passthrough_openat (int dirfd, const char* path, int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-openat (" + std::to_string (dirfd) + ", " + std::string (path) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_openat && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_openat = (libc_openat_t)dlsym (this->m_lib_handle, "openat")
            : m_metadata_operations.m_openat = (libc_openat_t)dlsym (RTLD_NEXT, "openat");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_openat) {
        m_metadata_operations.m_openat = (libc_openat_t)dlsym (this->m_lib_handle, "openat");
    }

    // perform original POSIX openat operation
    int result = m_metadata_operations.m_openat (dirfd, path, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::openat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::openat),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_open64 call. (...)
int PosixPassthrough::passthrough_open64 (const char* path, int flags, mode_t mode)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-open64-variadic (" + std::string (path) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_open64_var && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ()) ? m_metadata_operations.m_open64_var
            = (libc_open64_variadic_t)dlsym (this->m_lib_handle, "open64")
                                         : m_metadata_operations.m_open64_var
            = (libc_open64_variadic_t)dlsym (RTLD_NEXT, "open64");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_open64_var) {
        m_metadata_operations.m_open64_var
            = (libc_open64_variadic_t)dlsym (this->m_lib_handle, "open64");
    }

    // perform original POSIX open64 operation
    int result = m_metadata_operations.m_open64_var (path, flags, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (
                static_cast<int> (Metadata::open64_variadic),
                1,
                0);
        } else {
            this->m_metadata_stats
                .update_statistic_entry (static_cast<int> (Metadata::open64_variadic), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_open64 call. (...)
int PosixPassthrough::passthrough_open64 (const char* path, int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-open64 (" + std::string (path) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_open64 && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_open64 = (libc_open64_t)dlsym (this->m_lib_handle, "open64")
            : m_metadata_operations.m_open64 = (libc_open64_t)dlsym (RTLD_NEXT, "open64");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_open64) {
        m_metadata_operations.m_open64 = (libc_open64_t)dlsym (this->m_lib_handle, "open64");
    }

    // perform original POSIX open64 operation
    int result = m_metadata_operations.m_open64 (path, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::open64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::open64),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_close call.
int PosixPassthrough::passthrough_close (int fd)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-close (" + std::to_string (fd) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_close && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
        ? m_metadata_operations.m_close = (libc_close_t)dlsym (this->m_lib_handle, "close")
        : m_metadata_operations.m_close = (libc_close_t)dlsym (RTLD_NEXT, "close");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_close) {
        m_metadata_operations.m_close = (libc_close_t)dlsym (this->m_lib_handle, "close");
    }

    // perform original POSIX close operation
    int result = m_metadata_operations.m_close (fd);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::close),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::close),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_fsync call. (...)
int PosixPassthrough::passthrough_fsync (int fd)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-fsync (" + std::to_string (fd) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_fsync && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
        ? m_metadata_operations.m_fsync = (libc_fsync_t)dlsym (this->m_lib_handle, "fsync")
        : m_metadata_operations.m_fsync = (libc_fsync_t)dlsym (RTLD_NEXT, "fsync");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_fsync) {
        m_metadata_operations.m_fsync = (libc_fsync_t)dlsym (this->m_lib_handle, "fsync");
    }

    // perform original POSIX fsync operation
    int result = m_metadata_operations.m_fsync (fd);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fsync),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fsync),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_fdatasync call. (...)
int PosixPassthrough::passthrough_fdatasync (int fd)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-fdatasync (" + std::to_string (fd) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_fdatasync && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
        ? m_metadata_operations.m_fdatasync = (libc_fdatasync_t)dlsym (this->m_lib_handle, "fdatasync")
        : m_metadata_operations.m_fdatasync = (libc_fdatasync_t)dlsym (RTLD_NEXT, "fdatasync");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_fdatasync) {
        m_metadata_operations.m_fdatasync = (libc_fdatasync_t)dlsym (this->m_lib_handle, "fdatasync");
    }

    // perform original POSIX fdatasync operation
    int result = m_metadata_operations.m_fdatasync (fd);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fdatasync),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fdatasync),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_sync call. (...)
void PosixPassthrough::passthrough_sync ()
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-sync");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_sync && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
        ? m_metadata_operations.m_sync = (libc_sync_t)dlsym (this->m_lib_handle, "sync")
        : m_metadata_operations.m_sync = (libc_sync_t)dlsym (RTLD_NEXT, "sync");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_sync) {
        m_metadata_operations.m_sync = (libc_sync_t)dlsym (this->m_lib_handle, "sync");
    }

    // perform original POSIX sync operation
    m_metadata_operations.m_sync ();

    // update statistic entry
    if (this->m_collect) {
        this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::sync), 1, 0);
    }
}

// passthrough_syncfs call. (...)
int PosixPassthrough::passthrough_syncfs (int fd)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-syncfs (" + std::to_string (fd) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_syncfs && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
        ? m_metadata_operations.m_syncfs = (libc_syncfs_t)dlsym (this->m_lib_handle, "syncfs")
        : m_metadata_operations.m_syncfs = (libc_syncfs_t)dlsym (RTLD_NEXT, "syncfs");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_syncfs) {
        m_metadata_operations.m_syncfs = (libc_syncfs_t)dlsym (this->m_lib_handle, "syncfs");
    }

    // perform original POSIX syncfs operation
    int result = m_metadata_operations.m_syncfs (fd);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::syncfs),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::syncfs),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_truncate call. (...)
int PosixPassthrough::passthrough_truncate (const char* path, off_t length)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-truncate (" + std::string (path) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_truncate && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
        ? m_metadata_operations.m_truncate = (libc_truncate_t)dlsym (this->m_lib_handle, "truncate")
        : m_metadata_operations.m_truncate = (libc_truncate_t)dlsym (RTLD_NEXT, "truncate");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_truncate) {
        m_metadata_operations.m_truncate = (libc_truncate_t)dlsym (this->m_lib_handle, "truncate");
    }

    // perform original POSIX truncate operation
    int result = m_metadata_operations.m_truncate (path, length);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::truncate),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::truncate),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_truncate call. (...)
int PosixPassthrough::passthrough_ftruncate (int fd, off_t length)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-truncate (" + std::to_string (fd) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_ftruncate && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
        ? m_metadata_operations.m_ftruncate = (libc_ftruncate_t)dlsym (this->m_lib_handle, "ftruncate")
        : m_metadata_operations.m_ftruncate = (libc_ftruncate_t)dlsym (RTLD_NEXT, "ftruncate");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_ftruncate) {
        m_metadata_operations.m_ftruncate = (libc_ftruncate_t)dlsym (this->m_lib_handle, "ftruncate");
    }

    // perform original POSIX ftruncate operation
    int result = m_metadata_operations.m_ftruncate (fd, length);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::ftruncate),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::ftruncate),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_xstat call. (...)
int PosixPassthrough::passthrough_xstat (int version, const char* path, struct stat* statbuf)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-xstat (" + std::string (path) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_xstat && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
        ? m_metadata_operations.m_xstat = (libc_xstat_t)dlsym (this->m_lib_handle, "__xstat")
        : m_metadata_operations.m_xstat = (libc_xstat_t)dlsym (RTLD_NEXT, "__xstat");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_xstat) {
        m_metadata_operations.m_xstat = (libc_xstat_t)dlsym (this->m_lib_handle, "__xstat");
    }

    // perform original POSIX __xstat (stat) operation
    // int result = ((libc_xstat_t)dlsym (this->m_lib_handle, "__xstat")) (version, path, statbuf);
    int result = m_metadata_operations.m_xstat (version, path, statbuf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::stat), 1, 0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::stat),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_lxstat call. (...)
int PosixPassthrough::passthrough_lxstat (int version, const char* path, struct stat* statbuf)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-lxstat (" + std::string (path) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_lxstat && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
        ? m_metadata_operations.m_lxstat = (libc_lxstat_t)dlsym (this->m_lib_handle, "__lxstat")
        : m_metadata_operations.m_lxstat = (libc_lxstat_t)dlsym (RTLD_NEXT, "__lxstat");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_lxstat) {
        m_metadata_operations.m_lxstat = (libc_lxstat_t)dlsym (this->m_lib_handle, "__lxstat");
    }

    // perform original POSIX __lxstat (lstat) operation
    // int result = ((libc_lxstat_t)dlsym (this->m_lib_handle, "__lxstat")) (version, path, statbuf);
    int result = m_metadata_operations.m_lxstat (version, path, statbuf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::lstat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::lstat),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_fxstat call. (...)
int PosixPassthrough::passthrough_fxstat (int version, int fd, struct stat* statbuf)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-fxstat (" + std::to_string (fd) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_fxstat && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
        ? m_metadata_operations.m_fxstat = (libc_fxstat_t)dlsym (this->m_lib_handle, "__fxstat")
        : m_metadata_operations.m_fxstat = (libc_fxstat_t)dlsym (RTLD_NEXT, "__fxstat");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_fxstat) {
        m_metadata_operations.m_fxstat = (libc_fxstat_t)dlsym (this->m_lib_handle, "__fxstat");
    }

    // perform original POSIX __fxstat (fstat) operation
    // int result = ((libc_fxstat_t)dlsym (this->m_lib_handle, "__fxstat")) (version, fd, statbuf);
    int result = m_metadata_operations.m_fxstat (version, fd, statbuf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstat),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_fxstatat call. (...)
int PosixPassthrough::passthrough_fxstatat (int version,
    int dirfd,
    const char* path,
    struct stat* statbuf,
    int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-fxstatat (" + std::to_string (dirfd) + ", " + std::string (path) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_fxstatat && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
        ? m_metadata_operations.m_fxstatat = (libc_fxstatat_t)dlsym (this->m_lib_handle, "__fxstatat")
        : m_metadata_operations.m_fxstatat = (libc_fxstatat_t)dlsym (RTLD_NEXT, "__fxstatat");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_fxstatat) {
        m_metadata_operations.m_fxstatat = (libc_fxstatat_t)dlsym (this->m_lib_handle, "__fxstatat");
    }

    // perform original POSIX __fxstatat (fstatat) operation
    // int result = ((libc_fxstatat_t)dlsym (this->m_lib_handle,
    //     "__fxstatat")) (version, dirfd, path, statbuf, flags);
    int result = m_metadata_operations.m_fxstatat (version, dirfd, path, statbuf, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstatat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstatat),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_statfs call. (...)
int PosixPassthrough::passthrough_statfs (const char* path, struct statfs* buf)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-statfs (" + std::string (path) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_statfs && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
        ? m_metadata_operations.m_statfs = (libc_statfs_t)dlsym (this->m_lib_handle, "statfs")
        : m_metadata_operations.m_statfs = (libc_statfs_t)dlsym (RTLD_NEXT, "statfs");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_statfs) {
        m_metadata_operations.m_statfs = (libc_statfs_t)dlsym (this->m_lib_handle, "statfs");
    }

    // perform original POSIX statfs operation
    int result = m_metadata_operations.m_statfs (path, buf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::statfs),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::statfs),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_fstatfs call. (...)
int PosixPassthrough::passthrough_fstatfs (int fd, struct statfs* buf)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-fstatfs (" + std::to_string (fd) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_fstatfs && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
        ? m_metadata_operations.m_fstatfs = (libc_fstatfs_t)dlsym (this->m_lib_handle, "fstatfs")
        : m_metadata_operations.m_fstatfs = (libc_fstatfs_t)dlsym (RTLD_NEXT, "fstatfs");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_fstatfs) {
        m_metadata_operations.m_fstatfs = (libc_fstatfs_t)dlsym (this->m_lib_handle, "fstatfs");
    }

    // perform original POSIX fstatfs operation
    int result = m_metadata_operations.m_fstatfs (fd, buf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstatfs),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstatfs),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_link call. (...)
int PosixPassthrough::passthrough_link (const char* old_path, const char* new_path)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-link (" + std::string (old_path) + ", " + std::string (new_path) + ")");
    }

    // perform original POSIX link operation
    int result = ((libc_link_t)dlsym (this->m_lib_handle, "link")) (old_path, new_path);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::link), 1, 0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::link),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_unlink call. (...)
int PosixPassthrough::passthrough_unlink (const char* path)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-unlink (" + std::string (path) + ")");
    }

    // perform original POSIX unlink operation
    int result = ((libc_unlink_t)dlsym (this->m_lib_handle, "unlink")) (path);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::unlink),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::unlink),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_linkat call. (...)
int PosixPassthrough::passthrough_linkat (int olddirfd,
    const char* old_path,
    int newdirfd,
    const char* new_path,
    int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-linkat (" + std::to_string (olddirfd) + ", "
            + std::string (old_path) + ", " + std::to_string (newdirfd) + ", "
            + std::string (new_path) + ")");
    }

    // perform original POSIX linkat operation
    int result = ((libc_linkat_t)dlsym (this->m_lib_handle,
        "linkat")) (olddirfd, old_path, newdirfd, new_path, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::linkat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::linkat),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_unlinkat call. (...)
int PosixPassthrough::passthrough_unlinkat (int dirfd, const char* pathname, int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-unlinkat (" + std::to_string (dirfd) + ", "
            + std::string (pathname) + ", " + std::to_string (flags) + ")");
    }

    // perform original POSIX unlinkat operation
    int result = ((libc_unlinkat_t)dlsym (this->m_lib_handle, "unlinkat")) (dirfd, pathname, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::unlinkat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::unlinkat),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_rename call. (...)
int PosixPassthrough::passthrough_rename (const char* old_path, const char* new_path)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-rename (" + std::string (old_path) + ", " + std::string (new_path) + ")");
    }

    // perform original POSIX rename operation
    int result = ((libc_rename_t)dlsym (this->m_lib_handle, "rename")) (old_path, new_path);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::rename),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::rename),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_renameat call. (...)
int PosixPassthrough::passthrough_renameat (int olddirfd,
    const char* old_path,
    int newdirfd,
    const char* new_path)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-renameat (" + std::to_string (olddirfd) + ", "
            + std::string (old_path) + ", " + std::to_string (newdirfd) + ", "
            + std::string (new_path) + ")");
    }

    // perform original POSIX renameat operation
    int result = ((libc_renameat_t)dlsym (this->m_lib_handle,
        "renameat")) (olddirfd, old_path, newdirfd, new_path);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::renameat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::renameat),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_symlink call. (...)
int PosixPassthrough::passthrough_symlink (const char* target, const char* linkpath)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-symlink (" + std::string (target) + ", " + std::string (linkpath) + ")");
    }

    // perform original POSIX symlink operation
    int result = ((libc_symlink_t)dlsym (this->m_lib_handle, "symlink")) (target, linkpath);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::symlink),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::symlink),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_symlinkat call. (...)
int PosixPassthrough::passthrough_symlinkat (const char* target, int newdirfd, const char* linkpath)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-symlinkat (" + std::string (target) + ", "
            + std::to_string (newdirfd) + ", " + std::string (linkpath) + ")");
    }

    // perform original POSIX symlinkat operation
    int result
        = ((libc_symlinkat_t)dlsym (this->m_lib_handle, "symlinkat")) (target, newdirfd, linkpath);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::symlinkat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::symlinkat),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_readlink call. (...)
ssize_t PosixPassthrough::passthrough_readlink (const char* path, char* buf, size_t bufsize)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-readlink (" + std::string (path) + ")");
    }

    // perform original POSIX readlink operation
    ssize_t result = ((libc_readlink_t)dlsym (this->m_lib_handle, "readlink")) (path, buf, bufsize);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::readlink),
                1,
                result);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::readlink),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_readlinkat call. (...)
ssize_t
PosixPassthrough::passthrough_readlinkat (int dirfd, const char* path, char* buf, size_t bufsize)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-readlinkat (" + std::to_string (dirfd) + ", " + std::string (path) + ")");
    }

    // perform original POSIX readlinkat operation
    ssize_t result
        = ((libc_readlinkat_t)dlsym (this->m_lib_handle, "readlinkat")) (dirfd, path, buf, bufsize);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::readlinkat),
                1,
                result);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::readlinkat),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_fopen call. (...)
FILE* PosixPassthrough::passthrough_fopen (const char* pathname, const char* mode)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-fopen (" + std::string (pathname) + ")");
    }

    // perform original POSIX fopen operation
    FILE* result = ((libc_fopen_t)dlsym (this->m_lib_handle, "fopen")) (pathname, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result != nullptr) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fopen),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fopen),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_fdopen call. (...)
FILE* PosixPassthrough::passthrough_fdopen (int fd, const char* mode)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-fdopen (" + std::to_string (fd) + ")");
    }

    // perform original POSIX fdopen operation
    FILE* result = ((libc_fdopen_t)dlsym (this->m_lib_handle, "fdopen")) (fd, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result != nullptr) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fdopen),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fdopen),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_freopen call. (...)
FILE* PosixPassthrough::passthrough_freopen (const char* pathname, const char* mode, FILE* stream)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-freopen (" + std::string (pathname) + ")");
    }

    // perform original POSIX freopen operation
    FILE* result = ((libc_freopen_t)dlsym (this->m_lib_handle, "freopen")) (pathname, mode, stream);

    // update statistic entry
    if (this->m_collect) {
        if (result != nullptr) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::freopen),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::freopen),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_fclose call. (...)
int PosixPassthrough::passthrough_fclose (FILE* stream)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-fclose");
    }

    // perform original POSIX fclose operation
    int result = ((libc_fclose_t)dlsym (this->m_lib_handle, "fclose")) (stream);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fclose),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fclose),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_fflush call. (...)
int PosixPassthrough::passthrough_fflush (FILE* stream)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-fflush");
    }

    if (this->m_lib_handle == nullptr) {
        std::cout << "will open m_lib_handle from libc.so.6 \n";
        this->m_lib_handle = ::dlopen (this->m_lib_name.data (), RTLD_LAZY);
        printf ("--> %ld\n", this->m_lib_handle);
        //        m_libc_write = (libc_write_t)dlsym (this->m_lib_handle, "write");
        std::cout << "Passei no lib_c_fflush\n";
    }

    // perform original POSIX fflush operation
    int result = ((libc_fflush_t)dlsym (this->m_lib_handle, "fflush")) (stream);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fflush),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fflush),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_access call. (...)
int PosixPassthrough::passthrough_access (const char* path, int mode)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-access");
    }

    // perform original POSIX access operation
    int result = ((libc_access_t)dlsym (this->m_lib_handle, "access")) (path, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::access),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::access),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_faccessat call. (...)
int PosixPassthrough::passthrough_faccessat (int dirfd, const char* path, int mode, int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-faccessat");
    }

    // perform original POSIX faccessat operation
    int result
        = ((libc_faccessat_t)dlsym (this->m_lib_handle, "faccessat")) (dirfd, path, mode, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::faccessat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::faccessat),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_mkdir call. (...)
int PosixPassthrough::passthrough_mkdir (const char* path, mode_t mode)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-mkdir (" + std::string (path) + ")");
    }

    // perform original POSIX mkdir operation
    int result = ((libc_mkdir_t)dlsym (this->m_lib_handle, "mkdir")) (path, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::mkdir), 1, 0);
        } else {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::mkdir), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_mkdirat call. (...)
int PosixPassthrough::passthrough_mkdirat (int dirfd, const char* path, mode_t mode)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-mkdirat (" + std::to_string (dirfd) + ", " + std::string (path) + ")");
    }

    // perform original POSIX mkdirat operation
    int result = ((libc_mkdirat_t)dlsym (this->m_lib_handle, "mkdirat")) (dirfd, path, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::mkdirat), 1, 0);
        } else {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::mkdirat),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_readdir call. (...)
struct dirent* PosixPassthrough::passthrough_readdir (DIR* dirp)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-readdir");
    }

    struct dirent* entry;

    // perform original POSIX readdir operation
    entry = ((libc_readdir_t)dlsym (this->m_lib_handle, "readdir")) (dirp);

    // update statistic entry
    if (this->m_collect) {
        if (entry != nullptr) {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::readdir), 1, 0);
        } else {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::readdir),
                1,
                0,
                1);
        }
    }

    return entry;
}

// passthrough_opendir call. (...)
DIR* PosixPassthrough::passthrough_opendir (const char* path)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-opendir (" + std::string (path) + ")");
    }

    DIR* folder;

    // perform original POSIX opendir operation
    folder = ((libc_opendir_t)dlsym (this->m_lib_handle, "opendir")) (path);

    // update statistic entry
    if (this->m_collect) {
        if (folder != nullptr) {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::opendir), 1, 0);
        } else {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::opendir),
                1,
                0,
                1);
        }
    }

    return folder;
}

// passthrough_fdopendir call. (...)
DIR* PosixPassthrough::passthrough_fdopendir (int fd)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-fdopendir (" + std::to_string (fd) + ")");
    }

    DIR* folder;

    // perform original POSIX fopendir operation
    folder = ((libc_fdopendir_t)dlsym (this->m_lib_handle, "fdopendir")) (fd);

    // update statistic entry
    if (this->m_collect) {
        if (folder != nullptr) {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::fdopendir),
                1,
                0);
        } else {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::fdopendir),
                1,
                0,
                1);
        }
    }

    return folder;
}

// passthrough_closedir call. (...)
int PosixPassthrough::passthrough_closedir (DIR* dirp)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-closedir");
    }

    // perform original POSIX closedir operation
    int result = ((libc_closedir_t)dlsym (this->m_lib_handle, "closedir")) (dirp);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::closedir), 1, 0);
        } else {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::closedir),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_rmdir call. (...)
int PosixPassthrough::passthrough_rmdir (const char* path)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-rmdir");
    }

    // perform original POSIX rmdir operation
    int result = ((libc_rmdir_t)dlsym (this->m_lib_handle, "rmdir")) (path);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::rmdir), 1, 0);
        } else {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::rmdir), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_dirfd call. (...)
int PosixPassthrough::passthrough_dirfd (DIR* dirp)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-dirfd");
    }

    // perform original POSIX dirfd operation
    int result = ((libc_dirfd_t)dlsym (this->m_lib_handle, "dirfd")) (dirp);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::dirfd), 1, 0);
        } else {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::dirfd), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_getxattr call. (...)
ssize_t PosixPassthrough::passthrough_getxattr (const char* path,
    const char* name,
    void* value,
    size_t size)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-getxattr (" + std::string (path) + ", " + std::string (name) + ")");
    }

    // perform original POSIX getxattr operation
    ssize_t result
        = ((libc_getxattr_t)dlsym (this->m_lib_handle, "getxattr")) (path, name, value, size);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::getxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats
                .update_statistic_entry (static_cast<int> (ExtendedAttributes::getxattr), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_lgetxattr call. (...)
ssize_t PosixPassthrough::passthrough_lgetxattr (const char* path,
    const char* name,
    void* value,
    size_t size)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-lgetxattr (" + std::string (path) + ", " + std::string (name) + ")");
    }

    // perform original POSIX lgetxattr operation
    ssize_t result
        = ((libc_lgetxattr_t)dlsym (this->m_lib_handle, "lgetxattr")) (path, name, value, size);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::lgetxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats
                .update_statistic_entry (static_cast<int> (ExtendedAttributes::lgetxattr), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_fgetxattr call. (...)
ssize_t PosixPassthrough::passthrough_fgetxattr (int fd, const char* name, void* value, size_t size)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-fgetxattr (" + std::to_string (fd) + ", " + std::string (name) + ")");
    }

    // perform original POSIX getxattr operation
    ssize_t result
        = ((libc_fgetxattr_t)dlsym (this->m_lib_handle, "fgetxattr")) (fd, name, value, size);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::fgetxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats
                .update_statistic_entry (static_cast<int> (ExtendedAttributes::fgetxattr), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_setxattr call. (...)
int PosixPassthrough::passthrough_setxattr (const char* path,
    const char* name,
    const void* value,
    size_t size,
    int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-setxattr (" + std::string (path) + ", " + std::string (name) + ")");
    }

    // perform original POSIX getxattr operation
    int result = ((
        libc_setxattr_t)dlsym (this->m_lib_handle, "setxattr")) (path, name, value, size, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::setxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats
                .update_statistic_entry (static_cast<int> (ExtendedAttributes::setxattr), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_lsetxattr call. (...)
int PosixPassthrough::passthrough_lsetxattr (const char* path,
    const char* name,
    const void* value,
    size_t size,
    int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-lsetxattr (" + std::string (path) + ", " + std::string (name) + ")");
    }

    // perform original POSIX lgetxattr operation
    int result = ((
        libc_lsetxattr_t)dlsym (this->m_lib_handle, "lsetxattr")) (path, name, value, size, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::lsetxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats
                .update_statistic_entry (static_cast<int> (ExtendedAttributes::lsetxattr), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_fsetxattr call. (...)
int PosixPassthrough::passthrough_fsetxattr (int fd,
    const char* name,
    const void* value,
    size_t size,
    int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-fsetxattr (" + std::to_string (fd) + ", " + std::string (name) + ")");
    }

    // perform original POSIX getxattr operation
    int result = ((
        libc_fsetxattr_t)dlsym (this->m_lib_handle, "fsetxattr")) (fd, name, value, size, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::fsetxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats
                .update_statistic_entry (static_cast<int> (ExtendedAttributes::fsetxattr), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_listxattr call. (...)
ssize_t PosixPassthrough::passthrough_listxattr (const char* path, char* list, size_t size)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-listxattr (" + std::string (path) + ")");
    }

    // perform original POSIX listxattr operation
    ssize_t result = ((libc_listxattr_t)dlsym (this->m_lib_handle, "listxattr")) (path, list, size);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::listxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats
                .update_statistic_entry (static_cast<int> (ExtendedAttributes::listxattr), 1, 0, 1);
        }
    }

    return result;
}

// passthrough_llistxattr call. (...)
ssize_t PosixPassthrough::passthrough_llistxattr (const char* path, char* list, size_t size)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-llistxattr (" + std::string (path) + ")");
    }

    // perform original POSIX llistxattr operation
    ssize_t result
        = ((libc_llistxattr_t)dlsym (this->m_lib_handle, "llistxattr")) (path, list, size);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::llistxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::llistxattr),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_flistxattr call. (...)
ssize_t PosixPassthrough::passthrough_flistxattr (int fd, char* list, size_t size)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-flistxattr (" + std::to_string (fd) + ")");
    }

    // perform original POSIX flistxattr operation
    ssize_t result = ((libc_flistxattr_t)dlsym (this->m_lib_handle, "flistxattr")) (fd, list, size);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::flistxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::flistxattr),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_removexattr call. (...)
int PosixPassthrough::passthrough_removexattr (const char* path, const char* name)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-removexattr (" + std::string (path) + ", " + std::string (name) + ")");
    }

    // perform original POSIX removexattr operation
    int result = ((libc_removexattr_t)dlsym (this->m_lib_handle, "removexattr")) (path, name);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::removexattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::removexattr),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_lremovexattr call. (...)
int PosixPassthrough::passthrough_lremovexattr (const char* path, const char* name)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-lremovexattr (" + std::string (path) + ", " + std::string (name) + ")");
    }

    // perform original POSIX lremovexattr operation
    int result = ((libc_lremovexattr_t)dlsym (this->m_lib_handle, "lremovexattr")) (path, name);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::lremovexattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::lremovexattr),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_fremovexattr call. (...)
int PosixPassthrough::passthrough_fremovexattr (int fd, const char* name)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-fremovexattr (" + std::to_string (fd) + ", " + std::string (name) + ")");
    }

    // perform original POSIX fremovexattr operation
    int result = ((libc_fremovexattr_t)dlsym (this->m_lib_handle, "fremovexattr")) (fd, name);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::fremovexattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::fremovexattr),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_chmod call. (...)
int PosixPassthrough::passthrough_chmod (const char* path, mode_t mode)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-chmod (" + std::string (path) + ")");
    }

    // perform original POSIX chmod operation
    int result = ((libc_chmod_t)dlsym (this->m_lib_handle, "chmod")) (path, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::chmod),
                1,
                0);
        } else {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::chmod),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_fchmod call. (...)
int PosixPassthrough::passthrough_fchmod (int fd, mode_t mode)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-fchmod (" + std::to_string (fd) + ")");
    }

    // perform original POSIX fchmod operation
    int result = ((libc_fchmod_t)dlsym (this->m_lib_handle, "fchmod")) (fd, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::fchmod),
                1,
                0);
        } else {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::fchmod),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_fchmodat call. (...)
int PosixPassthrough::passthrough_fchmodat (int dirfd, const char* path, mode_t mode, int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-fchmodat (" + std::to_string (dirfd) + ", " + std::string (path) + ")");
    }

    // perform original POSIX fchmodat operation
    int result
        = ((libc_fchmodat_t)dlsym (this->m_lib_handle, "fchmodat")) (dirfd, path, mode, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::fchmodat),
                1,
                0);
        } else {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::fchmodat),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_chown call. (...)
int PosixPassthrough::passthrough_chown (const char* pathname, uid_t owner, gid_t group)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-chown (" + std::string (pathname) + ")");
    }

    // perform original POSIX chown operation
    int result = ((libc_chown_t)dlsym (this->m_lib_handle, "chown")) (pathname, owner, group);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::chown),
                1,
                0);
        } else {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::chown),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_lchown call. (...)
int PosixPassthrough::passthrough_lchown (const char* pathname, uid_t owner, gid_t group)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-lchown (" + std::string (pathname) + ")");
    }

    // perform original POSIX lchown operation
    int result = ((libc_lchown_t)dlsym (this->m_lib_handle, "lchown")) (pathname, owner, group);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::lchown),
                1,
                0);
        } else {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::lchown),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_fchown call. (...)
int PosixPassthrough::passthrough_fchown (int fd, uid_t owner, gid_t group)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-fchown (" + std::to_string (fd) + ")");
    }

    // perform original POSIX lchown operation
    int result = ((libc_fchown_t)dlsym (this->m_lib_handle, "fchown")) (fd, owner, group);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::fchown),
                1,
                0);
        } else {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::fchown),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_fchownat call. (...)
int PosixPassthrough::passthrough_fchownat (int dirfd,
    const char* pathname,
    uid_t owner,
    gid_t group,
    int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-fchownat (" + std::to_string (dirfd) + ", "
            + std::string (pathname) + ")");
    }

    // perform original POSIX fchownat operation
    int result = ((libc_fchownat_t)dlsym (this->m_lib_handle,
        "fchownat")) (dirfd, pathname, owner, group, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::fchownat),
                1,
                0);
        } else {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::fchownat),
                1,
                0,
                1);
        }
    }

    return result;
}

} // namespace padll
