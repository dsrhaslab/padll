/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
 **/

#ifndef PADLL_LIBC_ENUMS_HPP
#define PADLL_LIBC_ENUMS_HPP

#include <padll/thrid_party/enum.h>

namespace padll::headers {

/**
 * OperationType class.
 */
BETTER_ENUM (OperationType,
    int,
    metadata_calls = 1,
    data_calls = 2,
    directory_calls = 3,
    ext_attr_calls = 4)

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
    sync = 10,
    stat = 11,
    lstat = 12,
    fstat = 13,
    fstatat = 14,
    stat64 = 15,
    lstat64 = 16,
    fstat64 = 17,
    fstatat64 = 18,
    statfs = 19,
    fstatfs = 29,
    statfs64 = 21,
    fstatfs64 = 22,
    unlink = 23,
    unlinkat = 24,
    rename = 25,
    renameat = 26,
    fopen = 27,
    fopen64 = 28,
    fclose = 29)

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
    pread64 = 5,
    pwrite64 = 6)

/**
 * Directory Definitions.
 */
BETTER_ENUM (Directory, int, no_op = 0, mkdir = 1, mkdirat = 2, rmdir = 3)

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
    flistxattr = 9)

} // namespace padll::headers

#endif // PADLL_LIBC_ENUMS_HPP
