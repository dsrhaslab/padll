/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <ldpaio/interface/posix_passthrough.hpp>

namespace ldpaio {

// PosixPassthrough default constructor.
PosixPassthrough::PosixPassthrough ()
{
    Logging::log_info ("PosixPassthrough default constructor.");

    // Dynamic loading of the libc library (referred to as 'libc.so.6').
    // loads the dynamic shared object (shared library) file named by the null-terminated string
    // filename and returns an opaque "handle" for the loaded object.
    this->m_lib_handle = ::dlopen ("libc.so.6", RTLD_LAZY);

    // validate library pointer
    if (this->m_lib_handle == nullptr) {
        Logging::log_error ("Error while dlopen'ing libc.so.6.");
        return;
    }
}

// PosixPassthrough parameterized constructor.
PosixPassthrough::PosixPassthrough (const std::string& lib, bool stat_collection) :
    m_collect { stat_collection }
{
    Logging::log_info ("PosixPassthrough parameterized constructor.");

    // validate if 'lib' is valid
    if (lib.empty ()) {
        Logging::log_error ("Library not valid.");
        return;
    }

    // Dynamic loading of the library referred by 'lib'.
    // loads the dynamic shared object (shared library) file named by the null-terminated string
    // filename and returns an opaque "handle" for the loaded object.
    this->m_lib_handle = ::dlopen (lib.data (), RTLD_LAZY);

    // validate library pointer
    if (this->m_lib_handle == nullptr) {
        Logging::log_error ("Error while dlopen'ing " + lib + ".");
        return;
    }
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

    // perform original POSIX read operation
    ssize_t result = ((libc_read_t)dlsym (this->m_lib_handle, "read")) (fd, buf, counter);

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

    // perform original POSIX write operation
    ssize_t result = ((libc_write_t)dlsym (this->m_lib_handle, "write")) (fd, buf, counter);

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

    // perform original POSIX pread operation
    ssize_t result = ((libc_pread_t)dlsym (this->m_lib_handle, "pread")) (fd, buf, counter, offset);

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

    // perform original POSIX pwrite operation
    ssize_t result
        = ((libc_pwrite_t)dlsym (this->m_lib_handle, "pwrite")) (fd, buf, counter, offset);

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

    // perform original POSIX fread operation
    ssize_t result = ((libc_fread_t)dlsym (this->m_lib_handle, "fread")) (ptr, size, nmemb, stream);

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

    // perform original POSIX fwrite operation
    ssize_t result
        = ((libc_fwrite_t)dlsym (this->m_lib_handle, "fwrite")) (ptr, size, nmemb, stream);

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

    // perform original POSIX open operation
    int result = ((libc_open_variadic_t)dlsym (this->m_lib_handle, "open")) (path, flags, mode);

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

    // perform original POSIX open operation
    int result = ((libc_open_t)dlsym (this->m_lib_handle, "open")) (path, flags);

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

    // perform original POSIX open operation
    int result = ((libc_creat_t)dlsym (this->m_lib_handle, "creat")) (path, mode);

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

    // perform original POSIX openat operation
    int result
        = ((libc_openat_variadic_t)dlsym (this->m_lib_handle, "openat")) (dirfd, path, flags, mode);

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

    // perform original POSIX openat operation
    int result = ((libc_openat_t)dlsym (this->m_lib_handle, "openat")) (dirfd, path, flags);

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

    // perform original POSIX open64 operation
    int result = ((libc_open64_variadic_t)dlsym (this->m_lib_handle, "open64")) (path, flags, mode);

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

    // perform original POSIX open64 operation
    int result = ((libc_open64_t)dlsym (this->m_lib_handle, "open64")) (path, flags);

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

    // perform original POSIX close operation
    int result = ((libc_close_t)dlsym (this->m_lib_handle, "close")) (fd);

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

    // perform original POSIX fsync operation
    int result = ((libc_fsync_t)dlsym (this->m_lib_handle, "fsync")) (fd);

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

    // perform original POSIX fdatasync operation
    int result = ((libc_fdatasync_t)dlsym (this->m_lib_handle, "fdatasync")) (fd);

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

    // perform original POSIX sync operation
    ((libc_sync_t)dlsym (this->m_lib_handle, "sync")) ();

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

    // perform original POSIX syncfs operation
    int result = ((libc_syncfs_t)dlsym (this->m_lib_handle, "syncfs")) (fd);

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

    // perform original POSIX truncate operation
    int result = ((libc_truncate_t)dlsym (this->m_lib_handle, "truncate")) (path, length);

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

    // perform original POSIX ftruncate operation
    int result = ((libc_ftruncate_t)dlsym (this->m_lib_handle, "ftruncate")) (fd, length);

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

    // perform original POSIX __xstat (stat) operation
    int result = ((libc_xstat_t)dlsym (this->m_lib_handle, "__xstat")) (version, path, statbuf);

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

    // perform original POSIX __lxstat (lstat) operation
    int result = ((libc_lxstat_t)dlsym (this->m_lib_handle, "__lxstat")) (version, path, statbuf);

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

    // perform original POSIX __fxstat (fstat) operation
    int result = ((libc_fxstat_t)dlsym (this->m_lib_handle, "__fxstat")) (version, fd, statbuf);

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

    // perform original POSIX __fxstatat (fstatat) operation
    int result = ((libc_fxstatat_t)dlsym (this->m_lib_handle,
        "__fxstatat")) (version, dirfd, path, statbuf, flags);

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

    // perform original POSIX statfs operation
    int result = ((libc_statfs_t)dlsym (this->m_lib_handle, "statfs")) (path, buf);

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

    // perform original POSIX fstatfs operation
    int result = ((libc_fstatfs_t)dlsym (this->m_lib_handle, "fstatfs")) (fd, buf);

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

} // namespace ldpaio