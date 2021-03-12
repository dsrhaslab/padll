/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef LDPAIO_OPERATION_UTILS_HPP
#define LDPAIO_OPERATION_UTILS_HPP

#include <ldpaio/thrid_party/enum.h>

namespace ldpaio {

/**
 * OperationType class.
 */
BETTER_ENUM (OperationType,
    int,
    metadata_calls = 1,
    data_calls = 2,
    directory_calls = 3,
    ext_attr_calls = 4);

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
    truncate = 11,
    ftruncate = 12,
    stat = 13,
    lstat = 14,
    fstat = 15,
    fstatat = 16,
    link = 17,
    unlink = 18,
    linkat = 19,
    unlinkat = 20,
    rename = 21,
    renameat = 22,
    symlink = 23,
    symlinkat = 24,
    readlink = 25,
    readlinkat = 26);

/**
 * Data Definitions.
 */
BETTER_ENUM (Data, int, no_op = 0, read = 1, write = 2, pread = 3, pwrite = 4, fread = 5);

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

} // namespace ldpaio

#endif // LDPAIO_OPERATION_UTILS_HPP
