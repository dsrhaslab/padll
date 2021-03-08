/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <ldpaio/interface/posix_passthrough.hpp>

namespace ldpaio {

PosixPassthrough::PosixPassthrough ()
{
    std::cout << "posix passthrough default constructor\n";
}

PosixPassthrough::~PosixPassthrough ()
{
    std::cout << "posix passthrough default destructor\n";
}

std::string PosixPassthrough::to_string ()
{
    return "Hello world from PosixPassthrough";
}

// passthrough_read call.
ssize_t PosixPassthrough::passthrough_read (int fd, void* buf, ssize_t counter)
{
    std::cout << "One more read ...\n";
    int result =  ((real_read_t)dlsym (RTLD_NEXT, "read")) (fd, buf, counter);

    if (result >= 0) {
        this->m_data_stats.update_statistic_entry (static_cast<int>(Data::read), 1, result);
    }
    return result;
}

// passthrough_write call.
ssize_t PosixPassthrough::passthrough_write (int fd, const void* buf, ssize_t counter)
{
    std::cout << "One more write ...\n";
    int result = ((real_write_t)dlsym (RTLD_NEXT, "write")) (fd, buf, counter);

    if (result >= 0) {
        this->m_data_stats.update_statistic_entry (static_cast<int>(Data::write), 1, result);
    }
    return result;
}

// passthrough_pread call.
ssize_t PosixPassthrough::passthrough_pread (int fd, void* buf, ssize_t counter, off_t offset)
{
    std::cout << "One more pread ...\n";
    return ((real_pread_t)dlsym (RTLD_NEXT, "pread")) (fd, buf, counter, offset);
}

// passthrough_pwrite call.
ssize_t
PosixPassthrough::passthrough_pwrite (int fd, const void* buf, ssize_t counter, off_t offset)
{
    std::cout << "One more pwrite ...\n";
    return ((real_pwrite_t)dlsym (RTLD_NEXT, "pwrite")) (fd, buf, counter, offset);
}

// passthrough_open call.
int PosixPassthrough::passthrough_open (const char* path, int flags, mode_t mode)
{
    std::cout << "One more open (w/ mode) ... \n";
    return ((real_open_t)dlsym (RTLD_NEXT, "open")) (path, flags, mode);
}

// passthrough_open call.
int PosixPassthrough::passthrough_open (const char* path, int flags)
{
    std::cout << "One more open (w/o mode) ... \n";
    return ((real_open_simple_t)dlsym (RTLD_NEXT, "open")) (path, flags);
}

// passthrough_creat call.
int PosixPassthrough::passthrough_creat (const char* path, mode_t mode)
{
    std::cout << "One more creat ... \n";
    return ((real_creat_t)dlsym (RTLD_NEXT, "creat")) (path, mode);
}

// passthrough_openat call.
int PosixPassthrough::passthrough_openat (int dirfd, const char* path, int flags, mode_t mode)
{
    std::cout << "One more openat (w/ mode) ... \n";
    return ((real_openat_t)dlsym (RTLD_NEXT, "openat")) (dirfd, path, flags, mode);
}

// passthrough_openat call.
int PosixPassthrough::passthrough_openat (int dirfd, const char* path, int flags)
{
    std::cout << "One more openat (w/o mode) ... \n";
    return ((real_openat_simple_t)dlsym (RTLD_NEXT, "openat")) (dirfd, path, flags);
}

// passthrough_open64 call. (...)
int PosixPassthrough::passthrough_open64 (const char* path, int flags, mode_t mode)
{
    std::cout << "One more open64 (w/ mode) ... \n";
    return ((real_open64_t)dlsym (RTLD_NEXT, "open64")) (path, flags, mode);
}

// passthrough_open64 call. (...)
int PosixPassthrough::passthrough_open64 (const char* path, int flags)
{
    std::cout << "One more open64 (w/o mode) ... \n";
    return ((real_open64_simple_t)dlsym (RTLD_NEXT, "open64")) (path, flags);
}

// passthrough_close call.
int PosixPassthrough::passthrough_close (int fd)
{
    std::cout << "One more close ... \n";
    return ((real_close_t)dlsym (RTLD_NEXT, "close")) (fd);
}

// passthrough_fsync call. (...)
int PosixPassthrough::passthrough_fsync (int fd)
{
    std::cout << "One more fsync ...\n";
    return ((real_fsync_t)dlsym (RTLD_NEXT, "fsync")) (fd);
}

// passthrough_fdatasync call. (...)
int PosixPassthrough::passthrough_fdatasync (int fd)
{
    std::cout << "One more fdatasync ...\n";
    return ((real_fdatasync_t)dlsym (RTLD_NEXT, "fdatasync")) (fd);
}

// passthrough_truncate call. (...)
int PosixPassthrough::passthrough_truncate (const char* path, off_t length)
{
    std::cout << "One more truncate ... \n";
    return ((real_truncate_t)dlsym (RTLD_NEXT, "truncate")) (path, length);
}

// passthrough_truncate call. (...)
int PosixPassthrough::passthrough_ftruncate (int fd, off_t length)
{
    std::cout << "One more ftruncate ... \n";
    return ((real_ftruncate_t)dlsym (RTLD_NEXT, "ftruncate")) (fd, length);
}

// passthrough_link call. (...)
int PosixPassthrough::passthrough_link (const char* old_path, const char* new_path)
{
    std::cout << "One more link ... \n";
    return ((real_link_t)dlsym (RTLD_NEXT, "link")) (old_path, new_path);
}

// passthrough_unlink call. (...)
int PosixPassthrough::passthrough_unlink (const char* path)
{
    std::cout << "One more unlink ... \n";
    return ((real_unlink_t)dlsym (RTLD_NEXT, "unlink")) (path);
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

// passthrough_fread call. (...)
size_t PosixPassthrough::passthrough_fread (void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    std::cout << "One more fread ... \n";
    return ((real_fread_t)dlsym (RTLD_NEXT, "fread")) (ptr, size, nmemb, stream);
}

} // namespace ldpaio