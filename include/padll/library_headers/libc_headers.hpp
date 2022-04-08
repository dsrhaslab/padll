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
using libc_open_variadic_t = int (*)(const char*, int, ...);
using libc_open_t = int (*)(const char*, int);
using libc_creat_t = int (*)(const char*, mode_t);
using libc_creat64_t = int (*)(const char*, mode_t);
using libc_openat_variadic_t = int (*)(int, const char*, int, ...);
using libc_openat_t = int (*)(int, const char*, int);
using libc_open64_variadic_t = int (*)(const char*, int, ...);
using libc_open64_t = int (*)(const char*, int);
using libc_close_t = int (*)(int);
using libc_sync_t = void (*)();
using libc_statfs_t = int (*)(const char*, struct statfs*);
using libc_fstatfs_t = int (*)(int, struct statfs*);
using libc_statfs64_t = int (*)(const char*, struct statfs64*);
using libc_fstatfs64_t = int (*)(int, struct statfs64*);
using libc_unlink_t = int (*)(const char*);
using libc_unlinkat_t = int (*)(int, const char*, int);
using libc_rename_t = int (*)(const char*, const char*);
using libc_renameat_t = int (*)(int, const char*, int, const char*);
using libc_fopen_t = FILE* (*)(const char*, const char*);
using libc_fopen64_t = FILE* (*)(const char*, const char*);
using libc_fclose_t = int (*)(FILE*);

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
using libc_read_t = ssize_t (*)(int, void*, size_t);
using libc_write_t = ssize_t (*)(int, const void*, size_t);
using libc_pread_t = ssize_t (*)(int, void*, size_t, off_t);
using libc_pwrite_t = ssize_t (*)(int, const void*, size_t, off_t);
#if defined(__USE_LARGEFILE64)
using libc_pread64_t = ssize_t (*)(int, void*, size_t, off64_t);
using libc_pwrite64_t = ssize_t (*)(int, const void*, size_t, off64_t);
#endif
using libc_mmap_t = void* (*)(void*, size_t, int, int, int, off_t);
using libc_munmap_t = int (*)(void*, size_t);

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
using libc_mkdir_t = int (*)(const char*, mode_t);
using libc_mkdirat_t = int (*)(int, const char*, mode_t);
using libc_rmdir_t = int (*)(const char*);
using libc_mknod_t = int (*)(const char*, mode_t, dev_t);
using libc_mknodat_t = int (*)(int, const char*, mode_t, dev_t);

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
using libc_getxattr_t = ssize_t (*)(const char*, const char*, void*, size_t);
using libc_lgetxattr_t = ssize_t (*)(const char*, const char*, void*, size_t);
using libc_fgetxattr_t = ssize_t (*)(int, const char*, void*, size_t);
using libc_setxattr_t = int (*)(const char*, const char*, const void*, size_t, int);
using libc_lsetxattr_t = int (*)(const char*, const char*, const void*, size_t, int);
using libc_fsetxattr_t = int (*)(int, const char*, const void*, size_t, int);
using libc_listxattr_t = ssize_t (*)(const char*, char*, size_t);
using libc_llistxattr_t = ssize_t (*)(const char*, char*, size_t);
using libc_flistxattr_t = ssize_t (*)(int, char*, size_t);

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
