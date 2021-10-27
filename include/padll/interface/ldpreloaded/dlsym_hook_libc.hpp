/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#ifndef PADLL_DLSYM_HOOK_LIBC_HPP
#define PADLL_DLSYM_HOOK_LIBC_HPP

#include <padll/libraries/libc_headers.hpp>
#include <padll/utils/logging.hpp>
#include <utility>

namespace padll {

class DlsymHookLibc {

private:
    std::mutex m_lock;
    std::string m_lib_name { "libc.so.6" };
    void* m_lib_handle { nullptr };
    std::shared_ptr<Logging> m_logger_ptr {};

    /**
     *  initialize:
     */
    void initialize ()
    {
        // open library and assign pointer to m_lib_handle
        bool open_lib_handle = this->dlopen_library_handle ();

        // validate library pointer
        if (!open_lib_handle) {
            this->m_logger_ptr->log_error ("Error while dlopen'ing " + this->m_lib_name + ".");
            return;
        }
    }

    /**
     * dlopen_library_handle:
     * @param lib_handle
     * @param lib_name
     * @return
     * TODO: (re)validate this function
     */
    bool dlopen_library_handle () {
        std::unique_lock<std::mutex> unique_lock (this->m_lock);
        // Dynamic loading of the libc library (referred to as 'libc.so.6').
        // loads the dynamic shared object (shared library) file named by the null-terminated string
        // filename and returns an opaque "handle" for the loaded object.
        this->m_lib_handle = ::dlopen (this->m_lib_name.data(), RTLD_LAZY);

        // return true if the m_lib_handle is valid, and false otherwise.
        return (this->m_lib_handle != nullptr);
    }

public:

    /**
     * DlsymHookLibc default constructor.
     */
    DlsymHookLibc () :
        m_logger_ptr { std::make_shared<Logging> () }
    {
        // initialize library handle pointer
        this->initialize ();
    }

    /**
     * DlsymHookLibc parameterized constructor.
     * @param library_path
     * TODO: validate move operation of the Log
     */
    DlsymHookLibc (const std::string& library_path, std::shared_ptr<Logging> log_ptr) :
        m_lib_name { library_path },
        m_logger_ptr { std::move (log_ptr) }
    {
        // validate if 'lib' is valid
        if (library_path.empty ()) {
            this->m_logger_ptr->log_error ("Library path not valid.");
            return;
        }

        // initialize library handle pointer
        this->initialize ();
    }

    ~DlsymHookLibc ()
    {
        std::unique_lock <std::mutex> lock (this->m_lock);
        // validate if library handle is valid and close dynamic linking
        if (this->m_lib_handle != nullptr) {
            // close dynamic linking to intercepted library.
            // It decrements the reference count on the dynamically loaded shared object, referred to
            // by handle m_lib_handle. If the reference count drops to zero, then the object is
            // unloaded. All shared objects that were automatically loaded when dlopen () was invoked
            // on the object referred to by handle are recursively closed in the same manner.
            int dlclose_result = ::dlclose (this->m_lib_handle);

            // validate result from dlclose
            if (dlclose_result != 0) {
                this->m_logger_ptr->log_error (
                        "Error while closing dynamic link (" + std::to_string (dlclose_result) + ").");
            }
        }
    }

    /**
     * hook_posix_read:
     * @param read_ptr
     * @param lib_handle
     * @param lib_name
     */
    void hook_posix_read (libc_read_t& read_ptr) {
        // validate function and library handle pointers
        if (!read_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
            ? read_ptr = (libc_read_t) dlsym(this->m_lib_handle, "read")
            : read_ptr = (libc_read_t) dlsym(RTLD_NEXT, "read");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!read_ptr) {
            read_ptr = (libc_read_t) dlsym (this->m_lib_handle, "read");
        }
    }

    /**
     * hook_posix_write:
     * @param write_ptr
     */
    void hook_posix_write (libc_write_t& write_ptr)
    {
        // validate function and library handle pointers
        if (!write_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
            (this->dlopen_library_handle ())
            ? write_ptr = (libc_write_t)dlsym (this->m_lib_handle, "write")
            : write_ptr = (libc_write_t)dlsym (RTLD_NEXT, "write");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!write_ptr) {
            write_ptr = (libc_write_t)dlsym (this->m_lib_handle, "write");
        }
    }

    /**
     * hook_posix_pread:
     * @param pread_ptr
     */
    void hook_posix_pread (libc_pread_t& pread_ptr)
    {
        // validate function and library handle pointers
        if (!pread_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
            (this->dlopen_library_handle ())
            ? pread_ptr = (libc_pread_t)dlsym (this->m_lib_handle, "pread")
            : pread_ptr = (libc_pread_t)dlsym (RTLD_NEXT, "pread");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!pread_ptr) {
            pread_ptr = (libc_pread_t)dlsym (this->m_lib_handle, "pread");
        }
    }

    /**
     * hook_posix_pwrite:
     * @param pwrite_ptr
     */
    void hook_posix_pwrite (libc_pwrite_t& pwrite_ptr)
    {
        // validate function and library handle pointers
        if (!pwrite_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
            (this->dlopen_library_handle ())
            ? pwrite_ptr = (libc_pwrite_t)dlsym (this->m_lib_handle, "pwrite")
            : pwrite_ptr = (libc_pwrite_t)dlsym (RTLD_NEXT, "pwrite");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!pwrite_ptr) {
            pwrite_ptr = (libc_pwrite_t)dlsym (this->m_lib_handle, "pwrite");
        }
    }

    /**
     * hook_posix_pread64:
     * @param pread64_ptr
     */
    #if defined(__USE_LARGEFILE64)
    void hook_posix_pread64 (libc_pread64_t& pread64_ptr)
    {
        // validate function and library handle pointers
        if (!pread64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
            (this->dlopen_library_handle ())
            ? pread64_ptr = (libc_pread64_t)dlsym (this->m_lib_handle, "pread64")
            : pread64_ptr = (libc_pread64_t)dlsym (RTLD_NEXT, "pread64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!pread64_ptr) {
            pread64_ptr = (libc_pread64_t)dlsym (this->m_lib_handle, "pread64");
        }
    }
    #endif

    /**
     * hook_posix_pwrite64:
     * @param pwrite64_ptr
     */
    #if defined(__USE_LARGEFILE64)
    void hook_posix_pwrite64 (libc_pwrite64_t& pwrite64_ptr)
    {
        // validate function and library handle pointers
        if (!pwrite64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
            (this->dlopen_library_handle ())
            ? pwrite64_ptr = (libc_pwrite64_t)dlsym (this->m_lib_handle, "pwrite64")
            : pwrite64_ptr = (libc_pwrite64_t)dlsym (RTLD_NEXT, "pwrite64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!pwrite64_ptr) {
            pwrite64_ptr = (libc_pwrite64_t)dlsym (this->m_lib_handle, "pwrite64");
        }
    }
    #endif

    /**
     * hook_posix_fread:
     * @param fread_ptr
     */
    void hook_posix_fread (libc_fread_t& fread_ptr)
    {
        // validate function and library handle pointers
        if (!fread_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
            (this->dlopen_library_handle ())
            ? fread_ptr = (libc_fread_t)dlsym (this->m_lib_handle, "fread")
            : fread_ptr = (libc_fread_t)dlsym (RTLD_NEXT, "fread");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fread_ptr) {
            fread_ptr = (libc_fread_t)dlsym (this->m_lib_handle, "fread");
        }
    }

    /**
     * hook_posix_fwrite:
     * @param fwrite_ptr
     */
    void hook_posix_fwrite (libc_fwrite_t& fwrite_ptr)
    {
        // validate function and library handle pointers
        if (!fwrite_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
            (this->dlopen_library_handle ())
            ? fwrite_ptr = (libc_fwrite_t)dlsym (this->m_lib_handle, "fwrite")
            : fwrite_ptr = (libc_fwrite_t)dlsym (RTLD_NEXT, "fwrite");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fwrite_ptr) {
            fwrite_ptr = (libc_fwrite_t)dlsym (this->m_lib_handle, "fwrite");
        }
    }

};

}

#endif //PADLL_DLSYM_HOOK_LIBC_HPP
