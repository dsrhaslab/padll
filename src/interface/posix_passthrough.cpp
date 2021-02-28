/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <ldpaio/interface/posix_passthrough.hpp>

namespace ldpaio {

ssize_t PosixPassthrough::passthrough_read (int fd, void* buf, ssize_t counter)
{
    return ((real_read_t)dlsym (RTLD_NEXT, "read")) (fd, buf, counter);
}

ssize_t PosixPassthrough::passthrough_write (int fd, const void* buf, ssize_t counter)
{
    return ((real_write_t)dlsym (RTLD_NEXT, "write")) (fd, buf, counter);
}

ssize_t PosixPassthrough::passthrough_pread (int fd, void* buf, ssize_t counter, off_t offset)
{
    return ((real_pread_t)dlsym (RTLD_NEXT, "pread")) (fd, buf, counter, offset);
}

ssize_t PosixPassthrough::passthrough_pwrite (int fd, const void* buf, ssize_t counter, off_t offset)
{
    return ((real_pwrite_t)dlsym (RTLD_NEXT, "pwrite")) (fd, buf, counter, offset);
}

int PosixPassthrough::passthrough_open (const char* pathname, int flags) {
    return ((real_open_t)dlsym (RTLD_NEXT, "open")) (pathname, flags);
}

int PosixPassthrough::passthrough_open (const char* pathname, int flags, mode_t mode) {
    return ((real_open_2_t)dlsym (RTLD_NEXT, "open")) (pathname, flags, mode);
}

} // namespace ldpaio