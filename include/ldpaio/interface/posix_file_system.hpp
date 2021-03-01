/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef LDPAIO_POSIX_FILE_SYSTEM_H
#define LDPAIO_POSIX_FILE_SYSTEM_H

#include <cstdarg>
#include <cstring>
#include <ldpaio/interface/posix_passthrough.hpp>

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
 * @param path
 * @param flags
 * @param ...
 * @return
 */
extern "C" int open (const char* path, int flags, ...);

/**
 * creat:
 * @param path
 * @param mode
 * @return
 */
int creat (const char* path, mode_t mode);

/**
 * openat:
 * @param dirfd
 * @param path
 * @param flags
 * @param ...
 * @return
 */
extern "C" int openat (int dirfd, const char* path, int flags, ...);

/**
 * close:
 * @param fd
 * @return
 */
int close (int fd);

/**
 * fsync:
 * @param fd
 * @return
 */
int fsync (int fd);

/**
 * fdatasync:
 * @param fd
 * @return
 */
int fdatasync (int fd);

/**
 * truncate:
 * @param path
 * @param length
 * @return
 */
int truncate (const char* path, off_t length);

/**
 * ftruncate:
 * @param fd
 * @param length
 * @return
 */
int ftruncate (int fd, off_t length);

/**
 * link:
 * @param old_path
 * @param new_path
 * @return
 */
int link (const char* old_path, const char* new_path);

/**
 * unlink:
 * @param path
 * @return
 */
int unlink (const char* path);

/**
 * rename:
 * @param old_path
 * @param new_path
 * @return
 */
int rename (const char* old_path, const char* new_path);

/**
 * mkdir:
 * @param path
 * @param mode
 * @return
 */
int mkdir (const char* path, mode_t mode);

/**
 * readdir:
 * @param dirp
 * @return
 */
struct dirent* readdir (DIR* dirp);

/**
 * opendir:
 * @param path
 * @return
 */
DIR* opendir (const char* path);

/**
 * closedir:
 * @param dirp
 * @return
 */
int closedir (DIR* dirp);

/**
 * rmdir:
 * @param path
 * @return
 */
int rmdir (const char* path);


#endif // LDPAIO_POSIX_FILE_SYSTEM_H
