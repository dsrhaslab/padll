/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#ifndef PADLL_POSIX_PASSTHROUGH_H
#define PADLL_POSIX_PASSTHROUGH_H

#include <mutex>
#include <padll/utils/logging.hpp>
#include <padll/libraries/libc_operation_headers.hpp>
#include <string>

namespace padll {

class PosixPassthrough {

private:
    std::mutex m_lock;
    std::string m_lib_name { "libc.so.6" };
    void* m_lib_handle { nullptr };

    /**
     * initialize:
     */
    void initialize ();

    /**
     * dlopen_library_handle:
     * @return
     */
    bool dlopen_library_handle ();

public:

    /**
     * PosixPassthrough default constructor.
     */
    PosixPassthrough ();

    /**
     * PosixPassthrough (explicit) parameterized constructor.
     * @param lib
     */
    explicit PosixPassthrough (const std::string& lib);

    /**
     * PosixPassthrough default destructor.
     */
    ~PosixPassthrough ();

    /**
     * passthrough_posix_read:
     *  https://linux.die.net/man/2/read
     * @param fd
     * @param buf
     * @param counter
     * @return
     */
    ssize_t passthrough_posix_read (int fd, void* buf, ssize_t counter);

    /**
     * passthrough_posix_write:
     *  https://linux.die.net/man/2/write
     * @param fd
     * @param buf
     * @param counter
     * @return
     */
    ssize_t passthrough_posix_write (int fd, const void* buf, ssize_t counter);


};
}
#endif //PADLL_POSIX_PASSTHROUGH_H
