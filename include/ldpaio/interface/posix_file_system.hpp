/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef LDPAIO_POSIX_FILE_SYSTEM_H
#define LDPAIO_POSIX_FILE_SYSTEM_H

#include <cstdarg>
#include <cstring>
#include <ldpaio/interface/posix_passthrough.hpp>
#include <unistd.h>

/**
 * read:
 * @param fd
 * @param buf
 * @param size
 * @return
 */
ssize_t read (int fd, void* buf, size_t size);

/**
 * write:
 * @param fd
 * @param buf
 * @param size
 * @return
 */
ssize_t write (int fd, const void* buf, size_t size);

/**
 * pread:
 * @param fd
 * @param buf
 * @param size
 * @param offset
 * @return
 */
ssize_t pread (int fd, void* buf, size_t size, off_t offset);

/**
 * pwrite:
 * @param fd
 * @param buf
 * @param size
 * @param offset
 * @return
 */
ssize_t pwrite (int fd, const void* buf, size_t size, off_t offset);

/**
 * open:
 * @param pathname
 * @param flags
 * @param ...
 * @return
 */
extern "C" int open (const char* pathname, int flags, ...);

/**
 * creat:
 * @param pathname
 * @param mode
 * @return
 */
int creat (const char* pathname, mode_t mode);

/**
 * openat:
 * @param dirfd
 * @param pathname
 * @param flags
 * @param ...
 * @return
 */
extern "C" int openat (int dirfd, const char* pathname, int flags, ...);

/**
 * close:
 * @param fd
 * @return
 */
int close (int fd);

#endif // LDPAIO_POSIX_FILE_SYSTEM_H
