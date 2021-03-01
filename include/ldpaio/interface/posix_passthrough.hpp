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
#include <iostream>
#include <dirent.h>

namespace ldpaio {

typedef ssize_t (*real_read_t) (int, void*, size_t);
typedef ssize_t (*real_write_t) (int, const void*, size_t);
typedef ssize_t (*real_pread_t) (int, void*, size_t, off_t);
typedef ssize_t (*real_pwrite_t) (int, const void*, size_t, off_t);
typedef int (*real_open_t) (const char*, int, ...);
typedef int (*real_open_simple_t) (const char*, int);
typedef int (*real_creat_t) (const char*, mode_t);
typedef int (*real_openat_t) (int, const char*, int, ...);
typedef int (*real_openat_simple_t) (int, const char*, int);
typedef int (*real_close_t) (int);
typedef int (*real_fsync_t) (int);
typedef int (*real_fdatasync_t) (int);
typedef int (*real_truncate_t) (const char*, off_t);
typedef int (*real_ftruncate_t) (int, off_t);
typedef int (*real_link_t) (const char*, const char*);
typedef int (*real_unlink_t) (const char*);
typedef int (*real_rename_t) (const char*, const char*);
typedef int (*real_mkdir_t) (const char*);
typedef struct dirent* (*real_readdir_t) (DIR*);

class PosixPassthrough {

public:

    /**
     * passthrough_read:
     * @param fd
     * @param buf
     * @param counter
     * @return
     */
    static ssize_t passthrough_read (int fd, void* buf, ssize_t counter);

    /**
     * passthrough_write:
     * @param fd
     * @param buf
     * @param counter
     * @return
     */
    static ssize_t passthrough_write (int fd, const void* buf, ssize_t counter);

    /**
     * passthrough_pread:
     * @param fd
     * @param buf
     * @param counter
     * @param offset
     * @return
     */
    static ssize_t passthrough_pread (int fd, void* buf, ssize_t counter, off_t offset);

    /**
     * passthrough_pwrite:
     * @param fd
     * @param buf
     * @param counter
     * @param offset
     * @return
     */
    static ssize_t passthrough_pwrite (int fd, const void* buf, ssize_t counter, off_t offset);

    /**
     * passthrough_open:
     * Notes:
     *  https://github.com/fritzw/ld-preload-open/blob/master/path-mapping.c
     *  https://github.com/poliva/ldpreloadhook/blob/master/hook.c
     * @param path
     * @param flags
     * @param mode
     * @return
     */
    static int passthrough_open (const char* path, int flags, mode_t mode);

    /**
     * passthrough_open:
     * @param path
     * @param flags
     * @return
     */
    static int passthrough_open (const char* path, int flags);

    /**
     * passthrough_creat:
     * @param path
     * @param mode
     * @return
     */
    static int passthrough_creat (const char* path, mode_t mode);

    /**
     * passthrough_openat:
     * @param dirfd
     * @param path
     * @param flags
     * @param mode
     * @return
     */
    static int passthrough_openat (int dirfd, const char* path, int flags, mode_t mode);

    /**
     * passthrough_openat:
     * @param dirfd
     * @param path
     * @param flags
     * @return
     */
    static int passthrough_openat (int dirfd, const char* path, int flags);

    /**
     * passthrough_close:
     * @param fd
     * @return
     */
    static int passthrough_close (int fd);

    /**
     * passthrough_fsync:
     * @param fd
     * @return
     */
    static int passthrough_fsync (int fd);

    /**
     * passthrough_fdatasync:
     * @param fd
     * @return
     */
    static int passthrough_fdatasync (int fd);

    /**
     * passthrough_truncate:
     * @param path
     * @param length
     * @return
     */
    static int passthrough_truncate (const char* path, off_t length);

    /**
     * passthrough_ftruncate:
     * @param fd
     * @param length
     * @return
     */
    static int passthrough_ftruncate (int fd, off_t length);

    /**
     * passthrough_link:
     * @param old_path
     * @param new_path
     * @return
     */
    static int passthrough_link (const char* old_path, const char* new_path);

    /**
     * passthrough_unlink:
     * @param old_path
     * @return
     */
    static int passthrough_unlink (const char* old_path);

    /**
     * passthrough_rename:
     * @param old_path
     * @param new_path
     * @return
     */
    static int passthrough_rename (const char* old_path, const char* new_path);

    /**
     * mkdir:
     * @param path
     * @return
     */
    static int passthrough_mkdir (const char* path);

    /**
     * readdir:
     * @param dirp
     * @return
     */
    static struct dirent* passthrough_readdir (DIR* dirp);



};
} // namespace ldpaio

#endif // LDPAIO_POSIX_PASSTHROUGH_HPP
