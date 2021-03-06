/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#ifndef LDPAIO_OPERATION_UTILS_HPP
#define LDPAIO_OPERATION_UTILS_HPP

namespace ldpaio {

enum class OperationType {
    metadata_calls = 1,
    data_calls = 2,
    directory_calls = 3,
    ext_attr_calls = 4
};

// ------------------------------------------------------------------------------------

/**
 * METADATA_CALLS Definitions.
 */
enum class METADATA {
    open = 1,
    open_simple = 2,
    creat = 3,
    openat = 4,
    openat_simple = 5,
    open64 = 6,
    open64_simple = 7,
    close = 8,
    fsync = 9,
    fdatasync = 10,
    truncate = 11,
    ftruncate = 12,
    stat = 13,
    fstat = 14,
    link = 15,
    unlink = 16,
    rename = 17,
    no_op = 0
};

const int metadata_calls_size = 18;

// ------------------------------------------------------------------------------------

enum class DATA {
    read = 1,
    write = 2,
    pread = 3,
    pwrite = 4,
    fread = 5,
    no_op = 0
};

const int data_calls_size = 6;

// ------------------------------------------------------------------------------------

enum class DIRECTORY {
    mkdir = 1,
    readdir = 2,
    opendir = 3,
    closedir = 4,
    rmdir = 6,
    no_op = 0
};

const int directory_calls_size = 7;

// ------------------------------------------------------------------------------------

enum class EXT_ATTR {
    getxattr = 1,
    fgetxattr = 2,
    setxattr = 3,
    fsetxattr = 4,
    no_op = 0
};

const int ext_attr_calls_size = 5;

}


#endif //LDPAIO_OPERATION_UTILS_HPP
