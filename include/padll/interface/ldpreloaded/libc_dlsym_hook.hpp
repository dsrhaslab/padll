/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#ifndef PADLL_LIBC_DLSYM_HOOK_HPP
#define PADLL_LIBC_DLSYM_HOOK_HPP

#include <padll/libraries/libc_headers.hpp>

namespace padll {

class LibcDlsymHook {

private:
    // dlopen_library_handle call. (...)
    bool dlopen_library_handle (void* lib_handle, std::string lib_name) {
        //std::unique_lock<std::mutex> unique_lock (this->m_lock);
        // Dynamic loading of the libc library (referred to as 'libc.so.6').
        // loads the dynamic shared object (shared library) file named by the null-terminated string
        // filename and returns an opaque "handle" for the loaded object.
        lib_handle = ::dlopen(lib_name.data(), RTLD_LAZY);

        // return true if the m_lib_handle is valid, and false otherwise.
        return (lib_handle != nullptr);
    }

public:

    void hook_posix_read (libc_read_t& read_ptr, void *lib_handle, std::string lib_name) {
        // validate function and library handle pointers
        if (!read_ptr && !lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
            (this->dlopen_library_handle (lib_handle, lib_name))
            ? read_ptr = (libc_read_t) dlsym(lib_handle, "read")
            : read_ptr = (libc_read_t) dlsym(RTLD_NEXT, "read");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!read_ptr) {
            read_ptr = (libc_read_t) dlsym (lib_handle, "read");
        }

    }
};

}

#endif //PADLL_LIBC_DLSYM_HOOK_HPP
