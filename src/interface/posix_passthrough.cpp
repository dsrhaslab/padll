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
            ? m_metadata_operations.m_fdatasync
            = (libc_fdatasync_t)dlsym (this->m_lib_handle, "fdatasync")
            : m_metadata_operations.m_fdatasync = (libc_fdatasync_t)dlsym (RTLD_NEXT, "fdatasync");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_fdatasync) {
        m_metadata_operations.m_fdatasync
            = (libc_fdatasync_t)dlsym (this->m_lib_handle, "fdatasync");
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
            ? m_metadata_operations.m_truncate
            = (libc_truncate_t)dlsym (this->m_lib_handle, "truncate")
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

// passthrough_ftruncate call. (...)
int PosixPassthrough::passthrough_ftruncate (int fd, off_t length)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-ftruncate (" + std::to_string (fd) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_ftruncate && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_ftruncate
            = (libc_ftruncate_t)dlsym (this->m_lib_handle, "ftruncate")
            : m_metadata_operations.m_ftruncate = (libc_ftruncate_t)dlsym (RTLD_NEXT, "ftruncate");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_ftruncate) {
        m_metadata_operations.m_ftruncate
            = (libc_ftruncate_t)dlsym (this->m_lib_handle, "ftruncate");
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

// passthrough_truncate64 call. (...)
int PosixPassthrough::passthrough_truncate64 (const char* path, off_t length)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-truncate64 (" + std::string (path) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_truncate64 && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ()) ? m_metadata_operations.m_truncate64
            = (libc_truncate64_t)dlsym (this->m_lib_handle, "truncate64")
                                         : m_metadata_operations.m_truncate64
            = (libc_truncate64_t)dlsym (RTLD_NEXT, "truncate64");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_truncate64) {
        m_metadata_operations.m_truncate64
            = (libc_truncate64_t)dlsym (this->m_lib_handle, "truncate64");
    }

    // perform original POSIX truncate64 operation
    int result = m_metadata_operations.m_truncate64 (path, length);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::truncate64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::truncate64),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_ftruncate64 call. (...)
int PosixPassthrough::passthrough_ftruncate64 (int fd, off_t length)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-ftruncate64 (" + std::to_string (fd) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_ftruncate64 && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ()) ? m_metadata_operations.m_ftruncate64
            = (libc_ftruncate64_t)dlsym (this->m_lib_handle, "ftruncate64")
                                         : m_metadata_operations.m_ftruncate64
            = (libc_ftruncate64_t)dlsym (RTLD_NEXT, "ftruncate64");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_ftruncate64) {
        m_metadata_operations.m_ftruncate64
            = (libc_ftruncate64_t)dlsym (this->m_lib_handle, "ftruncate64");
    }

    // perform original POSIX ftruncate64 operation
    int result = m_metadata_operations.m_ftruncate64 (fd, length);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::ftruncate64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::ftruncate64),
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
    // int result = ((libc_lxstat_t)dlsym (this->m_lib_handle, "__lxstat")) (version, path,
    // statbuf);
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
            ? m_metadata_operations.m_fxstatat
            = (libc_fxstatat_t)dlsym (this->m_lib_handle, "__fxstatat")
            : m_metadata_operations.m_fxstatat = (libc_fxstatat_t)dlsym (RTLD_NEXT, "__fxstatat");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_fxstatat) {
        m_metadata_operations.m_fxstatat
            = (libc_fxstatat_t)dlsym (this->m_lib_handle, "__fxstatat");
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

// passthrough_xstat64 call. (...)
int PosixPassthrough::passthrough_xstat64 (int version, const char* path, struct stat64* statbuf)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-xstat64 (" + std::string (path) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_xstat64 && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_xstat64
            = (libc_xstat64_t)dlsym (this->m_lib_handle, "__xstat64")
            : m_metadata_operations.m_xstat64 = (libc_xstat64_t)dlsym (RTLD_NEXT, "__xstat64");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_xstat64) {
        m_metadata_operations.m_xstat64 = (libc_xstat64_t)dlsym (this->m_lib_handle, "__xstat64");
    }

    // perform original POSIX __xstat64 (stat) operation
    // int result = ((libc_xstat64_t)dlsym (this->m_lib_handle, "__xstat64")) (version, path,
    // statbuf);
    int result = m_metadata_operations.m_xstat64 (version, path, statbuf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::stat64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::stat64),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_lxstat64 call. (...)
int PosixPassthrough::passthrough_lxstat64 (int version, const char* path, struct stat64* statbuf)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-lxstat64 (" + std::string (path) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_lxstat64 && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_lxstat64
            = (libc_lxstat64_t)dlsym (this->m_lib_handle, "__lxstat64")
            : m_metadata_operations.m_lxstat64 = (libc_lxstat64_t)dlsym (RTLD_NEXT, "__lxstat64");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_lxstat64) {
        m_metadata_operations.m_lxstat64
            = (libc_lxstat64_t)dlsym (this->m_lib_handle, "__lxstat64");
    }

    // perform original POSIX __lxstat64 (lstat64) operation
    // int result = ((libc_lxstat64_t)dlsym (this->m_lib_handle, "__lxstat64")) (version, path,
    // statbuf);
    int result = m_metadata_operations.m_lxstat64 (version, path, statbuf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::lstat64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::lstat64),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_fxstat64 call. (...)
int PosixPassthrough::passthrough_fxstat64 (int version, int fd, struct stat64* statbuf)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-fxstat64 (" + std::to_string (fd) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_fxstat64 && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_fxstat64
            = (libc_fxstat64_t)dlsym (this->m_lib_handle, "__fxstat64")
            : m_metadata_operations.m_fxstat64 = (libc_fxstat64_t)dlsym (RTLD_NEXT, "__fxstat64");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_fxstat64) {
        m_metadata_operations.m_fxstat64
            = (libc_fxstat64_t)dlsym (this->m_lib_handle, "__fxstat64");
    }

    // perform original POSIX __fxstat64 (fxstat64) operation
    // int result = ((libc_fxstat64_t)dlsym (this->m_lib_handle, "__fxstat64")) (version, fd,
    // statbuf);
    int result = m_metadata_operations.m_fxstat64 (version, fd, statbuf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstat64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstat64),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_fxstatat64 call. (...)
int PosixPassthrough::passthrough_fxstatat64 (int version,
    int dirfd,
    const char* path,
    struct stat64* statbuf,
    int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug (
            "passthrough-fxstatat64 (" + std::to_string (dirfd) + ", " + std::string (path) + ")");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_fxstatat64 && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ()) ? m_metadata_operations.m_fxstatat64
            = (libc_fxstatat64_t)dlsym (this->m_lib_handle, "__fxstatat64")
                                         : m_metadata_operations.m_fxstatat64
            = (libc_fxstatat64_t)dlsym (RTLD_NEXT, "__fxstatat64");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_fxstatat64) {
        m_metadata_operations.m_fxstatat64
            = (libc_fxstatat64_t)dlsym (this->m_lib_handle, "__fxstatat64");
    }

    // perform original POSIX __fxstatat64 (fxstatat64) operation
    // int result = ((libc_fxstatat_t)dlsym (this->m_lib_handle,
    //     "__fxstatat64")) (version, dirfd, path, statbuf, flags);
    int result = m_metadata_operations.m_fxstatat64 (version, dirfd, path, statbuf, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstatat64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstatat64),
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
            ? m_metadata_operations.m_fstatfs
            = (libc_fstatfs_t)dlsym (this->m_lib_handle, "fstatfs")
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

    // validate function and library handle pointers
    if (!m_metadata_operations.m_link && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_link = (libc_link_t)dlsym (this->m_lib_handle, "link")
            : m_metadata_operations.m_link = (libc_link_t)dlsym (RTLD_NEXT, "link");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_link) {
        m_metadata_operations.m_link = (libc_link_t)dlsym (this->m_lib_handle, "link");
    }

    // perform original POSIX link operation
    int result = m_metadata_operations.m_link (old_path, new_path);

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

    // validate function and library handle pointers
    if (!m_metadata_operations.m_unlink && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_unlink = (libc_unlink_t)dlsym (this->m_lib_handle, "unlink")
            : m_metadata_operations.m_unlink = (libc_unlink_t)dlsym (RTLD_NEXT, "unlink");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_unlink) {
        m_metadata_operations.m_unlink = (libc_unlink_t)dlsym (this->m_lib_handle, "unlink");
    }

    // perform original POSIX unlink operation
    int result = m_metadata_operations.m_unlink (path);

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

    // validate function and library handle pointers
    if (!m_metadata_operations.m_linkat && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_linkat = (libc_linkat_t)dlsym (this->m_lib_handle, "linkat")
            : m_metadata_operations.m_linkat = (libc_linkat_t)dlsym (RTLD_NEXT, "linkat");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_linkat) {
        m_metadata_operations.m_linkat = (libc_linkat_t)dlsym (this->m_lib_handle, "linkat");
    }

    // perform original POSIX linkat operation
    int result = m_metadata_operations.m_linkat (olddirfd, old_path, newdirfd, new_path, flags);

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

    // validate function and library handle pointers
    if (!m_metadata_operations.m_unlinkat && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_unlinkat
            = (libc_unlinkat_t)dlsym (this->m_lib_handle, "unlinkat")
            : m_metadata_operations.m_unlinkat = (libc_unlinkat_t)dlsym (RTLD_NEXT, "unlinkat");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_unlinkat) {
        m_metadata_operations.m_unlinkat = (libc_unlinkat_t)dlsym (this->m_lib_handle, "unlinkat");
    }

    // perform original POSIX unlinkat operation
    int result = m_metadata_operations.m_unlinkat (dirfd, pathname, flags);

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

    // validate function and library handle pointers
    if (!m_metadata_operations.m_rename && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_rename = (libc_rename_t)dlsym (this->m_lib_handle, "rename")
            : m_metadata_operations.m_rename = (libc_rename_t)dlsym (RTLD_NEXT, "rename");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_rename) {
        m_metadata_operations.m_rename = (libc_rename_t)dlsym (this->m_lib_handle, "rename");
    }

    // perform original POSIX rename operation
    int result = m_metadata_operations.m_rename (old_path, new_path);

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

    // validate function and library handle pointers
    if (!m_metadata_operations.m_renameat && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_renameat
            = (libc_renameat_t)dlsym (this->m_lib_handle, "renameat")
            : m_metadata_operations.m_renameat = (libc_renameat_t)dlsym (RTLD_NEXT, "renameat");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_renameat) {
        m_metadata_operations.m_renameat = (libc_renameat_t)dlsym (this->m_lib_handle, "renameat");
    }

    // perform original POSIX renameat operation
    int result = m_metadata_operations.m_renameat (olddirfd, old_path, newdirfd, new_path);

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

    // validate function and library handle pointers
    if (!m_metadata_operations.m_symlink && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_symlink
            = (libc_symlink_t)dlsym (this->m_lib_handle, "symlink")
            : m_metadata_operations.m_symlink = (libc_symlink_t)dlsym (RTLD_NEXT, "symlink");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_symlink) {
        m_metadata_operations.m_symlink = (libc_symlink_t)dlsym (this->m_lib_handle, "symlink");
    }

    // perform original POSIX symlink operation
    int result = m_metadata_operations.m_symlink (target, linkpath);

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

    // validate function and library handle pointers
    if (!m_metadata_operations.m_symlinkat && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_symlinkat
            = (libc_symlinkat_t)dlsym (this->m_lib_handle, "symlinkat")
            : m_metadata_operations.m_symlinkat = (libc_symlinkat_t)dlsym (RTLD_NEXT, "symlinkat");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_symlinkat) {
        m_metadata_operations.m_symlinkat
            = (libc_symlinkat_t)dlsym (this->m_lib_handle, "symlinkat");
    }

    // perform original POSIX symlinkat operation
    int result = m_metadata_operations.m_symlinkat (target, newdirfd, linkpath);

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

    // validate function and library handle pointers
    if (!m_metadata_operations.m_readlink && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_readlink
            = (libc_readlink_t)dlsym (this->m_lib_handle, "readlink")
            : m_metadata_operations.m_readlink = (libc_readlink_t)dlsym (RTLD_NEXT, "readlink");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_readlink) {
        m_metadata_operations.m_readlink = (libc_readlink_t)dlsym (this->m_lib_handle, "readlink");
    }

    // perform original POSIX readlink operation
    ssize_t result = m_metadata_operations.m_readlink (path, buf, bufsize);

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

    // validate function and library handle pointers
    if (!m_metadata_operations.m_readlinkat && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ()) ? m_metadata_operations.m_readlinkat
            = (libc_readlinkat_t)dlsym (this->m_lib_handle, "readlinkat")
                                         : m_metadata_operations.m_readlinkat
            = (libc_readlinkat_t)dlsym (RTLD_NEXT, "readlinkat");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_readlinkat) {
        m_metadata_operations.m_readlinkat
            = (libc_readlinkat_t)dlsym (this->m_lib_handle, "readlinkat");
    }

    // perform original POSIX readlinkat operation
    ssize_t result = m_metadata_operations.m_readlinkat (dirfd, path, buf, bufsize);

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

    // validate function and library handle pointers
    if (!m_metadata_operations.m_fopen && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_fopen = (libc_fopen_t)dlsym (this->m_lib_handle, "fopen")
            : m_metadata_operations.m_fopen = (libc_fopen_t)dlsym (RTLD_NEXT, "fopen");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_fopen) {
        m_metadata_operations.m_fopen = (libc_fopen_t)dlsym (this->m_lib_handle, "fopen");
    }

    // perform original POSIX fopen operation
    FILE* result = m_metadata_operations.m_fopen (pathname, mode);

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

    // validate function and library handle pointers
    if (!m_metadata_operations.m_fdopen && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_fdopen = (libc_fdopen_t)dlsym (this->m_lib_handle, "fdopen")
            : m_metadata_operations.m_fdopen = (libc_fdopen_t)dlsym (RTLD_NEXT, "fdopen");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_fdopen) {
        m_metadata_operations.m_fdopen = (libc_fdopen_t)dlsym (this->m_lib_handle, "fdopen");
    }

    // perform original POSIX fdopen operation
    FILE* result = m_metadata_operations.m_fdopen (fd, mode);

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

    // validate function and library handle pointers
    if (!m_metadata_operations.m_freopen && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_freopen
            = (libc_freopen_t)dlsym (this->m_lib_handle, "freopen")
            : m_metadata_operations.m_freopen = (libc_freopen_t)dlsym (RTLD_NEXT, "freopen");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_freopen) {
        m_metadata_operations.m_freopen = (libc_freopen_t)dlsym (this->m_lib_handle, "freopen");
    }

    // perform original POSIX freopen operation
    FILE* result = m_metadata_operations.m_freopen (pathname, mode, stream);

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

    // validate function and library handle pointers
    if (!m_metadata_operations.m_fclose && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_fclose = (libc_fclose_t)dlsym (this->m_lib_handle, "fclose")
            : m_metadata_operations.m_fclose = (libc_fclose_t)dlsym (RTLD_NEXT, "fclose");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_fclose) {
        m_metadata_operations.m_fclose = (libc_fclose_t)dlsym (this->m_lib_handle, "fclose");
    }

    // perform original POSIX fclose operation
    int result = m_metadata_operations.m_fclose (stream);

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

    // validate function and library handle pointers
    if (!m_metadata_operations.m_fflush && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_fflush = (libc_fflush_t)dlsym (this->m_lib_handle, "fflush")
            : m_metadata_operations.m_fflush = (libc_fflush_t)dlsym (RTLD_NEXT, "fflush");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_fflush) {
        m_metadata_operations.m_fflush = (libc_fflush_t)dlsym (this->m_lib_handle, "fflush");
    }

    // perform original POSIX fflush operation
    int result = m_metadata_operations.m_fflush (stream);

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

    // validate function and library handle pointers
    if (!m_metadata_operations.m_access && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_access = (libc_access_t)dlsym (this->m_lib_handle, "access")
            : m_metadata_operations.m_access = (libc_access_t)dlsym (RTLD_NEXT, "access");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_access) {
        m_metadata_operations.m_access = (libc_access_t)dlsym (this->m_lib_handle, "access");
    }

    // perform original POSIX access operation
    int result = m_metadata_operations.m_access (path, mode);

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

    // validate function and library handle pointers
    if (!m_metadata_operations.m_faccessat && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_faccessat
            = (libc_faccessat_t)dlsym (this->m_lib_handle, "faccessat")
            : m_metadata_operations.m_faccessat = (libc_faccessat_t)dlsym (RTLD_NEXT, "faccessat");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_faccessat) {
        m_metadata_operations.m_faccessat
            = (libc_faccessat_t)dlsym (this->m_lib_handle, "faccessat");
    }

    // perform original POSIX faccessat operation
    int result = m_metadata_operations.m_faccessat (dirfd, path, mode, flags);

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

// passthrough_lseek call. (...)
off_t PosixPassthrough::passthrough_lseek (int fd, off_t offset, int whence)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-lseek");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_lseek && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_lseek = (libc_lseek_t)dlsym (this->m_lib_handle, "lseek")
            : m_metadata_operations.m_lseek = (libc_lseek_t)dlsym (RTLD_NEXT, "lseek");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_lseek) {
        m_metadata_operations.m_lseek = (libc_lseek_t)dlsym (this->m_lib_handle, "lseek");
    }

    // perform original POSIX lseek operation
    off_t result = m_metadata_operations.m_lseek (fd, offset, whence);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::lseek),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::lseek),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_fseek call. (...)
int PosixPassthrough::passthrough_fseek (FILE* stream, long offset, int whence)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-fseek");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_fseek && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_fseek = (libc_fseek_t)dlsym (this->m_lib_handle, "fseek")
            : m_metadata_operations.m_fseek = (libc_fseek_t)dlsym (RTLD_NEXT, "fseek");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_fseek) {
        m_metadata_operations.m_fseek = (libc_fseek_t)dlsym (this->m_lib_handle, "fseek");
    }

    // perform original POSIX fseek operation
    int result = m_metadata_operations.m_fseek (stream, offset, whence);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fseek),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fseek),
                1,
                0,
                1);
        }
    }

    return result;
}

// passthrough_ftell call. (...)
long PosixPassthrough::passthrough_ftell (FILE* stream)
{
    // logging message
    if (option_default_detailed_logging) {
        Logging::log_debug ("passthrough-ftell");
    }

    // validate function and library handle pointers
    if (!m_metadata_operations.m_ftell && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_metadata_operations.m_ftell = (libc_ftell_t)dlsym (this->m_lib_handle, "ftell")
            : m_metadata_operations.m_ftell = (libc_ftell_t)dlsym (RTLD_NEXT, "ftell");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_metadata_operations.m_ftell) {
        m_metadata_operations.m_ftell = (libc_ftell_t)dlsym (this->m_lib_handle, "ftell");
    }

    // perform original POSIX ftell operation
    long result = m_metadata_operations.m_ftell (stream);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::ftell),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::ftell),
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

    // validate function and library handle pointers
    if (!m_directory_operations.m_mkdir && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_directory_operations.m_mkdir = (libc_mkdir_t)dlsym (this->m_lib_handle, "mkdir")
            : m_directory_operations.m_mkdir = (libc_mkdir_t)dlsym (RTLD_NEXT, "mkdir");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_directory_operations.m_mkdir) {
        m_directory_operations.m_mkdir = (libc_mkdir_t)dlsym (this->m_lib_handle, "mkdir");
    }

    // perform original POSIX mkdir operation
    int result = m_directory_operations.m_mkdir (path, mode);

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

    // validate function and library handle pointers
    if (!m_directory_operations.m_mkdirat && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_directory_operations.m_mkdirat
            = (libc_mkdirat_t)dlsym (this->m_lib_handle, "mkdirat")
            : m_directory_operations.m_mkdirat = (libc_mkdirat_t)dlsym (RTLD_NEXT, "mkdirat");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_directory_operations.m_mkdirat) {
        m_directory_operations.m_mkdirat = (libc_mkdirat_t)dlsym (this->m_lib_handle, "mkdirat");
    }

    // perform original POSIX mkdirat operation
    int result = m_directory_operations.m_mkdirat (dirfd, path, mode);

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

    // validate function and library handle pointers
    if (!m_directory_operations.m_readdir && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_directory_operations.m_readdir
            = (libc_readdir_t)dlsym (this->m_lib_handle, "readdir")
            : m_directory_operations.m_readdir = (libc_readdir_t)dlsym (RTLD_NEXT, "readdir");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_directory_operations.m_readdir) {
        m_directory_operations.m_readdir = (libc_readdir_t)dlsym (this->m_lib_handle, "readdir");
    }

    // perform original POSIX readdir operation
    struct dirent* entry = m_directory_operations.m_readdir (dirp);

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

    // validate function and library handle pointers
    if (!m_directory_operations.m_opendir && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_directory_operations.m_opendir
            = (libc_opendir_t)dlsym (this->m_lib_handle, "opendir")
            : m_directory_operations.m_opendir = (libc_opendir_t)dlsym (RTLD_NEXT, "opendir");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_directory_operations.m_opendir) {
        m_directory_operations.m_opendir = (libc_opendir_t)dlsym (this->m_lib_handle, "opendir");
    }

    // perform original POSIX opendir operation
    DIR* folder = m_directory_operations.m_opendir (path);

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

    // validate function and library handle pointers
    if (!m_directory_operations.m_fdopendir && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_directory_operations.m_fdopendir
            = (libc_fdopendir_t)dlsym (this->m_lib_handle, "fdopendir")
            : m_directory_operations.m_fdopendir = (libc_fdopendir_t)dlsym (RTLD_NEXT, "fdopendir");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_directory_operations.m_fdopendir) {
        m_directory_operations.m_fdopendir
            = (libc_fdopendir_t)dlsym (this->m_lib_handle, "fdopendir");
    }

    // perform original POSIX fopendir operation
    DIR* folder = m_directory_operations.m_fdopendir (fd);

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

    // validate function and library handle pointers
    if (!m_directory_operations.m_closedir && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_directory_operations.m_closedir
            = (libc_closedir_t)dlsym (this->m_lib_handle, "closedir")
            : m_directory_operations.m_closedir = (libc_closedir_t)dlsym (RTLD_NEXT, "closedir");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_directory_operations.m_closedir) {
        m_directory_operations.m_closedir = (libc_closedir_t)dlsym (this->m_lib_handle, "closedir");
    }

    // perform original POSIX closedir operation
    int result = m_directory_operations.m_closedir (dirp);

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

    // validate function and library handle pointers
    if (!m_directory_operations.m_rmdir && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_directory_operations.m_rmdir = (libc_rmdir_t)dlsym (this->m_lib_handle, "rmdir")
            : m_directory_operations.m_rmdir = (libc_rmdir_t)dlsym (RTLD_NEXT, "rmdir");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_directory_operations.m_rmdir) {
        m_directory_operations.m_rmdir = (libc_rmdir_t)dlsym (this->m_lib_handle, "rmdir");
    }

    // perform original POSIX rmdir operation
    int result = m_directory_operations.m_rmdir (path);

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

    // validate function and library handle pointers
    if (!m_directory_operations.m_dirfd && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_directory_operations.m_dirfd = (libc_dirfd_t)dlsym (this->m_lib_handle, "dirfd")
            : m_directory_operations.m_dirfd = (libc_dirfd_t)dlsym (RTLD_NEXT, "dirfd");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_directory_operations.m_dirfd) {
        m_directory_operations.m_dirfd = (libc_dirfd_t)dlsym (this->m_lib_handle, "dirfd");
    }

    // perform original POSIX dirfd operation
    int result = m_directory_operations.m_dirfd (dirp);

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

    // validate function and library handle pointers
    if (!m_extattr_operations.m_getxattr && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_extattr_operations.m_getxattr
            = (libc_getxattr_t)dlsym (this->m_lib_handle, "getxattr")
            : m_extattr_operations.m_getxattr = (libc_getxattr_t)dlsym (RTLD_NEXT, "getxattr");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_extattr_operations.m_getxattr) {
        m_extattr_operations.m_getxattr = (libc_getxattr_t)dlsym (this->m_lib_handle, "getxattr");
    }

    // perform original POSIX getxattr operation
    ssize_t result = m_extattr_operations.m_getxattr (path, name, value, size);

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

    // validate function and library handle pointers
    if (!m_extattr_operations.m_lgetxattr && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_extattr_operations.m_lgetxattr
            = (libc_lgetxattr_t)dlsym (this->m_lib_handle, "lgetxattr")
            : m_extattr_operations.m_lgetxattr = (libc_lgetxattr_t)dlsym (RTLD_NEXT, "lgetxattr");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_extattr_operations.m_lgetxattr) {
        m_extattr_operations.m_lgetxattr
            = (libc_lgetxattr_t)dlsym (this->m_lib_handle, "lgetxattr");
    }

    // perform original POSIX lgetxattr operation
    ssize_t result = m_extattr_operations.m_lgetxattr (path, name, value, size);

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

    // validate function and library handle pointers
    if (!m_extattr_operations.m_fgetxattr && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_extattr_operations.m_fgetxattr
            = (libc_fgetxattr_t)dlsym (this->m_lib_handle, "fgetxattr")
            : m_extattr_operations.m_fgetxattr = (libc_fgetxattr_t)dlsym (RTLD_NEXT, "fgetxattr");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_extattr_operations.m_fgetxattr) {
        m_extattr_operations.m_fgetxattr
            = (libc_fgetxattr_t)dlsym (this->m_lib_handle, "fgetxattr");
    }

    // perform original POSIX fgetxattr operation
    ssize_t result = m_extattr_operations.m_fgetxattr (fd, name, value, size);

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

    // validate function and library handle pointers
    if (!m_extattr_operations.m_setxattr && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_extattr_operations.m_setxattr
            = (libc_setxattr_t)dlsym (this->m_lib_handle, "setxattr")
            : m_extattr_operations.m_setxattr = (libc_setxattr_t)dlsym (RTLD_NEXT, "setxattr");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_extattr_operations.m_setxattr) {
        m_extattr_operations.m_setxattr = (libc_setxattr_t)dlsym (this->m_lib_handle, "setxattr");
    }

    // perform original POSIX setxattr operation
    int result = m_extattr_operations.m_setxattr (path, name, value, size, flags);

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

    // validate function and library handle pointers
    if (!m_extattr_operations.m_lsetxattr && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_extattr_operations.m_lsetxattr
            = (libc_lsetxattr_t)dlsym (this->m_lib_handle, "lsetxattr")
            : m_extattr_operations.m_lsetxattr = (libc_lsetxattr_t)dlsym (RTLD_NEXT, "lsetxattr");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_extattr_operations.m_lsetxattr) {
        m_extattr_operations.m_lsetxattr
            = (libc_lsetxattr_t)dlsym (this->m_lib_handle, "lsetxattr");
    }

    // perform original POSIX lsetxattr operation
    int result = m_extattr_operations.m_lsetxattr (path, name, value, size, flags);

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

    // validate function and library handle pointers
    if (!m_extattr_operations.m_fsetxattr && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_extattr_operations.m_fsetxattr
            = (libc_fsetxattr_t)dlsym (this->m_lib_handle, "fsetxattr")
            : m_extattr_operations.m_fsetxattr = (libc_fsetxattr_t)dlsym (RTLD_NEXT, "fsetxattr");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_extattr_operations.m_fsetxattr) {
        m_extattr_operations.m_fsetxattr
            = (libc_fsetxattr_t)dlsym (this->m_lib_handle, "fsetxattr");
    }

    // perform original POSIX fsetxattr operation
    int result = m_extattr_operations.m_fsetxattr (fd, name, value, size, flags);

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

    // validate function and library handle pointers
    if (!m_extattr_operations.m_listxattr && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_extattr_operations.m_listxattr
            = (libc_listxattr_t)dlsym (this->m_lib_handle, "listxattr")
            : m_extattr_operations.m_listxattr = (libc_listxattr_t)dlsym (RTLD_NEXT, "listxattr");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_extattr_operations.m_listxattr) {
        m_extattr_operations.m_listxattr
            = (libc_listxattr_t)dlsym (this->m_lib_handle, "listxattr");
    }

    // perform original POSIX listxattr operation
    ssize_t result = m_extattr_operations.m_listxattr (path, list, size);

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

    // validate function and library handle pointers
    if (!m_extattr_operations.m_llistxattr && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ()) ? m_extattr_operations.m_llistxattr
            = (libc_llistxattr_t)dlsym (this->m_lib_handle, "llistxattr")
                                         : m_extattr_operations.m_llistxattr
            = (libc_llistxattr_t)dlsym (RTLD_NEXT, "llistxattr");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_extattr_operations.m_llistxattr) {
        m_extattr_operations.m_llistxattr
            = (libc_llistxattr_t)dlsym (this->m_lib_handle, "llistxattr");
    }

    // perform original POSIX llistxattr operation
    ssize_t result = m_extattr_operations.m_llistxattr (path, list, size);

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

    // validate function and library handle pointers
    if (!m_extattr_operations.m_flistxattr && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ()) ? m_extattr_operations.m_flistxattr
            = (libc_flistxattr_t)dlsym (this->m_lib_handle, "flistxattr")
                                         : m_extattr_operations.m_flistxattr
            = (libc_flistxattr_t)dlsym (RTLD_NEXT, "flistxattr");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_extattr_operations.m_flistxattr) {
        m_extattr_operations.m_flistxattr
            = (libc_flistxattr_t)dlsym (this->m_lib_handle, "flistxattr");
    }

    // perform original POSIX flistxattr operation
    ssize_t result = m_extattr_operations.m_flistxattr (fd, list, size);

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

    // validate function and library handle pointers
    if (!m_extattr_operations.m_removexattr && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ()) ? m_extattr_operations.m_removexattr
            = (libc_removexattr_t)dlsym (this->m_lib_handle, "removexattr")
                                         : m_extattr_operations.m_removexattr
            = (libc_removexattr_t)dlsym (RTLD_NEXT, "removexattr");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_extattr_operations.m_removexattr) {
        m_extattr_operations.m_removexattr
            = (libc_removexattr_t)dlsym (this->m_lib_handle, "removexattr");
    }

    // perform original POSIX removexattr operation
    int result = m_extattr_operations.m_removexattr (path, name);

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

    // validate function and library handle pointers
    if (!m_extattr_operations.m_lremovexattr && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ()) ? m_extattr_operations.m_lremovexattr
            = (libc_lremovexattr_t)dlsym (this->m_lib_handle, "lremovexattr")
                                         : m_extattr_operations.m_lremovexattr
            = (libc_lremovexattr_t)dlsym (RTLD_NEXT, "lremovexattr");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_extattr_operations.m_lremovexattr) {
        m_extattr_operations.m_lremovexattr
            = (libc_lremovexattr_t)dlsym (this->m_lib_handle, "lremovexattr");
    }

    // perform original POSIX lremovexattr operation
    int result = m_extattr_operations.m_lremovexattr (path, name);

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

    // validate function and library handle pointers
    if (!m_extattr_operations.m_fremovexattr && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ()) ? m_extattr_operations.m_fremovexattr
            = (libc_fremovexattr_t)dlsym (this->m_lib_handle, "fremovexattr")
                                         : m_extattr_operations.m_fremovexattr
            = (libc_fremovexattr_t)dlsym (RTLD_NEXT, "fremovexattr");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_extattr_operations.m_fremovexattr) {
        m_extattr_operations.m_fremovexattr
            = (libc_fremovexattr_t)dlsym (this->m_lib_handle, "fremovexattr");
    }

    // perform original POSIX fremovexattr operation
    int result = m_extattr_operations.m_fremovexattr (fd, name);

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

    // validate function and library handle pointers
    if (!m_filemodes_operations.m_chmod && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_filemodes_operations.m_chmod = (libc_chmod_t)dlsym (this->m_lib_handle, "chmod")
            : m_filemodes_operations.m_chmod = (libc_chmod_t)dlsym (RTLD_NEXT, "chmod");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_filemodes_operations.m_chmod) {
        m_filemodes_operations.m_chmod = (libc_chmod_t)dlsym (this->m_lib_handle, "chmod");
    }

    // perform original POSIX chmod operation
    int result = m_filemodes_operations.m_chmod (path, mode);

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

    // validate function and library handle pointers
    if (!m_filemodes_operations.m_fchmod && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_filemodes_operations.m_fchmod = (libc_fchmod_t)dlsym (this->m_lib_handle, "fchmod")
            : m_filemodes_operations.m_fchmod = (libc_fchmod_t)dlsym (RTLD_NEXT, "fchmod");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_filemodes_operations.m_fchmod) {
        m_filemodes_operations.m_fchmod = (libc_fchmod_t)dlsym (this->m_lib_handle, "fchmod");
    }

    // perform original POSIX fchmod operation
    int result = m_filemodes_operations.m_fchmod (fd, mode);

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

    // validate function and library handle pointers
    if (!m_filemodes_operations.m_fchmodat && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_filemodes_operations.m_fchmodat
            = (libc_fchmodat_t)dlsym (this->m_lib_handle, "fchmodat")
            : m_filemodes_operations.m_fchmodat = (libc_fchmodat_t)dlsym (RTLD_NEXT, "fchmodat");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_filemodes_operations.m_fchmodat) {
        m_filemodes_operations.m_fchmodat = (libc_fchmodat_t)dlsym (this->m_lib_handle, "fchmodat");
    }

    // perform original POSIX fchmodat operation
    int result = m_filemodes_operations.m_fchmodat (dirfd, path, mode, flags);

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

    // validate function and library handle pointers
    if (!m_filemodes_operations.m_chown && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_filemodes_operations.m_chown = (libc_chown_t)dlsym (this->m_lib_handle, "chown")
            : m_filemodes_operations.m_chown = (libc_chown_t)dlsym (RTLD_NEXT, "chown");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_filemodes_operations.m_chown) {
        m_filemodes_operations.m_chown = (libc_chown_t)dlsym (this->m_lib_handle, "chown");
    }

    // perform original POSIX chown operation
    int result = m_filemodes_operations.m_chown (pathname, owner, group);

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

    // validate function and library handle pointers
    if (!m_filemodes_operations.m_lchown && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_filemodes_operations.m_lchown = (libc_chown_t)dlsym (this->m_lib_handle, "lchown")
            : m_filemodes_operations.m_lchown = (libc_chown_t)dlsym (RTLD_NEXT, "lchown");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_filemodes_operations.m_lchown) {
        m_filemodes_operations.m_lchown = (libc_chown_t)dlsym (this->m_lib_handle, "lchown");
    }

    // perform original POSIX lchown operation
    int result = m_filemodes_operations.m_lchown (pathname, owner, group);

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

    // validate function and library handle pointers
    if (!m_filemodes_operations.m_fchown && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_filemodes_operations.m_fchown = (libc_fchown_t)dlsym (this->m_lib_handle, "fchown")
            : m_filemodes_operations.m_fchown = (libc_fchown_t)dlsym (RTLD_NEXT, "fchown");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_filemodes_operations.m_fchown) {
        m_filemodes_operations.m_fchown = (libc_fchown_t)dlsym (this->m_lib_handle, "fchown");
    }

    // perform original POSIX fchown operation
    int result = m_filemodes_operations.m_fchown (fd, owner, group);

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

    // validate function and library handle pointers
    if (!m_filemodes_operations.m_fchownat && !this->m_lib_handle) {
        // open library handle, and assign the operation pointer through m_lib_handle if the open
        // was successful, or through the next operation link.
        (this->dlopen_library_handle ())
            ? m_filemodes_operations.m_fchownat
            = (libc_fchownat_t)dlsym (this->m_lib_handle, "fchownat")
            : m_filemodes_operations.m_fchownat = (libc_fchownat_t)dlsym (RTLD_NEXT, "fchownat");

        // in case the library handle pointer is valid, assign the operation pointer
    } else if (!m_filemodes_operations.m_fchownat) {
        m_filemodes_operations.m_fchownat = (libc_fchownat_t)dlsym (this->m_lib_handle, "fchownat");
    }

    // perform original POSIX fchownat operation
    int result = m_filemodes_operations.m_fchownat (dirfd, pathname, owner, group, flags);

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
