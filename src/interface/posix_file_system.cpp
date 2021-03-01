/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <ldpaio/interface/posix_file_system.hpp>

// read call. ...
ssize_t read (int fd, void* buf, size_t size)
{
    std::cout << "One more read ...\n";
    return ldpaio::PosixPassthrough::passthrough_read (fd, buf, size);
}

// write call. ...
ssize_t write (int fd, const void* buf, size_t size)
{
    std::cout << "One more write ...\n";
    return ldpaio::PosixPassthrough::passthrough_write (fd, buf, size);
}

// pread call. ...
ssize_t pread (int fd, void* buf, size_t size, off_t offset)
{
    std::cout << "One more pread ...\n";
    return ldpaio::PosixPassthrough::passthrough_pread (fd, buf, size, offset);
}

// pwrite call. ...
ssize_t pwrite (int fd, const void* buf, size_t size, off_t offset)
{
    std::cout << "One more pwrite ...\n";
    return ldpaio::PosixPassthrough::passthrough_pwrite (fd, buf, size, offset);
}

// open call. ...
int open (const char* pathname, int flags, ...) {
    if (flags & O_CREAT) {
        va_list args;

        va_start (args, flags);
        mode_t mode = va_arg (args, int);
        va_end (args);

        std::cout << "One more open (w/ mode) ... \n";
        return ldpaio::PosixPassthrough::passthrough_open (pathname, flags, mode);
    } else {
        std::cout << "One more open (w/o mode) ... \n";
        return ldpaio::PosixPassthrough::passthrough_open (pathname, flags);
    }
}

