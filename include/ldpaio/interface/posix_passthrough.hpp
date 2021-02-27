/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#ifndef LDPAIO_POSIX_PASSTHROUGH_HPP
#define LDPAIO_POSIX_PASSTHROUGH_HPP

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace ldpaio {

    ssize_t passthrough_read (int fd, void* buf, ssize_t counter);

    ssize_t passthrough_write (int fd, const void* buf, ssize_t counter);

    ssize_t passthrough_pread (int fd, void* buf, ssize_t counter, off_t offset);

    ssize_t passthrough_pwrite (int fd, const void* buf, ssize_t counter, off_t offset);

    // open calls
    int passthrough_open (const char* pathname, int flags);

    int passthrough_open (const char* pathname, int flags, mode_t mode);

    int passthrough_creat (const char* pathname, mode_t mode);

    int passthrough_openat (int dirfd, const char* pathname, int flags);

    int passthrough_openat (int dirfd, const char* pathname, int flags, mode_t mode);

}


#endif //LDPAIO_POSIX_PASSTHROUGH_HPP
