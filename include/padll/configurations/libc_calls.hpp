/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
 **/

#ifndef PADLL_LIBC_CALLS_HPP
#define PADLL_LIBC_CALLS_HPP

struct PosixDataCalls {
    bool padll_intercept_read = false;
    bool padll_intercept_write = false;
    bool padll_intercept_pread = false;
    bool padll_intercept_pwrite = false;
#if defined(__USE_LARGEFILE64)
    bool padll_intercept_pread64 = false;
    bool padll_intercept_pwrite64 = false;
#endif
    bool padll_intercept_mmap = false; // todo
    bool padll_intercept_munmap = false; // todo
};

struct PosixDirectoryCalls {
    bool padll_intercept_mkdir = false;
    bool padll_intercept_mkdirat = false;
    bool padll_intercept_rmdir = false;
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
    bool paddl_intercept_sync = false;
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
    bool paddl_intercept_unlink = false;
    bool paddl_intercept_unlinkat = false;
    bool paddl_intercept_rename = false;
    bool paddl_intercept_renameat = false;
    bool paddl_intercept_fopen = false;
    bool paddl_intercept_fopen64 = false;
    bool paddl_intercept_fclose = false;
};

const static PosixDataCalls posix_data_calls;
const static PosixDirectoryCalls posix_directory_calls;
const static PosixExtendedAttributesCalls posix_extended_attributes_calls;
const static PosixMetadataCalls posix_metadata_calls;

#endif // PADLL_LIBC_CALLS_HPP
