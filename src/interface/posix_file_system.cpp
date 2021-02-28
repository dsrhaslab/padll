/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#define __USE_GNU
#define _GNU_SOURCE

#include <ldpaio/interface/posix_file_system.hpp>
#include <ldpaio/interface/posix_passthrough.hpp>
#include <cstring>
#include <iostream>
#include <dlfcn.h>


typedef ssize_t (*real_read_t)(int, void*, size_t);

ssize_t real_read (int fd, void* data, size_t size) {
    return ((real_read_t) dlsym (RTLD_NEXT, "read"))(fd, data, size);
}

ssize_t read (int fd, void* buf, size_t size)
{
    std::cout << "One more read ...\n";
    // return ldpaio::passthrough_read (fd, buf, size);
    return real_read (fd, buf, size);
}