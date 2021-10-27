/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef PADLL_LIBC_CALLS_HPP
#define PADLL_LIBC_CALLS_HPP

struct PosixDataCalls {
    bool padll_intercept_read = true;
    bool padll_intercept_write = false;
    bool padll_intercept_pread = false;
    bool padll_intercept_pwrite = false;
    bool padll_intercept_fread = false;
    bool padll_intercept_fwrite = false;
#if defined(__USE_LARGEFILE64)
    bool padll_intercept_pread64 = false;
    bool padll_intercept_pwrite64 = false;
#endif
};

struct PosixDirectoryCalls {
    bool padll_intercept_mkdir = false;
    bool padll_intercept_mkdirat = false;
    bool padll_intercept_readdir = false;
    bool padll_intercept_readdir64 = false;
    bool padll_intercept_opendir = false;
    bool padll_intercept_fdopendir = false;
    bool padll_intercept_closedir = false;
    bool padll_intercept_rmdir = false;
    bool padll_intercept_dirfd = false;
};

struct PosixExtendedAttributesCalls {
    bool padll_intercept_getxattr = false;
    bool padll_intercept_lgetxattr = false;
    bool padll_intercept_fgetxattr = false;
    bool padll_intercept_setxattr = false;
    bool padll_intercept_lsetxattr = false;
    bool padll_intercept_fsetxattr = false;
    bool padll_intercept_listxattr = false;
    bool padll_intercept_llistxattr = false;
    bool padll_intercept_flistxattr = false;
    bool padll_intercept_removexattr = false;
    bool padll_intercept_lremovexattr = false;
    bool padll_intercept_fremovexattr = false;
};

struct PosixFileModesCalls {
    bool padll_intercept_chmod = false;
    bool padll_intercept_fchmod = false;
    bool padll_intercept_fchmodat = false;
    bool padll_intercept_chown = false;
    bool padll_intercept_lchown = false;
    bool padll_intercept_fchown = false;
    bool padll_intercept_fchownat = false;
};

struct PosixMetadataCalls {
    bool paddl_intercept_open_var = false;
    bool paddl_intercept_open = false;
    bool paddl_intercept_creat = false;
    bool paddl_intercept_creat64 = false;
    bool paddl_intercept_openat_var = false;
    bool paddl_intercept_openat = false;
    bool paddl_intercept_open64_var = false;
    bool paddl_intercept_open64 = false;
    bool paddl_intercept_close = false;
    bool paddl_intercept_fsync = false;
    bool paddl_intercept_fdatasync = false;
    bool paddl_intercept_sync = false;
    bool paddl_intercept_syncfs = false;
    bool paddl_intercept_truncate = false;
    bool paddl_intercept_ftruncate = false;
    bool paddl_intercept_truncate64 = false;
    bool paddl_intercept_ftruncate64 = false;
    bool paddl_intercept_xstat = false;
    bool paddl_intercept_lxstat = false;
    bool paddl_intercept_fxstat = false;
    bool paddl_intercept_fxstatat = false;
    bool paddl_intercept_xstat64 = false;
    bool paddl_intercept_lxstat64 = false;
    bool paddl_intercept_fxstat64 = false;
    bool paddl_intercept_fxstatat64 = false;
    bool paddl_intercept_statfs = false;
    bool paddl_intercept_fstatfs = false;
    bool paddl_intercept_statfs64 = false;
    bool paddl_intercept_fstatfs64 = false;
    bool paddl_intercept_link = false;
    bool paddl_intercept_unlink = false;
    bool paddl_intercept_linkat = false;
    bool paddl_intercept_unlinkat = false;
    bool paddl_intercept_rename = false;
    bool paddl_intercept_renameat = false;
    bool paddl_intercept_symlink = false;
    bool paddl_intercept_symlinkat = false;
    bool paddl_intercept_readlink = false;
    bool paddl_intercept_readlinkat = false;
    bool paddl_intercept_fopen = false;
    bool paddl_intercept_fopen64 = false;
    bool paddl_intercept_fdopen = false;
    bool paddl_intercept_freopen = false;
    bool paddl_intercept_freopen64 = false;
    bool paddl_intercept_fclose = false;
    bool paddl_intercept_fflush = false;
    bool paddl_intercept_access = false;
    bool paddl_intercept_faccessat = false;
    bool paddl_intercept_lseek = false;
    bool paddl_intercept_fseek = false;
    bool paddl_intercept_ftell = false;
    bool paddl_intercept_lseek64 = false;
    bool paddl_intercept_fseeko64 = false;
    bool paddl_intercept_ftello64 = false;
};

const static PosixDataCalls posix_data_calls;
const static PosixDirectoryCalls posix_directory_calls;
const static PosixExtendedAttributesCalls posix_extended_attributes_calls;
const static PosixFileModesCalls posix_file_modes_calls;
const static PosixMetadataCalls posix_metadata_calls;

#endif // PADLL_LIBC_CALLS_HPP
