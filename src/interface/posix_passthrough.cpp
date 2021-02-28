/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#include <ldpaio/interface/posix_passthrough.hpp>

namespace ldpaio {

    ssize_t passthrough_read (int fd, void* buf, ssize_t counter) {
        return ((real_read_t) dlsym (RTLD_NEXT, "read"))(fd, buf, counter);
    }


}