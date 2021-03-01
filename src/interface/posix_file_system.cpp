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
int open (const char* pathname, int flags, ...)
{
    if (flags & O_CREAT) {
        va_list args;

        va_start (args, flags);
        mode_t mode = va_arg (args, int);
        va_end (args);

        return ldpaio::PosixPassthrough::passthrough_open (pathname, flags, mode);
    } else {
        return ldpaio::PosixPassthrough::passthrough_open (pathname, flags);
    }
}

// creat call. (...)
int creat (const char* pathname, mode_t mode)
{
    return ldpaio::PosixPassthrough::passthrough_creat (pathname, mode);
}

// openat call. (...)
int openat (int dirfd, const char* pathname, int flags, ...)
{
    if (flags & O_CREAT) {
        va_list args;

        va_start (args, flags);
        mode_t mode = va_arg (args, int);
        va_end (args);

        return ldpaio::PosixPassthrough::passthrough_openat (dirfd, pathname, flags, mode);
    } else {
        return ldpaio::PosixPassthrough::passthrough_openat (dirfd, pathname, flags);
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
int truncate (const char* pathname, off_t length)
{
    return ldpaio::PosixPassthrough::passthrough_truncate (pathname, length);
}

// ftruncate call. (...)
int ftruncate (int fd, off_t length)
{
    return ldpaio::PosixPassthrough::passthrough_ftruncate (fd, length);
}

int link (const char* old_pathname, const char* new_pathname)
{
    return ldpaio::PosixPassthrough::passthrough_link (old_pathname, new_pathname);
}

int unlink (const char* pathname)
{
    return ldpaio::PosixPassthrough::passthrough_unlink (pathname);
}

