/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef PADLL_LIBC_CALLS_HPP
#define PADLL_LIBC_CALLS_HPP


struct PosixDataCalls {
    bool padll_intercept_read = false;
    bool padll_intercept_write = true;
};

static PosixDataCalls posix_data_calls;

#endif // PADLL_LIBC_CALLS_HPP
