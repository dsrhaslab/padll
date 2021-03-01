/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <ldpaio/interface/posix_file_system.hpp>

// read call. (...)
ssize_t read (int fd, void* buf, size_t size)
{
    return ldpaio::PosixPassthrough::passthrough_read (fd, buf, size);
}

// write call. (...)
ssize_t write (int fd, const void* buf, size_t size)
{
    return ldpaio::PosixPassthrough::passthrough_write (fd, buf, size);
}

// pread call. (...)
ssize_t pread (int fd, void* buf, size_t size, off_t offset)
{
    return ldpaio::PosixPassthrough::passthrough_pread (fd, buf, size, offset);
}

// pwrite call. (...)
ssize_t pwrite (int fd, const void* buf, size_t size, off_t offset)
{
    return ldpaio::PosixPassthrough::passthrough_pwrite (fd, buf, size, offset);
}

// open call. (...)
int open (const char* path, int flags, ...)
{
    if (flags & O_CREAT) {
        va_list args;

        va_start (args, flags);
        mode_t mode = va_arg (args, int);
        va_end (args);

        return ldpaio::PosixPassthrough::passthrough_open (path, flags, mode);
    } else {
        return ldpaio::PosixPassthrough::passthrough_open (path, flags);
    }
}

// creat call. (...)
int creat (const char* path, mode_t mode)
{
    return ldpaio::PosixPassthrough::passthrough_creat (path, mode);
}

// openat call. (...)
int openat (int dirfd, const char* path, int flags, ...)
{
    if (flags & O_CREAT) {
        va_list args;

        va_start (args, flags);
        mode_t mode = va_arg (args, int);
        va_end (args);

        return ldpaio::PosixPassthrough::passthrough_openat (dirfd, path, flags, mode);
    } else {
        return ldpaio::PosixPassthrough::passthrough_openat (dirfd, path, flags);
    }
}

// close call. (...)
int close (int fd)
{
    return ldpaio::PosixPassthrough::passthrough_close (fd);
}

// fsync call. (...)
int fsync (int fd)
{
    return ldpaio::PosixPassthrough::passthrough_fsync (fd);
}

// fdatasync call. (...)
int fdatasync (int fd)
{
    return ldpaio::PosixPassthrough::passthrough_fdatasync (fd);
}

// truncate call. (...)
int truncate (const char* path, off_t length)
{
    return ldpaio::PosixPassthrough::passthrough_truncate (path, length);
}

// ftruncate call. (...)
int ftruncate (int fd, off_t length)
{
    return ldpaio::PosixPassthrough::passthrough_ftruncate (fd, length);
}

// link call. (...)
int link (const char* old_path, const char* new_path)
{
    return ldpaio::PosixPassthrough::passthrough_link (old_path, new_path);
}

// unlink call. (...)
int unlink (const char* path)
{
    return ldpaio::PosixPassthrough::passthrough_unlink (path);
}

// rename call. (...)
int rename (const char* old_path, const char* new_path)
{
    return ldpaio::PosixPassthrough::passthrough_rename (old_path, new_path);
}

// mkdir call. (...)
int mkdir (const char* path)
{
    return ldpaio::PosixPassthrough::passthrough_mkdir (path);
}

// readdir call. (...)
struct dirent* readdir (DIR* dirp)
{
    return ldpaio::PosixPassthrough::passthrough_readdir (dirp);
}

// opendir call. (...)
DIR* opendir (const char* path)
{
    return ldpaio::PosixPassthrough::passthrough_opendir (path);
}

// closedir call. (...)
int closedir (DIR* dirp)
{
    return ldpaio::PosixPassthrough::passthrough_closedir (dirp);
}

// rmdir call. (...)
int rmdir (const char* path)
{
    return ldpaio::PosixPassthrough::passthrough_rmdir (path);
}

// getxattr call. (...)
ssize_t getxattr (const char* path, const char* name, void* value, size_t size)
{
    return ldpaio::PosixPassthrough::passthrough_getxattr (path, name, value, size);
}

// fgetxattr call. (...)
ssize_t fgetxattr (int fd, const char* name, void* value, size_t size)
{
    return ldpaio::PosixPassthrough::passthrough_fgetxattr (fd, name, value, size);
}