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

/**
 * getxattr:
 * @param path
 * @param name
 * @param value
 * @param size
 * @return
 */
ssize_t getxattr (const char* path, const char* name, void* value, size_t size);

/**
 * fgetxattr:
 * @param fd
 * @param name
 * @param value
 * @param size
 * @return
 */
ssize_t fgetxattr (int fd, const char* name, void* value, size_t size);

/**
 * setxattr:
 * @param path
 * @param name
 * @param value
 * @param size
 * @param flags
 * @return
 */
int setxattr (const char* path, const char* name, const void* value, size_t size, int flags);

/**
 * fsetxattr:
 * @param fd
 * @param name
 * @param value
 * @param size
 * @param flags
 * @return
 */
int fsetxattr (int fd, const char* name, const void* value, size_t size, int flags);

/**
 * stat:
 * @param path
 * @param statbuf
 * @return
 */
int stat (const char* path, struct stat* statbuf);

/**
 * fstat:
 * @param fd
 * @param statbuf
 * @return
 */
int fstat (int fd, struct stat* statbuf);


#endif // LDPAIO_POSIX_FILE_SYSTEM_H
