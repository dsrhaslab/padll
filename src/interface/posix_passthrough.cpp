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
}

// PosixPassthrough parameterized constructor.
PosixPassthrough::PosixPassthrough (bool stat_collection) : m_collect { stat_collection }
{
    Logging::log_info ("PosixPassthrough parameterized constructor.");
}

// PosixPassthrough default destructor.
PosixPassthrough::~PosixPassthrough ()
{
    Logging::log_info ("PosixPassthrough default destructor.");
    Logging::log_debug (this->to_string ());
}

// set_statistic_collection call. (...)
void PosixPassthrough::set_statistic_collection (bool value)
{
    this->m_collect.store (value);
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
    stream << "}";

    return stream.str ();
}

// passthrough_read call.
ssize_t PosixPassthrough::passthrough_read (int fd, void* buf, ssize_t counter)
{
    // logging message
    Logging::log_debug ("passthrough-read (" + std::to_string (fd) + ")");

    // perform original POSIX read operation
    int result = ((real_read_t)dlsym (RTLD_NEXT, "read")) (fd, buf, counter);

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
    Logging::log_debug ("passthrough-write (" + std::to_string (fd) + ")");

    // perform original POSIX write operation
    int result = ((real_write_t)dlsym (RTLD_NEXT, "write")) (fd, buf, counter);

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
    Logging::log_debug ("passthrough-pread (" + std::to_string (fd) + ")");

    // perform original POSIX pread operation
    int result = ((real_pread_t)dlsym (RTLD_NEXT, "pread")) (fd, buf, counter, offset);

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
    Logging::log_debug ("passthrough-pwrite (" + std::to_string (fd) + ")");

    // perform original POSIX pwrite operation
    int result = ((real_pwrite_t)dlsym (RTLD_NEXT, "pwrite")) (fd, buf, counter, offset);

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

// passthrough_open call.
int PosixPassthrough::passthrough_open (const char* path, int flags, mode_t mode)
{
    // logging message
    Logging::log_debug ("passthrough-open-variadic (" + std::string (path) + ")");

    // perform original POSIX open operation
    int result = ((real_open_variadic_t)dlsym (RTLD_NEXT, "open")) (path, flags, mode);

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
    Logging::log_debug ("passthrough-open (" + std::string (path) + ")");

    // perform original POSIX open operation
    int result = ((real_open_t)dlsym (RTLD_NEXT, "open")) (path, flags);

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
    Logging::log_debug ("passthrough-creat (" + std::string (path) + ")");

    // perform original POSIX open operation
    int result = ((real_creat_t)dlsym (RTLD_NEXT, "creat")) (path, mode);

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
    Logging::log_debug (
        "passthrough-openat-variadic (" + std::to_string (dirfd) + ", " + std::string (path) + ")");

    // perform original POSIX openat operation
    int result = ((real_openat_variadic_t)dlsym (RTLD_NEXT, "openat")) (dirfd, path, flags, mode);

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
    Logging::log_debug (
        "passthrough-openat (" + std::to_string (dirfd) + ", " + std::string (path) + ")");

    // perform original POSIX openat operation
    int result = ((real_openat_t)dlsym (RTLD_NEXT, "openat")) (dirfd, path, flags);

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
    Logging::log_debug ("passthrough-open64-variadic (" + std::string (path) + ")");

    // perform original POSIX open64 operation
    int result = ((real_open64_variadic_t)dlsym (RTLD_NEXT, "open64")) (path, flags, mode);

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
    Logging::log_debug ("passthrough-open64 (" + std::string (path) + ")");

    // perform original POSIX open64 operation
    int result = ((real_open64_t)dlsym (RTLD_NEXT, "open64")) (path, flags);

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
    Logging::log_debug ("passthrough-close (" + std::to_string (fd) + ")");

    // perform original POSIX close operation
    int result = ((real_close_t)dlsym (RTLD_NEXT, "close")) (fd);

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
    Logging::log_debug ("passthrough-fsync (" + std::to_string (fd) + ")");

    // perform original POSIX fsync operation
    int result = ((real_fsync_t)dlsym (RTLD_NEXT, "fsync")) (fd);

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
    Logging::log_debug ("passthrough-fdatasync (" + std::to_string (fd) + ")");

    // perform original POSIX fsync operation
    int result = ((real_fdatasync_t)dlsym (RTLD_NEXT, "fdatasync")) (fd);

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

// passthrough_truncate call. (...)
int PosixPassthrough::passthrough_truncate (const char* path, off_t length)
{
    // logging message
    Logging::log_debug ("passthrough-truncate (" + std::string (path) + ")");

    // perform original POSIX truncate operation
    int result = ((real_truncate_t)dlsym (RTLD_NEXT, "truncate")) (path, length);

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
    Logging::log_debug ("passthrough-truncate (" + std::to_string (fd) + ")");

    // perform original POSIX ftruncate operation
    int result = ((real_ftruncate_t)dlsym (RTLD_NEXT, "ftruncate")) (fd, length);

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

// passthrough_stat call. (...)
int PosixPassthrough::passthrough_stat (const char* path, struct stat* statbuf)
{
    std::cout << "One more stat ... \n";
    return ((real_stat_t)dlsym (RTLD_NEXT, "stat")) (path, statbuf);
}

// passthrough_fstat call. (...)
int PosixPassthrough::passthrough_fstat (int fd, struct stat* statbuf)
{
    std::cout << "One more fstat ... \n";
    return ((real_fstat_t)dlsym (RTLD_NEXT, "fstat")) (fd, statbuf);
}

// passthrough_link call. (...)
int PosixPassthrough::passthrough_link (const char* old_path, const char* new_path)
{
    // logging message
    Logging::log_debug (
        "passthrough-link (" + std::string (old_path) + ", " + std::string (new_path) + ")");

    // perform original POSIX link operation
    int result = ((real_link_t)dlsym (RTLD_NEXT, "link")) (old_path, new_path);

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
    Logging::log_debug ("passthrough-unlink (" + std::string (path) + ")");

    // perform original POSIX unlink operation
    int result = ((real_unlink_t)dlsym (RTLD_NEXT, "unlink")) (path);

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
    Logging::log_debug ("passthrough-linkat (" + std::to_string (olddirfd) + ", "
        + std::string (old_path) + ", " + std::to_string (newdirfd) + ", " + std::string (new_path)
        + ")");

    // perform original POSIX linkat operation
    int result = ((
        real_linkat_t)dlsym (RTLD_NEXT, "linkat")) (olddirfd, old_path, newdirfd, new_path, flags);

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
    Logging::log_debug ("passthrough-unlinkat (" + std::to_string (dirfd) + ", "
        + std::string (pathname) + ", " + std::to_string (flags) + ")");

    // perform original POSIX unlinkat operation
    int result = ((real_unlinkat_t)dlsym (RTLD_NEXT, "unlinkat")) (dirfd, pathname, flags);

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
    std::cout << "One more rename ... \n";
    return ((real_rename_t)dlsym (RTLD_NEXT, "rename")) (old_path, new_path);
}

// passthrough_mkdir call. (...)
int PosixPassthrough::passthrough_mkdir (const char* path)
{
    std::cout << "One more mkdir ... \n";
    return ((real_mkdir_t)dlsym (RTLD_NEXT, "mkdir")) (path);
}

// passthrough_readdir call. (...)
struct dirent* PosixPassthrough::passthrough_readdir (DIR* dirp)
{
    std::cout << "One more readdir ... \n";
    return ((real_readdir_t)dlsym (RTLD_NEXT, "readdir")) (dirp);
}

// passthrough_opendir call. (...)
DIR* PosixPassthrough::passthrough_opendir (const char* path)
{
    std::cout << "One more opendir ... \n";
    return ((real_opendir_t)dlsym (RTLD_NEXT, "opendir")) (path);
}

// passthrough_closedir call. (...)
int PosixPassthrough::passthrough_closedir (DIR* dirp)
{
    std::cout << "One more closedir ... \n";
    return ((real_closedir_t)dlsym (RTLD_NEXT, "closedir")) (dirp);
}

// passthrough_rmdir call. (...)
int PosixPassthrough::passthrough_rmdir (const char* path)
{
    std::cout << "One more rmdir ... \n";
    return ((real_rmdir_t)dlsym (RTLD_NEXT, "rmdir")) (path);
}

// passthrough_getxattr call. (...)
ssize_t PosixPassthrough::passthrough_getxattr (const char* path,
    const char* name,
    void* value,
    size_t size)
{
    std::cout << "One more getxattr ... \n";
    return ((real_getxattr_t)dlsym (RTLD_NEXT, "getxattr")) (path, name, value, size);
}

// passthrough_fgetxattr call. (...)
ssize_t PosixPassthrough::passthrough_fgetxattr (int fd, const char* name, void* value, size_t size)
{
    std::cout << "One more fgetxattr ... \n";
    return ((real_fgetxattr_t)dlsym (RTLD_NEXT, "fgetxattr")) (fd, name, value, size);
}

// passthrough_setxattr call. (...)
int PosixPassthrough::passthrough_setxattr (const char* path,
    const char* name,
    const void* value,
    size_t size,
    int flags)
{
    std::cout << "One more setxattr ... \n";
    return ((real_setxattr_t)dlsym (RTLD_NEXT, "setxattr")) (path, name, value, size, flags);
}

// passthrough_fsetxattr call. (...)
int PosixPassthrough::passthrough_fsetxattr (int fd,
    const char* name,
    const void* value,
    size_t size,
    int flags)
{
    std::cout << "One more fsetxattr ... \n";
    return ((real_fsetxattr_t)dlsym (RTLD_NEXT, "fsetxattr")) (fd, name, value, size, flags);
}

// passthrough_fread call. (...)
size_t PosixPassthrough::passthrough_fread (void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    std::cout << "One more fread ... \n";
    return ((real_fread_t)dlsym (RTLD_NEXT, "fread")) (ptr, size, nmemb, stream);
}

} // namespace ldpaio