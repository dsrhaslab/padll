/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef LDPAIO_POSIX_FILE_SYSTEM_H
#define LDPAIO_POSIX_FILE_SYSTEM_H

#include <cstring>
#include <unistd.h>

ssize_t read (int fd, void* buf, size_t size);

ssize_t write (int fd, const void* buf, size_t size);

ssize_t pread (int fd, void* buf, size_t size, off_t offset);

ssize_t pwrite (int fd, const void* buf, size_t size, off_t offset);

int open (const char* pathname, int flags);

int open64 (const char* pathname, int flags, mode_t mode);


#endif // LDPAIO_POSIX_FILE_SYSTEM_H
