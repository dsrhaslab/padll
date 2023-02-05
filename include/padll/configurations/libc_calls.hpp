/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#ifndef PADLL_LIBC_CALLS_HPP
#define PADLL_LIBC_CALLS_HPP

/**
 * PosixDataCalls struct.
 * Defines which data-based POSIX operations (read, write, ...) should be handled by PADLL.
 * Operations set to false will follow the passthrough workflow.
 */
struct PosixDataCalls {
    bool padll_intercept_read = false;
    bool padll_intercept_write = false;
    bool padll_intercept_pread = false;
    bool padll_intercept_pwrite = false;
#if defined(__USE_LARGEFILE64)
    bool padll_intercept_pread64 = false;
    bool padll_intercept_pwrite64 = false;
#endif
    bool padll_intercept_mmap = false;
    bool padll_intercept_munmap = false;
};

/**
 * PosixDirectoryCalls struct.
 * Defines which directory-based POSIX operations (mkdir, mknod, ...) should be handled by PADLL.
 * Operations set to false will follow the passthrough workflow.
 */
struct PosixDirectoryCalls {
    bool padll_intercept_mkdir = false;
    bool padll_intercept_mkdirat = false;
    bool padll_intercept_rmdir = false;
    bool padll_intercept_mknod = false;
    bool padll_intercept_mknodat = false;
};

/**
 * PosixExtendedAttributesCalls struct.
 * Defines which extended attributes based POSIX operations (getxattr, setxattr, ...) should be
 * handled by PADLL. Operations set to false will follow the passthrough workflow.
 */
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

/**
 * PosixMetadataCalls struct.
 * Defines which metadata-based POSIX operations (open, close, unlink, ...) should be handled by
 * PADLL. Operations set to false will follow the passthrough workflow.
 */
struct PosixMetadataCalls {
    bool padll_intercept_open_var = true;
    bool padll_intercept_open = true;
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

/**
 * PosixSpecialCalls struct.
 * Defines special POSIX operations that should be managed with PADLL, for internal organization
 * (for example, both ::socket and ::fcntl calls generate file descriptors).
 */
struct PosixSpecialCalls {
    bool padll_intercept_socket = false;
    bool padll_intercept_fcntl = false;
};

const static PosixDataCalls posix_data_calls;
const static PosixDirectoryCalls posix_directory_calls;
const static PosixExtendedAttributesCalls posix_extended_attributes_calls;
const static PosixMetadataCalls posix_metadata_calls;
const static PosixSpecialCalls posix_special_calls;

#endif // PADLL_LIBC_CALLS_HPP
