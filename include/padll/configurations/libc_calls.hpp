/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef PADLL_LIBC_CALLS_HPP
#define PADLL_LIBC_CALLS_HPP

struct PosixDataCalls {
    bool padll_intercept_read = false;
    bool padll_intercept_write = true;
    bool padll_intercept_pread = false;
    bool padll_intercept_pwrite = true;
    bool padll_intercept_fread = false;
    bool padll_intercept_fwrite = true;
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

static PosixDataCalls posix_data_calls;
static PosixDirectoryCalls posix_directory_calls;
static PosixDirectoryCalls posix_extended_attributes_calls;
static PosixFileModesCalls posix_file_modes_calls;

#endif // PADLL_LIBC_CALLS_HPP
