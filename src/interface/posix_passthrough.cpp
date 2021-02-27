/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#include <unistd.h>

namespace ldpaio {

    ssize_t passthrough_read (int fd, void* buf, ssize_t counter) {
        return ::read (fd, buf, counter);
    }

}