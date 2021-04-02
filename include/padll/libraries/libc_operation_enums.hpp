/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef PADLL_LIBC_OPERATION_ENUMS_HPP
#define PADLL_LIBC_OPERATION_ENUMS_HPP

#include <padll/thrid_party/enum.h>

namespace padll {

/**
 * OperationType class.
 */
BETTER_ENUM (OperationType,
    int,
    metadata_calls = 1,
    data_calls = 2,
    directory_calls = 3,
    ext_attr_calls = 4,
    file_mode_calls = 5);

/**
 * Metadata Definitions.
 */
BETTER_ENUM (Metadata,
    int,
    no_op = 0,
    open_variadic = 1,
    open = 2,
    creat = 3,
    openat_variadic = 4,
    openat = 5,
    open64_variadic = 6,
    open64 = 7,
    close = 8,
    fsync = 9,
    fdatasync = 10,
    sync = 11,
    syncfs = 12,
    truncate = 13,
    ftruncate = 14,
    truncate64 = 15,
    ftruncate64 = 16,
    stat = 17,
    lstat = 18,
    fstat = 19,
    fstatat = 20,
    stat64 = 21,
    lstat64 = 22,
    fstat64 = 23,
    fstatat64 = 24,
    statfs = 25,
    fstatfs = 26,
    statfs64 = 27,
    fstatfs64 = 28,
    link = 29,
    unlink = 30,
    linkat = 31,
    unlinkat = 32,
    rename = 33,
    renameat = 34,
    symlink = 35,
    symlinkat = 36,
    readlink = 37,
    readlinkat = 38,
    fopen = 39,
    fopen64 = 40,
    fdopen = 41,
    freopen = 42,
    freopen64 = 43,
    fclose = 44,
    fflush = 45,
    access = 46,
    faccessat = 47,
    lseek = 48,
    fseek = 49,
    ftell = 50,
    lseek64 = 51,
    fseeko64 = 52,
    ftello64 = 53);

/**
 * Data Definitions.
 */
BETTER_ENUM (Data,
    int,
    no_op = 0,
    read = 1,
    write = 2,
    pread = 3,
    pwrite = 4,
    fread = 5,
    fwrite = 6);

/**
 * Directory Definitions.
 */
BETTER_ENUM (Directory,
    int,
    no_op = 0,
    mkdir = 1,
    mkdirat = 2,
    readdir = 3,
    opendir = 4,
    fdopendir = 5,
    closedir = 6,
    rmdir = 7,
    dirfd = 8);

/**
 * ExtendedAttributes Definitions.
 */
BETTER_ENUM (ExtendedAttributes,
    int,
    no_op = 0,
    getxattr = 1,
    lgetxattr = 2,
    fgetxattr = 3,
    setxattr = 4,
    lsetxattr = 5,
    fsetxattr = 6,
    listxattr = 7,
    llistxattr = 8,
    flistxattr = 9,
    removexattr = 10,
    lremovexattr = 11,
    fremovexattr = 12);

/**
 * FileModes definitions.
 */
BETTER_ENUM (FileModes,
    int,
    no_op = 0,
    chmod = 1,
    fchmod = 2,
    fchmodat = 3,
    chown = 4,
    lchown = 5,
    fchown = 6,
    fchownat = 7);

} // namespace padll

#endif // PADLL_LIBC_OPERATION_ENUMS_HPP
