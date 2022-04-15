/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
 **/

#ifndef PADLL_LIBC_CALLS_HPP
#define PADLL_LIBC_CALLS_HPP

struct PosixDataCalls {
    bool padll_intercept_read = true;
    bool padll_intercept_write = true;
    bool padll_intercept_pread = false;
    bool padll_intercept_pwrite = false;
#if defined(__USE_LARGEFILE64)
    bool padll_intercept_pread64 = false;
    bool padll_intercept_pwrite64 = false;
#endif
    bool padll_intercept_mmap = false;
    bool padll_intercept_munmap = false;
};

struct PosixDirectoryCalls {
    bool padll_intercept_mkdir = true;
    bool padll_intercept_mkdirat = true;
    bool padll_intercept_rmdir = true;
    bool padll_intercept_mknod = true;
    bool padll_intercept_mknodat = true;
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
    bool padll_intercept_open_var = false;
    bool padll_intercept_open = false;
    bool padll_intercept_creat = false;
    bool padll_intercept_creat64 = false;
    bool padll_intercept_openat_var = false;
    bool padll_intercept_openat = false;
    bool padll_intercept_open64_var = false;
    bool padll_intercept_open64 = false;
    bool padll_intercept_close = false;
    bool padll_intercept_sync = false;
    bool padll_intercept_statfs = false;
    bool padll_intercept_fstatfs = false;
    bool padll_intercept_statfs64 = false;
    bool padll_intercept_fstatfs64 = false;
    bool padll_intercept_unlink = false;
    bool padll_intercept_unlinkat = false;
    bool padll_intercept_rename = false;
    bool padll_intercept_renameat = false;
    bool padll_intercept_fopen = false;
    bool padll_intercept_fopen64 = false;
    bool padll_intercept_fclose = false;
};

struct PosixSpecialCalls {
    bool padll_intercept_socket = true;
    bool padll_intercept_fcntl = true;
};

const static PosixDataCalls posix_data_calls;
const static PosixDirectoryCalls posix_directory_calls;
const static PosixExtendedAttributesCalls posix_extended_attributes_calls;
const static PosixMetadataCalls posix_metadata_calls;
const static PosixSpecialCalls posix_special_calls;

#endif // PADLL_LIBC_CALLS_HPP
