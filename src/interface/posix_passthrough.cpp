/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <ldpaio/interface/posix_passthrough.hpp>

namespace ldpaio {

// passthrough_read call.
ssize_t PosixPassthrough::passthrough_read (int fd, void* buf, ssize_t counter)
{
    std::cout << "One more read ...\n";
    return ((real_read_t)dlsym (RTLD_NEXT, "read")) (fd, buf, counter);
}

// passthrough_write call.
ssize_t PosixPassthrough::passthrough_write (int fd, const void* buf, ssize_t counter)
{
    std::cout << "One more write ...\n";
    return ((real_write_t)dlsym (RTLD_NEXT, "write")) (fd, buf, counter);
}

// passthrough_pread call.
ssize_t PosixPassthrough::passthrough_pread (int fd, void* buf, ssize_t counter, off_t offset)
{
    std::cout << "One more pread ...\n";
    return ((real_pread_t)dlsym (RTLD_NEXT, "pread")) (fd, buf, counter, offset);
}

// passthrough_pwrite call.
ssize_t PosixPassthrough::passthrough_pwrite (int fd,
    const void* buf,
    ssize_t counter,
    off_t offset)
{
    std::cout << "One more pwrite ...\n";
    return ((real_pwrite_t)dlsym (RTLD_NEXT, "pwrite")) (fd, buf, counter, offset);
}

// passthrough_open call.
int PosixPassthrough::passthrough_open (const char* pathname, int flags, mode_t mode)
{
    std::cout << "One more open (w/ mode) ... \n";
    return ((real_open_t)dlsym (RTLD_NEXT, "open")) (pathname, flags, mode);
}

// passthrough_open call.
int PosixPassthrough::passthrough_open (const char* pathname, int flags)
{
    std::cout << "One more open (w/o mode) ... \n";
    return ((real_open_simple_t)dlsym (RTLD_NEXT, "open")) (pathname, flags);
}

// passthrough_creat call.
int PosixPassthrough::passthrough_creat (const char* pathname, mode_t mode) {
    std::cout << "One more creat ... \n";
    return ((real_creat_t)dlsym (RTLD_NEXT, "creat")) (pathname, mode);
}

// passthrough_openat call.
int PosixPassthrough::passthrough_openat (int dirfd, const char* pathname, int flags, mode_t mode)
{
    std::cout << "One more openat (w/ mode) ... \n";
    return ((real_openat_t)dlsym (RTLD_NEXT, "openat")) (dirfd, pathname, flags, mode);
}

// passthrough_openat call.
int PosixPassthrough::passthrough_openat (int dirfd, const char* pathname, int flags)
{
    std::cout << "One more openat (w/o mode) ... \n";
    return ((real_openat_simple_t)dlsym (RTLD_NEXT, "openat")) (dirfd, pathname, flags);
}

// passthrough_close call.
int PosixPassthrough::passthrough_close (int fd)
{
    std::cout << "One more close ... \n";
    return ((real_close_t)dlsym (RTLD_NEXT, "close")) (fd);
}


} // namespace ldpaio