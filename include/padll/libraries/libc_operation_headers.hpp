/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#ifndef PADLL_LIBC_OPERATION_HEADERS_HPP
#define PADLL_LIBC_OPERATION_HEADERS_HPP

#include <sys/stat.h>
#ifdef __linux__
#include <sys/vfs.h>
#elif __APPLE__
#include <sys/mount.h>
#endif

namespace padll {

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
    typedef void (*libc_sync_t) ();
    typedef int (*libc_syncfs_t) (int);
    typedef int (*libc_truncate_t) (const char*, off_t);
    typedef int (*libc_ftruncate_t) (int, off_t);
    typedef int (*libc_xstat_t) (int, const char*, struct stat*);
    typedef int (*libc_lxstat_t) (int, const char*, struct stat*);
    typedef int (*libc_fxstat_t) (int, int, struct stat*);
    typedef int (*libc_fxstatat_t) (int, int, const char*, struct stat*, int);
    typedef int (*libc_statfs_t) (const char*, struct statfs*);
    typedef int (*libc_fstatfs_t) (int, struct statfs*);
    typedef int (*libc_link_t) (const char*, const char*);
    typedef int (*libc_unlink_t) (const char*);
    typedef int (*libc_linkat_t) (int, const char*, int, const char*, int);
    typedef int (*libc_unlinkat_t) (int, const char*, int);
    typedef int (*libc_rename_t) (const char*, const char*);
    typedef int (*libc_renameat_t) (int, const char*, int, const char*);
    typedef int (*libc_symlink_t) (const char*, const char*);
    typedef int (*libc_symlinkat_t) (const char*, int, const char*);
    typedef ssize_t (*libc_readlink_t) (const char*, char*, size_t);
    typedef ssize_t (*libc_readlinkat_t) (int, const char*, char*, size_t);
    typedef FILE* (*libc_fopen_t) (const char*, const char*);
    typedef FILE* (*libc_fdopen_t) (int, const char*);
    typedef FILE* (*libc_freopen_t) (const char*, const char*, FILE*);
    typedef int (*libc_fclose_t) (FILE*);
    typedef int (*libc_fflush_t) (FILE*);
    typedef int (*libc_access_t) (const char*, int);
    typedef int (*libc_faccessat_t) (int, const char*, int, int);

struct libc_metadata {
    libc_open_variadic_t m_open_var { nullptr };
    libc_open_t m_open { nullptr };
    libc_creat_t m_creat { nullptr };
    libc_openat_variadic_t m_openat_var { nullptr };
    libc_openat_t m_openat { nullptr };
    libc_open64_variadic_t m_open64_var { nullptr };
    libc_open64_t m_open64 { nullptr };
    libc_close_t m_close { nullptr };
    libc_fsync_t m_fsync { nullptr };
    libc_fdatasync_t m_fdatasync { nullptr };
    libc_sync_t m_sync { nullptr };
    libc_syncfs_t m_syncfs { nullptr };
    libc_truncate_t m_truncate { nullptr };
    libc_ftruncate_t m_ftruncate { nullptr };
    libc_xstat_t m_xstat { nullptr };
    libc_lxstat_t m_lxstat { nullptr };
    libc_fxstat_t m_fxstat { nullptr };
    libc_fxstatat_t m_fxstatat { nullptr };
    libc_statfs_t m_statfs { nullptr };
    libc_fstatfs_t m_fstatfs { nullptr };
    libc_link_t m_link { nullptr };
    libc_unlink_t m_unlink { nullptr };
    libc_linkat_t m_linkat { nullptr };
    libc_unlinkat_t m_unlinkat { nullptr };
    libc_rename_t m_rename { nullptr };
    libc_renameat_t m_renameat { nullptr };
    libc_symlink_t m_symlink { nullptr };
    libc_symlinkat_t m_symlinkat { nullptr };
    libc_readlink_t m_readlink { nullptr };
    libc_readlinkat_t m_readlinkat { nullptr };
    libc_fopen_t m_fopen { nullptr };
    libc_fdopen_t m_fdopen { nullptr };
    libc_freopen_t m_freopen { nullptr };
    libc_fclose_t m_fclose { nullptr };
    libc_fflush_t m_fflush { nullptr };
    libc_access_t m_access { nullptr };
    libc_faccessat_t m_faccessat { nullptr };
};

/**
 * Data calls.
 */
    typedef ssize_t (*libc_read_t) (int, void*, size_t);
    typedef ssize_t (*libc_write_t) (int, const void*, size_t);
    typedef ssize_t (*libc_pread_t) (int, void*, size_t, off_t);
    typedef ssize_t (*libc_pwrite_t) (int, const void*, size_t, off_t);
    typedef size_t (*libc_fread_t) (void*, size_t, size_t, FILE*);
    typedef size_t (*libc_fwrite_t) (const void*, size_t, size_t, FILE*);

struct libc_data {
    libc_read_t m_read { nullptr };
    libc_write_t m_write { nullptr };
    libc_pread_t m_pread { nullptr };
    libc_pwrite_t m_pwrite { nullptr };
    libc_fread_t m_fread { nullptr };
    libc_fwrite_t m_fwrite { nullptr };
};

/**
 * Directory calls.
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

/**
 * File modes calls.
 */
    typedef int (*libc_chmod_t) (const char*, mode_t);
    typedef int (*libc_fchmod_t) (int, mode_t);
    typedef int (*libc_fchmodat_t) (int, const char*, mode_t, int);
    typedef int (*libc_chown_t) (const char*, uid_t, gid_t);
    typedef int (*libc_lchown_t) (const char*, uid_t, gid_t);
    typedef int (*libc_fchown_t) (int, uid_t, gid_t);
    typedef int (*libc_fchownat_t) (int, const char*, uid_t, gid_t, int);


}

#endif //PADLL_LIBC_OPERATION_HEADERS_HPP
