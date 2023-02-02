/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#ifndef PADLL_LIBC_ENUMS_HPP
#define PADLL_LIBC_ENUMS_HPP

#include <padll/third_party/enum.h>

namespace padll::headers {

/**
 * OperationType class.
 */
BETTER_ENUM (OperationType,
    int,
    metadata_calls = 1,
    data_calls = 2,
    directory_calls = 3,
    ext_attr_calls = 4,
    special_calls = 5)

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
    statfs = 11,
    fstatfs = 12,
    statfs64 = 13,
    fstatfs64 = 14,
    unlink = 15,
    unlinkat = 16,
    rename = 17,
    renameat = 18,
    fopen = 19,
    fopen64 = 20,
    fclose = 21)

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
    pwrite64 = 6,
    mmap = 7,
    munmap = 8)

/**
 * Directory Definitions.
 */
BETTER_ENUM (Directory, int, no_op = 0, mkdir = 1, mkdirat = 2, rmdir = 3, mknod = 4, mknodat = 5)

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

/**
 * Special POSIX calls definitions.
 */
BETTER_ENUM (Special, int, no_op = 0, socket = 1, fcntl = 2)

} // namespace padll::headers

#endif // PADLL_LIBC_ENUMS_HPP
