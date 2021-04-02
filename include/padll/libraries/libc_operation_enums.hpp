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
    open = 1,
    open_variadic = 2,
    open64 = 3,
    open64_variadic = 4,
    creat = 5,
    creat64 = 6,
    openat = 7,
    openat_variadic = 8,
    close = 9,
    fsync = 10,
    fdatasync = 11,
    sync = 12,
    syncfs = 13,
    truncate = 14,
    ftruncate = 15,
    truncate64 = 16,
    ftruncate64 = 17,
    stat = 18,
    lstat = 19,
    fstat = 20,
    fstatat = 21,
    stat64 = 22,
    lstat64 = 23,
    fstat64 = 24,
    fstatat64 = 25,
    statfs = 26,
    fstatfs = 27,
    statfs64 = 28,
    fstatfs64 = 29,
    link = 30,
    unlink = 31,
    linkat = 32,
    unlinkat = 33,
    rename = 34,
    renameat = 35,
    symlink = 36,
    symlinkat = 37,
    readlink = 38,
    readlinkat = 39,
    fopen = 40,
    fopen64 = 41,
    fdopen = 42,
    freopen = 43,
    freopen64 = 44,
    fclose = 45,
    fflush = 46,
    access = 47,
    faccessat = 48,
    lseek = 49,
    fseek = 50,
    ftell = 51,
    lseek64 = 52,
    fseeko64 = 53,
    ftello64 = 54);

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
