/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef LDPAIO_POSIX_PASSTHROUGH_HPP
#define LDPAIO_POSIX_PASSTHROUGH_HPP

#define __USE_GNU
#define _GNU_SOURCE

#include <dlfcn.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace ldpaio {

typedef ssize_t (*real_read_t) (int, void*, size_t);
typedef ssize_t (*real_write_t) (int, const void*, size_t);
typedef ssize_t (*real_pread_t) (int, void*, size_t, off_t);
typedef ssize_t (*real_pwrite_t) (int, const void*, size_t, off_t);
// typedef int (*real_open_t) (const char*, int);
typedef int (*real_open_t) (const char*, int, ...);


class PosixPassthrough {

public:

    static ssize_t passthrough_read (int fd, void *buf, ssize_t counter);

    static ssize_t passthrough_write (int fd, const void *buf, ssize_t counter);

    static ssize_t passthrough_pread (int fd, void *buf, ssize_t counter, off_t offset);

    static ssize_t passthrough_pwrite (int fd, const void *buf, ssize_t counter, off_t offset);

// open calls
    // static int passthrough_open (const char* pathname, int flags);

    static int passthrough_open (const char* pathname, int flags, mode_t mode);

    static int passthrough_creat (const char* pathname, mode_t mode);

    static int passthrough_openat (int dirfd, const char* pathname, int flags);

    static int passthrough_openat (int dirfd, const char* pathname, int flags, mode_t mode);

};
} // namespace ldpaio

#endif // LDPAIO_POSIX_PASSTHROUGH_HPP
