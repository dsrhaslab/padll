/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef LDPAIO_POSIX_PASSTHROUGH_HPP
#define LDPAIO_POSIX_PASSTHROUGH_HPP

#define __USE_GNU
#define _GNU_SOURCE

#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ldpaio/statistics/statistics.hpp>

namespace ldpaio {

/**
 * Metadata calls.
 */
typedef int (*real_open_t) (const char*, int, ...);
typedef int (*real_open_simple_t) (const char*, int);
typedef int (*real_creat_t) (const char*, mode_t);
typedef int (*real_openat_t) (int, const char*, int, ...);
typedef int (*real_openat_simple_t) (int, const char*, int);
typedef int (*real_open64_t) (const char*, int, ...);
typedef int (*real_open64_simple_t) (const char*, int);
typedef int (*real_close_t) (int);
typedef int (*real_fsync_t) (int);
typedef int (*real_fdatasync_t) (int);
typedef int (*real_truncate_t) (const char*, off_t);
typedef int (*real_ftruncate_t) (int, off_t);
typedef int (*real_stat_t) (const char*, struct stat*);
typedef int (*real_fstat_t) (int fd, struct stat*);
typedef int (*real_link_t) (const char*, const char*);
typedef int (*real_unlink_t) (const char*);
typedef int (*real_rename_t) (const char*, const char*);

/**
 * Data calls.
 */
typedef ssize_t (*real_read_t) (int, void*, size_t);
typedef ssize_t (*real_write_t) (int, const void*, size_t);
typedef ssize_t (*real_pread_t) (int, void*, size_t, off_t);
typedef ssize_t (*real_pwrite_t) (int, const void*, size_t, off_t);
typedef size_t (*real_fread_t) (void*, size_t, size_t, FILE*);
// removed fwrite (segfault) ... need to add later

/**
 * Directory calls
 */
typedef int (*real_mkdir_t) (const char*);
typedef struct dirent* (*real_readdir_t) (DIR*);
typedef DIR* (*real_opendir_t) (const char*);
typedef int (*real_closedir_t) (DIR*);
typedef int (*real_rmdir_t) (const char*);

/**
 * Extended attributes calls.
 */
typedef ssize_t (*real_getxattr_t) (const char*, const char*, void*, size_t);
typedef ssize_t (*real_fgetxattr_t) (int, const char*, void*, size_t);
typedef int (*real_setxattr_t) (const char*, const char*, const void*, size_t, int);
typedef int (*real_fsetxattr_t) (int, const char*, const void*, size_t, int);

class PosixPassthrough {
private:
    Statistics m_metadata_stats { "posix-passthrough-metadata", OperationType::metadata_calls };
    Statistics m_data_stats { "posix-passthrough-data", OperationType::data_calls };
    Statistics m_dir_stats { "posix-passthrough-directory", OperationType::directory_calls };
    Statistics m_ext_attr_stats { "posix-passthrough-ext-attr", OperationType::ext_attr_calls };

public:
    /**
     * PosixPassthrough default constructor.
     */
    PosixPassthrough ();

    /**
     * PosixPassthrough default destructor.
     */
    ~PosixPassthrough ();

    std::string to_string ();

    /**
     * passthrough_read:
     * @param fd
     * @param buf
     * @param counter
     * @return
     */
    ssize_t passthrough_read (int fd, void* buf, ssize_t counter);

    /**
     * passthrough_write:
     * @param fd
     * @param buf
     * @param counter
     * @return
     */
    ssize_t passthrough_write (int fd, const void* buf, ssize_t counter);

    /**
     * passthrough_pread:
     * @param fd
     * @param buf
     * @param counter
     * @param offset
     * @return
     */
    ssize_t passthrough_pread (int fd, void* buf, ssize_t counter, off_t offset);

    /**
     * passthrough_pwrite:
     * @param fd
     * @param buf
     * @param counter
     * @param offset
     * @return
     */
    ssize_t passthrough_pwrite (int fd, const void* buf, ssize_t counter, off_t offset);

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
    int passthrough_open (const char* path, int flags, mode_t mode);

    /**
     * passthrough_open:
     * @param path
     * @param flags
     * @return
     */
    int passthrough_open (const char* path, int flags);

    /**
     * passthrough_creat:
     * @param path
     * @param mode
     * @return
     */
    int passthrough_creat (const char* path, mode_t mode);

    /**
     * passthrough_openat:
     * @param dirfd
     * @param path
     * @param flags
     * @param mode
     * @return
     */
    int passthrough_openat (int dirfd, const char* path, int flags, mode_t mode);

    /**
     * passthrough_openat:
     * @param dirfd
     * @param path
     * @param flags
     * @return
     */
    int passthrough_openat (int dirfd, const char* path, int flags);

    /**
     * passthrough_open64:
     *  https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/open64.c.html
     * @param path
     * @param flags
     * @param mode
     * @return
     */
    int passthrough_open64 (const char* path, int flags, mode_t mode);

    /**
     * passthrough_open64:
     *  https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/open64.c.html
     * @param path
     * @param flags
     * @return
     */
    int passthrough_open64 (const char* path, int flags);

    /**
     * passthrough_close:
     * @param fd
     * @return
     */
    int passthrough_close (int fd);

    /**
     * passthrough_fsync:
     * @param fd
     * @return
     */
    int passthrough_fsync (int fd);

    /**
     * passthrough_fdatasync:
     * @param fd
     * @return
     */
    int passthrough_fdatasync (int fd);

    /**
     * passthrough_truncate:
     * @param path
     * @param length
     * @return
     */
    int passthrough_truncate (const char* path, off_t length);

    /**
     * passthrough_ftruncate:
     * @param fd
     * @param length
     * @return
     */
    int passthrough_ftruncate (int fd, off_t length);

    /**
     * passthrough_link:
     * @param old_path
     * @param new_path
     * @return
     */
    int passthrough_link (const char* old_path, const char* new_path);

    /**
     * passthrough_unlink:
     * @param old_path
     * @return
     */
    int passthrough_unlink (const char* old_path);

    /**
     * passthrough_rename:
     * @param old_path
     * @param new_path
     * @return
     */
    int passthrough_rename (const char* old_path, const char* new_path);

    /**
     * mkdir:
     * @param path
     * @return
     */
    int passthrough_mkdir (const char* path);

    /**
     * readdir:
     *  https://man7.org/linux/man-pages/man3/readdir.3.html
     * @param dirp
     * @return
     */
    struct dirent* passthrough_readdir (DIR* dirp);

    /**
     * opendir:
     *  https://man7.org/linux/man-pages/man3/opendir.3.html
     * @param path
     * @return
     */
    DIR* passthrough_opendir (const char* path);

    /**
     * closedir:
     *  https://man7.org/linux/man-pages/man3/closedir.3.html
     * @param dirp
     * @return
     */
    int passthrough_closedir (DIR* dirp);

    /**
     * rmdir:
     *  https://man7.org/linux/man-pages/man2/rmdir.2.html
     * @param path
     * @return
     */
    int passthrough_rmdir (const char* path);

    /**
     * passthrough_getxattr:
     *  https://man7.org/linux/man-pages/man2/getxattr.2.html
     * @param path
     * @param name
     * @param value
     * @param size
     * @return
     */
    ssize_t passthrough_getxattr (const char* path, const char* name, void* value, size_t size);

    /**
     * passthrough_fgetxattr:
     *  https://man7.org/linux/man-pages/man2/fgetxattr.2.html
     * @param fd
     * @param name
     * @param value
     * @param size
     * @return
     */
    ssize_t passthrough_fgetxattr (int fd, const char* name, void* value, size_t size);

    /**
     * passthrough_setxattr:
     *  https://man7.org/linux/man-pages/man2/setxattr.2.html
     * @param path
     * @param name
     * @param value
     * @param size
     * @param flags
     * @return
     */
    int passthrough_setxattr (const char* path,
        const char* name,
        const void* value,
        size_t size,
        int flags);

    /**
     * passthrough_fsetxattr:
     *  https://man7.org/linux/man-pages/man2/fsetxattr.2.html
     * @param fd
     * @param name
     * @param value
     * @param size
     * @param flags
     * @return
     */
    int passthrough_fsetxattr (int fd, const char* name, const void* value, size_t size, int flags);

    /**
     * passthrough_stat:
     *  https://man7.org/linux/man-pages/man2/stat.2.html
     * @param path
     * @param statbuf
     * @return
     */
    int passthrough_stat (const char* path, struct stat* statbuf);

    /**
     * passthrough_fstat:
     *  https://man7.org/linux/man-pages/man2/fstat.2.html
     * @param fd
     * @param statbuf
     * @return
     */
    int passthrough_fstat (int fd, struct stat* statbuf);

    /**
     * passthrough_fread:
     *  https://man7.org/linux/man-pages/man3/fread.3.html
     * @param ptr
     * @param size
     * @param nmemb
     * @param stream
     * @return
     */
    size_t passthrough_fread (void* ptr, size_t size, size_t nmemb, FILE* stream);
};
} // namespace ldpaio

#endif // LDPAIO_POSIX_PASSTHROUGH_HPP
