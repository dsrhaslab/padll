/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
 **/

#ifndef PADLL_LIBC_HEADERS_HPP
#define PADLL_LIBC_HEADERS_HPP

#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sstream>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef __linux__
#include <sys/vfs.h>
#include <sys/xattr.h>
#elif __APPLE__
#include <sys/mount.h>
#endif

namespace padll::headers {

/**
 * Metadata calls.
 */
typedef int (*libc_open_variadic_t) (const char*, int, ...);
typedef int (*libc_open_t) (const char*, int);
typedef int (*libc_creat_t) (const char*, mode_t);
typedef int (*libc_creat64_t) (const char*, mode_t);
typedef int (*libc_openat_variadic_t) (int, const char*, int, ...);
typedef int (*libc_openat_t) (int, const char*, int);
typedef int (*libc_open64_variadic_t) (const char*, int, ...);
typedef int (*libc_open64_t) (const char*, int);
typedef int (*libc_close_t) (int);
typedef void (*libc_sync_t) ();
// typedef int (*libc_xstat_t) (int, const char*, struct stat*);
// typedef int (*libc_lxstat_t) (int, const char*, struct stat*);
// typedef int (*libc_fxstat_t) (int, int, struct stat*);
// typedef int (*libc_fxstatat_t) (int, int, const char*, struct stat*, int);
// typedef int (*libc_xstat64_t) (int, const char*, struct stat64*);
// typedef int (*libc_lxstat64_t) (int, const char*, struct stat64*);
// typedef int (*libc_fxstat64_t) (int, int, struct stat64*);
// typedef int (*libc_fxstatat64_t) (int, int, const char*, struct stat64*, int);
typedef int (*libc_statfs_t) (const char*, struct statfs*);
typedef int (*libc_fstatfs_t) (int, struct statfs*);
typedef int (*libc_statfs64_t) (const char*, struct statfs64*);
typedef int (*libc_fstatfs64_t) (int, struct statfs64*);
typedef int (*libc_unlink_t) (const char*);
typedef int (*libc_unlinkat_t) (int, const char*, int);
typedef int (*libc_rename_t) (const char*, const char*);
typedef int (*libc_renameat_t) (int, const char*, int, const char*);
typedef FILE* (*libc_fopen_t) (const char*, const char*);
typedef FILE* (*libc_fopen64_t) (const char*, const char*);
typedef int (*libc_fclose_t) (FILE*);

/**
 * libc_metadata struct: provides an object with the function pointers to all libc metadata-like
 * operations.
 */
struct libc_metadata {
    libc_open_variadic_t m_open_var { nullptr };
    libc_open_t m_open { nullptr };
    libc_creat_t m_creat { nullptr };
    libc_creat64_t m_creat64 { nullptr };
    libc_openat_variadic_t m_openat_var { nullptr };
    libc_openat_t m_openat { nullptr };
    libc_open64_variadic_t m_open64_var { nullptr };
    libc_open64_t m_open64 { nullptr };
    libc_close_t m_close { nullptr };
    libc_sync_t m_sync { nullptr };
    libc_statfs_t m_statfs { nullptr };
    libc_fstatfs_t m_fstatfs { nullptr };
    libc_statfs64_t m_statfs64 { nullptr };
    libc_fstatfs64_t m_fstatfs64 { nullptr };
    libc_unlink_t m_unlink { nullptr };
    libc_unlinkat_t m_unlinkat { nullptr };
    libc_rename_t m_rename { nullptr };
    libc_renameat_t m_renameat { nullptr };
    libc_fopen_t m_fopen { nullptr };
    libc_fopen64_t m_fopen64 { nullptr };
    libc_fclose_t m_fclose { nullptr };
};

/**
 * Data calls.
 */
typedef ssize_t (*libc_read_t) (int, void*, size_t);
typedef ssize_t (*libc_write_t) (int, const void*, size_t);
typedef ssize_t (*libc_pread_t) (int, void*, size_t, off_t);
typedef ssize_t (*libc_pwrite_t) (int, const void*, size_t, off_t);
#if defined(__USE_LARGEFILE64)
typedef ssize_t (*libc_pread64_t) (int, void*, size_t, off64_t);
typedef ssize_t (*libc_pwrite64_t) (int, const void*, size_t, off64_t);
#endif
typedef void* (*libc_mmap_t) (void*, size_t, int, int, int, off_t);
typedef int (*libc_munmap_t) (void*, size_t);

/**
 * libc_data: provides an object with the function pointers to all libc data-like operations.
 */
struct libc_data {
    libc_read_t m_read { nullptr };
    libc_write_t m_write { nullptr };
    libc_pread_t m_pread { nullptr };
    libc_pwrite_t m_pwrite { nullptr };
#if defined(__USE_LARGEFILE64)
    libc_pread64_t m_pread64 { nullptr };
    libc_pwrite64_t m_pwrite64 { nullptr };
#endif
    libc_mmap_t m_mmap { nullptr };
    libc_munmap_t m_munmap { nullptr };
};

/**
 * Directory calls.
 */
typedef int (*libc_mkdir_t) (const char*, mode_t);
typedef int (*libc_mkdirat_t) (int, const char*, mode_t);
typedef int (*libc_rmdir_t) (const char*);
typedef int (*libc_mknod_t) (const char*, mode_t, dev_t);
typedef int (*libc_mknodat_t) (int, const char*, mode_t, dev_t);

/**
 * libc_directory: provides an object with the function pointers to all libc directory-like
 * operations.
 */
struct libc_directory {
    libc_mkdir_t m_mkdir { nullptr };
    libc_mkdirat_t m_mkdirat { nullptr };
    libc_rmdir_t m_rmdir { nullptr };
    libc_mknod_t m_mknod { nullptr };
    libc_mknodat_t m_mknodat { nullptr };
};

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

/**
 * libc_extattr: provides an object with the function pointers to all libc extended attributes
 * operations.
 */
struct libc_extattr {
    libc_getxattr_t m_getxattr { nullptr };
    libc_lgetxattr_t m_lgetxattr { nullptr };
    libc_fgetxattr_t m_fgetxattr { nullptr };
    libc_setxattr_t m_setxattr { nullptr };
    libc_lsetxattr_t m_lsetxattr { nullptr };
    libc_fsetxattr_t m_fsetxattr { nullptr };
    libc_listxattr_t m_listxattr { nullptr };
    libc_llistxattr_t m_llistxattr { nullptr };
    libc_flistxattr_t m_flistxattr { nullptr };
};

} // namespace padll::headers

#endif // PADLL_LIBC_HEADERS_HPP
