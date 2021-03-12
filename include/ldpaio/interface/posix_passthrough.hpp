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
#include <ldpaio/statistics/statistics.hpp>
#include <ldpaio/utils/options.hpp>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace ldpaio {

/**
 * Metadata calls.
 */
typedef int (*libc_open_variadic_t) (const char*, int, ...);
typedef int (*libc_open_t) (const char*, int);
typedef int (*libc_creat_t) (const char*, mode_t);
typedef int (*libc_openat_variadic_t) (int, const char*, int, ...);
typedef int (*libc_openat_t) (int, const char*, int);
typedef int (*libc_open64_variadic_t) (const char*, int, ...);
typedef int (*libc_open64_t) (const char*, int);
typedef int (*libc_close_t) (int);
typedef int (*libc_fsync_t) (int);
typedef int (*libc_fdatasync_t) (int);
typedef int (*libc_truncate_t) (const char*, off_t);
typedef int (*libc_ftruncate_t) (int, off_t);
typedef int (*libc_stat_t) (const char*, struct stat*);
typedef int (*libc_fstat_t) (int fd, struct stat*);
typedef int (*libc_link_t) (const char*, const char*);
typedef int (*libc_unlink_t) (const char*);
typedef int (*libc_linkat_t) (int, const char*, int, const char*, int);
typedef int (*libc_unlinkat_t) (int, const char*, int);
typedef int (*libc_rename_t) (const char*, const char*);

/**
 * Data calls.
 */
typedef ssize_t (*libc_read_t) (int, void*, size_t);
typedef ssize_t (*libc_write_t) (int, const void*, size_t);
typedef ssize_t (*libc_pread_t) (int, void*, size_t, off_t);
typedef ssize_t (*libc_pwrite_t) (int, const void*, size_t, off_t);
typedef size_t (*libc_fread_t) (void*, size_t, size_t, FILE*);
// removed fwrite (segfault) ... need to add later

/**
 * Directory calls
 */
typedef int (*libc_mkdir_t) (const char*, mode_t);
typedef int (*libc_mkdirat_t) (int, const char*, mode_t);
typedef struct dirent* (*libc_readdir_t) (DIR*);
typedef DIR* (*libc_opendir_t) (const char*);
typedef DIR* (*libc_fdopendir_t) (int);
typedef int (*libc_closedir_t) (DIR*);
typedef int (*libc_rmdir_t) (const char*);
typedef int (*libc_dirfd_t) (DIR*);

/**
 * Extended attributes calls.
 */
typedef ssize_t (*libc_getxattr_t) (const char*, const char*, void*, size_t);
typedef ssize_t (*libc_lgetxattr_t) (const char*, const char*, void*, size_t);
typedef ssize_t (*libc_fgetxattr_t) (int, const char*, void*, size_t);
typedef int (*libc_setxattr_t) (const char*, const char*, const void*, size_t, int);
typedef int (*libc_lsetxattr_t) (const char*, const char*, const void*, size_t, int);
typedef int (*libc_fsetxattr_t) (int, const char*, const void*, size_t, int);
typedef ssize_t (*libc_listxattr_t) (const char*, char*, size_t);
typedef ssize_t (*libc_llistxattr_t) (const char*, char*, size_t);
typedef ssize_t (*libc_flistxattr_t) (int, char*, size_t);
typedef int (*libc_removexattr_t) (const char*, const char*);
typedef int (*libc_lremovexattr_t) (const char*, const char*);
typedef int (*libc_fremovexattr_t) (int, const char*);

class PosixPassthrough {

private:
    std::atomic<bool> m_collect { option_default_statistic_collection };
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
     * PosixPassthrough parameterized constructor.
     * @param stat_collection Boolean that defines if statistic collection is enabled or disabled.
     */
    explicit PosixPassthrough (bool stat_collection);

    /**
     * PosixPassthrough default destructor.
     */
    ~PosixPassthrough ();

    /**
     * get_statistic_entry:
     * @param operation_type
     * @param operation_entry
     * @return
     */
    StatisticEntry get_statistic_entry (const OperationType& operation_type,
        const int& operation_entry);

    /**
     * set_statistic_collection:
     * @param value
     * @return
     */
    void set_statistic_collection (bool value);

    /**
     * to_string:
     * @return
     */
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
     * passthrough_linkat:
     * @param olddirfd
     * @param old_path
     * @param newdirfd
     * @param new_path
     * @param flags
     * @return
     */
    int passthrough_linkat (int olddirfd,
        const char* old_path,
        int newdirfd,
        const char* new_path,
        int flags);

    /**
     * passthrough_unlinkat:
     * @param dirfd
     * @param pathname
     * @param flags
     * @return
     */
    int passthrough_unlinkat (int dirfd, const char* pathname, int flags);

    /**
     * passthrough_rename:
     * @param old_path
     * @param new_path
     * @return
     */
    int passthrough_rename (const char* old_path, const char* new_path);

    /**
     * passthrough_mkdir:
     * @param path
     * @param mode
     * @return
     */
    int passthrough_mkdir (const char* path, mode_t mode);

    /**
     * passthrough_mkdirat:
     *  https://man7.org/linux/man-pages/man2/mkdirat.2.html
     * @param dirfd
     * @param path
     * @param mode
     * @return
     */
    int passthrough_mkdirat (int dirfd, const char* path, mode_t mode);

    /**
     * passthrough_readdir:
     *  https://man7.org/linux/man-pages/man3/readdir.3.html
     * @param dirp
     * @return
     */
    struct dirent* passthrough_readdir (DIR* dirp);

    /**
     * passthrough_opendir:
     *  https://man7.org/linux/man-pages/man3/opendir.3.html
     * @param path
     * @return
     */
    DIR* passthrough_opendir (const char* path);

    /**
     * passthrough_fdopendir:
     *  https://linux.die.net/man/3/fdopendir
     * @param fd
     * @return
     */
    DIR* passthrough_fdopendir (int fd);

    /**
     * passthrough_closedir:
     *  https://man7.org/linux/man-pages/man3/closedir.3.html
     * @param dirp
     * @return
     */
    int passthrough_closedir (DIR* dirp);

    /**
     * passthrough_rmdir:
     *  https://man7.org/linux/man-pages/man2/rmdir.2.html
     * @param path
     * @return
     */
    int passthrough_rmdir (const char* path);

    /**
     * passthrough_dirfd:
     *  https://man7.org/linux/man-pages/man3/dirfd.3.html
     * @param dirp
     * @return
     */
    int passthrough_dirfd (DIR* dirp);

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
     * passthrough_lgetxattr:
     *  https://linux.die.net/man/2/lgetxattr
     * @param path
     * @param name
     * @param value
     * @param size
     * @return
     */
    ssize_t passthrough_lgetxattr (const char* path, const char* name, void* value, size_t size);

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
     * passthrough_lsetxattr:
     *  https://linux.die.net/man/2/lsetxattr
     * @param path
     * @param name
     * @param value
     * @param size
     * @param flags
     * @return
     */
    int passthrough_lsetxattr (const char* path,
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
     * passthrough_listxattr:
     *  https://linux.die.net/man/2/listxattr
     * @param path
     * @param list
     * @param size
     * @return
     */
    ssize_t passthrough_listxattr (const char* path, char* list, size_t size);

    /**
     * passthrough_llistxattr:
     *  https://linux.die.net/man/2/llistxattr
     * @param path
     * @param list
     * @param size
     * @return
     */
    ssize_t passthrough_llistxattr (const char* path, char* list, size_t size);

    /**
     * passthrough_flistxattr:
     *  https://linux.die.net/man/2/flistxattr
     * @param fd
     * @param list
     * @param size
     * @return
     */
    ssize_t passthrough_flistxattr (int fd, char* list, size_t size);

    /**
     * passthrough_removexattr:
     *  https://linux.die.net/man/2/removexattr
     * @param path
     * @param name
     * @return
     */
    int passthrough_removexattr (const char* path, const char* name);

    /**
     * passthrough_lremovexattr:
     *  https://linux.die.net/man/2/lremovexattr
     * @param path
     * @param name
     * @return
     */
    int passthrough_lremovexattr (const char* path, const char* name);

    /**
     * passthrough_fremovexattr:
     *  https://linux.die.net/man/2/fremovexattr
     * @param fd
     * @param name
     * @return
     */
    int passthrough_fremovexattr (int fd, const char* name);

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
