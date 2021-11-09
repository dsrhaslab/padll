/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef PADLL_DLSYM_HOOK_LIBC_HPP
#define PADLL_DLSYM_HOOK_LIBC_HPP

#include <padll/libraries/libc_headers.hpp>
#include <padll/utils/logging.hpp>
#include <utility>
#include <padll/utils/options.hpp>

namespace padll {

class DlsymHookLibc {

private:
    std::mutex m_lock;
    std::string m_lib_name { option_library_name };
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
    bool dlopen_library_handle ()
    {
        std::unique_lock<std::mutex> unique_lock (this->m_lock);
        // Dynamic loading of the libc library (referred to as 'libc.so.6').
        // loads the dynamic shared object (shared library) file named by the null-terminated string
        // filename and returns an opaque "handle" for the loaded object.
        this->m_lib_handle = ::dlopen (this->m_lib_name.data (), RTLD_LAZY);

        // return true if the m_lib_handle is valid, and false otherwise.
        return (this->m_lib_handle != nullptr);
    }

public:
    /**
     * DlsymHookLibc default constructor.
     */
    DlsymHookLibc () : m_logger_ptr { std::make_shared<Logging> () }
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
        std::unique_lock<std::mutex> lock (this->m_lock);
        // validate if library handle is valid and close dynamic linking
        if (this->m_lib_handle != nullptr) {
            // close dynamic linking to intercepted library.
            // It decrements the reference count on the dynamically loaded shared object, referred
            // to by handle m_lib_handle. If the reference count drops to zero, then the object is
            // unloaded. All shared objects that were automatically loaded when dlopen () was
            // invoked on the object referred to by handle are recursively closed in the same
            // manner.
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
    void hook_posix_read (libc_read_t& read_ptr)
    {
        // validate function and library handle pointers
        if (!read_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? read_ptr = (libc_read_t)dlsym (this->m_lib_handle, "read")
                : read_ptr = (libc_read_t)dlsym (RTLD_NEXT, "read");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!read_ptr) {
            read_ptr = (libc_read_t)dlsym (this->m_lib_handle, "read");
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
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fwrite_ptr = (libc_fwrite_t)dlsym (this->m_lib_handle, "fwrite")
                : fwrite_ptr = (libc_fwrite_t)dlsym (RTLD_NEXT, "fwrite");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fwrite_ptr) {
            fwrite_ptr = (libc_fwrite_t)dlsym (this->m_lib_handle, "fwrite");
        }
    }

    /**
     * hook_posix_open_var:
     * @param open_ptr
     */
    void hook_posix_open_var (libc_open_variadic_t& open_ptr)
    {
        // validate function and library handle pointers
        if (!open_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? open_ptr = (libc_open_variadic_t)dlsym (this->m_lib_handle, "open")
                : open_ptr = (libc_open_variadic_t)dlsym (RTLD_NEXT, "open");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!open_ptr) {
            open_ptr = (libc_open_variadic_t)dlsym (this->m_lib_handle, "open");
        }
    }

    /**
     * hook_posix_open:
     * @param open_ptr
     */
    void hook_posix_open (libc_open_t& open_ptr)
    {
        // validate function and library handle pointers
        if (!open_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? open_ptr = (libc_open_t)dlsym (this->m_lib_handle, "open")
                : open_ptr = (libc_open_t)dlsym (RTLD_NEXT, "open");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!open_ptr) {
            open_ptr = (libc_open_t)dlsym (this->m_lib_handle, "open");
        }
    }

    /**
     * hook_posix_creat:
     * @param creat_ptr
     */
    void hook_posix_creat (libc_creat_t& creat_ptr)
    {
        // validate function and library handle pointers
        if (!creat_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? creat_ptr = (libc_creat_t)dlsym (this->m_lib_handle, "creat")
                : creat_ptr = (libc_creat_t)dlsym (RTLD_NEXT, "creat");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!creat_ptr) {
            creat_ptr = (libc_creat_t)dlsym (this->m_lib_handle, "creat");
        }
    }

    /**
     * hook_posix_creat64:
     * @param creat64_ptr
     */
    void hook_posix_creat64 (libc_creat64_t& creat64_ptr)
    {
        // validate function and library handle pointers
        if (!creat64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? creat64_ptr = (libc_creat64_t)dlsym (this->m_lib_handle, "creat64")
                : creat64_ptr = (libc_creat64_t)dlsym (RTLD_NEXT, "creat64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!creat64_ptr) {
            creat64_ptr = (libc_creat64_t)dlsym (this->m_lib_handle, "creat64");
        }
    }

    /**
     * hook_posix_openat_var:
     * @param openat_ptr
     */
    void hook_posix_openat_var (libc_openat_variadic_t& openat_ptr)
    {
        // validate function and library handle pointers
        if (!openat_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? openat_ptr = (libc_openat_variadic_t)dlsym (this->m_lib_handle, "openat")
                : openat_ptr = (libc_openat_variadic_t)dlsym (RTLD_NEXT, "openat");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!openat_ptr) {
            openat_ptr = (libc_openat_variadic_t)dlsym (this->m_lib_handle, "openat");
        }
    }

    /**
     * hook_posix_openat:
     * @param openat_ptr
     */
    void hook_posix_openat (libc_openat_t& openat_ptr)
    {
        // validate function and library handle pointers
        if (!openat_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? openat_ptr = (libc_openat_t)dlsym (this->m_lib_handle, "openat")
                : openat_ptr = (libc_openat_t)dlsym (RTLD_NEXT, "openat");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!openat_ptr) {
            openat_ptr = (libc_openat_t)dlsym (this->m_lib_handle, "openat");
        }
    }

    /**
     * hook_posix_open64_variadic:
     * @param open64_ptr
     */
    void hook_posix_open64_variadic (libc_open64_variadic_t& open64_ptr)
    {
        // validate function and library handle pointers
        if (!open64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? open64_ptr = (libc_open64_variadic_t)dlsym (this->m_lib_handle, "open64")
                : open64_ptr = (libc_open64_variadic_t)dlsym (RTLD_NEXT, "open64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!open64_ptr) {
            open64_ptr = (libc_open64_variadic_t)dlsym (this->m_lib_handle, "open64");
        }
    }

    /**
     * hook_posix_open64:
     * @param open64_ptr
     */
    void hook_posix_open64 (libc_open64_t& open64_ptr)
    {
        // validate function and library handle pointers
        if (!open64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? open64_ptr = (libc_open64_t)dlsym (this->m_lib_handle, "open64")
                : open64_ptr = (libc_open64_t)dlsym (RTLD_NEXT, "open64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!open64_ptr) {
            open64_ptr = (libc_open64_t)dlsym (this->m_lib_handle, "open64");
        }
    }

    /**
     * hook_posix_close:
     * @param close_ptr
     */
    void hook_posix_close (libc_close_t& close_ptr)
    {
        // validate function and library handle pointers
        if (!close_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? close_ptr = (libc_close_t)dlsym (this->m_lib_handle, "close")
                : close_ptr = (libc_close_t)dlsym (RTLD_NEXT, "close");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!close_ptr) {
            close_ptr = (libc_close_t)dlsym (this->m_lib_handle, "close");
        }
    }

    /**
     * hook_posix_fsync:
     * @param fsync_ptr
     */
    void hook_posix_fsync (libc_fsync_t& fsync_ptr)
    {
        // validate function and library handle pointers
        if (!fsync_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fsync_ptr = (libc_fsync_t)dlsym (this->m_lib_handle, "fsync")
                : fsync_ptr = (libc_fsync_t)dlsym (RTLD_NEXT, "fsync");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fsync_ptr) {
            fsync_ptr = (libc_fsync_t)dlsym (this->m_lib_handle, "fsync");
        }
    }

    /**
     * hook_posix_fdatasync:
     * @param fdatasync_ptr
     */
    void hook_posix_fdatasync (libc_fdatasync_t& fdatasync_ptr)
    {
        // validate function and library handle pointers
        if (!fdatasync_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fdatasync_ptr = (libc_fdatasync_t)dlsym (this->m_lib_handle, "fdatasync")
                : fdatasync_ptr = (libc_fdatasync_t)dlsym (RTLD_NEXT, "fdatasync");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fdatasync_ptr) {
            fdatasync_ptr = (libc_fdatasync_t)dlsym (this->m_lib_handle, "fdatasync");
        }
    }

    /**
     * hook_posix_sync:
     * @param sync_ptr
     */
    void hook_posix_sync (libc_sync_t& sync_ptr)
    {
        // validate function and library handle pointers
        if (!sync_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? sync_ptr = (libc_sync_t)dlsym (this->m_lib_handle, "sync")
                : sync_ptr = (libc_sync_t)dlsym (RTLD_NEXT, "sync");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!sync_ptr) {
            sync_ptr = (libc_sync_t)dlsym (this->m_lib_handle, "sync");
        }
    }

    /**
     * hook_posix_syncfs:
     * @param syncfs_ptr
     */
    void hook_posix_syncfs (libc_syncfs_t& syncfs_ptr)
    {
        // validate function and library handle pointers
        if (!syncfs_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? syncfs_ptr = (libc_syncfs_t)dlsym (this->m_lib_handle, "syncfs")
                : syncfs_ptr = (libc_syncfs_t)dlsym (RTLD_NEXT, "syncfs");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!syncfs_ptr) {
            syncfs_ptr = (libc_syncfs_t)dlsym (this->m_lib_handle, "syncfs");
        }
    }

    /**
     * hook_posix_truncate:
     * @param truncate_ptr
     */
    void hook_posix_truncate (libc_truncate_t& truncate_ptr)
    {
        // validate function and library handle pointers
        if (!truncate_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? truncate_ptr = (libc_truncate_t)dlsym (this->m_lib_handle, "truncate")
                : truncate_ptr = (libc_truncate_t)dlsym (RTLD_NEXT, "truncate");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!truncate_ptr) {
            truncate_ptr = (libc_truncate_t)dlsym (this->m_lib_handle, "truncate");
        }
    }

    /**
     * ftruncate_ptr
     * @param ftruncate_ptr
     */
    void hook_posix_ftruncate (libc_ftruncate_t& ftruncate_ptr)
    {
        // validate function and library handle pointers
        if (!ftruncate_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? ftruncate_ptr = (libc_ftruncate_t)dlsym (this->m_lib_handle, "ftruncate")
                : ftruncate_ptr = (libc_ftruncate_t)dlsym (RTLD_NEXT, "ftruncate");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!ftruncate_ptr) {
            ftruncate_ptr = (libc_ftruncate_t)dlsym (this->m_lib_handle, "ftruncate");
        }
    }

    /**
     * hook_posix_truncate64:
     * @param truncate64_ptr
     */
    void hook_posix_truncate64 (libc_truncate64_t& truncate64_ptr)
    {
        // validate function and library handle pointers
        if (!truncate64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? truncate64_ptr = (libc_truncate64_t)dlsym (this->m_lib_handle, "truncate64")
                : truncate64_ptr = (libc_truncate64_t)dlsym (RTLD_NEXT, "truncate64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!truncate64_ptr) {
            truncate64_ptr = (libc_truncate64_t)dlsym (this->m_lib_handle, "truncate64");
        }
    }

    /**
     * hook_posix_ftruncate64:
     * @param ftruncate64_ptr
     */
    void hook_posix_ftruncate64 (libc_ftruncate64_t& ftruncate64_ptr)
    {
        // validate function and library handle pointers
        if (!ftruncate64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? ftruncate64_ptr = (libc_ftruncate64_t)dlsym (this->m_lib_handle, "ftruncate64")
                : ftruncate64_ptr = (libc_ftruncate64_t)dlsym (RTLD_NEXT, "ftruncate64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!ftruncate64_ptr) {
            ftruncate64_ptr = (libc_ftruncate64_t)dlsym (this->m_lib_handle, "ftruncate64");
        }
    }

    /**
     * hook_posix_xstat:
     * @param xstat_ptr
     */
    void hook_posix_xstat (libc_xstat_t& xstat_ptr)
    {
        // validate function and library handle pointers
        if (!xstat_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? xstat_ptr = (libc_xstat_t)dlsym (this->m_lib_handle, "__xstat")
                : xstat_ptr = (libc_xstat_t)dlsym (RTLD_NEXT, "__xstat");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!xstat_ptr) {
            xstat_ptr = (libc_xstat_t)dlsym (this->m_lib_handle, "__xstat");
        }
    }

    /**
     * hook_posix_lxstat:
     * @param lxstat_ptr
     */
    void hook_posix_lxstat (libc_lxstat_t& lxstat_ptr)
    {
        // validate function and library handle pointers
        if (!lxstat_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? lxstat_ptr = (libc_lxstat_t)dlsym (this->m_lib_handle, "__lxstat")
                : lxstat_ptr = (libc_lxstat_t)dlsym (RTLD_NEXT, "__lxstat");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!lxstat_ptr) {
            lxstat_ptr = (libc_lxstat_t)dlsym (this->m_lib_handle, "__lxstat");
        }
    }

    /**
     * hook_posix_fxstat:
     * @param fxstat_ptr
     */
    void hook_posix_fxstat (libc_fxstat_t& fxstat_ptr)
    {
        // validate function and library handle pointers
        if (!fxstat_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fxstat_ptr = (libc_fxstat_t)dlsym (this->m_lib_handle, "__fxstat")
                : fxstat_ptr = (libc_fxstat_t)dlsym (RTLD_NEXT, "__fxstat");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fxstat_ptr) {
            fxstat_ptr = (libc_fxstat_t)dlsym (this->m_lib_handle, "__fxstat");
        }
    }

    /**
     * hook_posix_fxstatat:
     * @param fxstatat_ptr
     */
    void hook_posix_fxstatat (libc_fxstatat_t& fxstatat_ptr)
    {
        // validate function and library handle pointers
        if (!fxstatat_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fxstatat_ptr = (libc_fxstatat_t)dlsym (this->m_lib_handle, "__fxstatat")
                : fxstatat_ptr = (libc_fxstatat_t)dlsym (RTLD_NEXT, "__fxstatat");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fxstatat_ptr) {
            fxstatat_ptr = (libc_fxstatat_t)dlsym (this->m_lib_handle, "__fxstatat");
        }
    }

    /**
     * hook_posix_xstat64:
     * @param xstat64_ptr
     */
    void hook_posix_xstat64 (libc_xstat64_t& xstat64_ptr)
    {
        // validate function and library handle pointers
        if (!xstat64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? xstat64_ptr = (libc_xstat64_t)dlsym (this->m_lib_handle, "__xstat64")
                : xstat64_ptr = (libc_xstat64_t)dlsym (RTLD_NEXT, "__xstat64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!xstat64_ptr) {
            xstat64_ptr = (libc_xstat64_t)dlsym (this->m_lib_handle, "__xstat64");
        }
    }

    /**
     * hook_posix_lxstat64:
     * @param lxstat64_ptr
     */
    void hook_posix_lxstat64 (libc_lxstat64_t& lxstat64_ptr)
    {
        // validate function and library handle pointers
        if (!lxstat64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? lxstat64_ptr = (libc_lxstat64_t)dlsym (this->m_lib_handle, "__lxstat64")
                : lxstat64_ptr = (libc_lxstat64_t)dlsym (RTLD_NEXT, "__lxstat64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!lxstat64_ptr) {
            lxstat64_ptr = (libc_lxstat64_t)dlsym (this->m_lib_handle, "__lxstat64");
        }
    }

    /**
     * hook_posix_fxstat64:
     * @param fxstat64_ptr
     */
    void hook_posix_fxstat64 (libc_fxstat64_t& fxstat64_ptr)
    {
        // validate function and library handle pointers
        if (!fxstat64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fxstat64_ptr = (libc_fxstat64_t)dlsym (this->m_lib_handle, "__fxstat64")
                : fxstat64_ptr = (libc_fxstat64_t)dlsym (RTLD_NEXT, "__fxstat64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fxstat64_ptr) {
            fxstat64_ptr = (libc_fxstat64_t)dlsym (this->m_lib_handle, "__fxstat64");
        }
    }

    /**
     * hook_posix_fxstatat64:
     * @param fxstatat64_ptr
     */
    void hook_posix_fxstatat64 (libc_fxstatat64_t& fxstatat64_ptr)
    {
        // validate function and library handle pointers
        if (!fxstatat64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fxstatat64_ptr = (libc_fxstatat64_t)dlsym (this->m_lib_handle, "__fxstatat64")
                : fxstatat64_ptr = (libc_fxstatat64_t)dlsym (RTLD_NEXT, "__fxstatat64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fxstatat64_ptr) {
            fxstatat64_ptr = (libc_fxstatat64_t)dlsym (this->m_lib_handle, "__fxstatat64");
        }
    }

    /**
     * hook_posix_statfs:
     * @param statfs_ptr
     */
    void hook_posix_statfs (libc_statfs_t& statfs_ptr)
    {
        // validate function and library handle pointers
        if (!statfs_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? statfs_ptr = (libc_statfs_t)dlsym (this->m_lib_handle, "statfs")
                : statfs_ptr = (libc_statfs_t)dlsym (RTLD_NEXT, "statfs");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!statfs_ptr) {
            statfs_ptr = (libc_statfs_t)dlsym (this->m_lib_handle, "statfs");
        }
    }

    /**
     * hook_posix_fstatfs:
     * @param fstatfs_ptr
     */
    void hook_posix_fstatfs (libc_fstatfs_t& fstatfs_ptr)
    {
        // validate function and library handle pointers
        if (!fstatfs_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fstatfs_ptr = (libc_fstatfs_t)dlsym (this->m_lib_handle, "fstatfs")
                : fstatfs_ptr = (libc_fstatfs_t)dlsym (RTLD_NEXT, "fstatfs");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fstatfs_ptr) {
            fstatfs_ptr = (libc_fstatfs_t)dlsym (this->m_lib_handle, "fstatfs");
        }
    }

    /**
     * hook_posix_statfs64:
     * @param statfs64_ptr
     */
    void hook_posix_statfs64 (libc_statfs64_t& statfs64_ptr)
    {
        // validate function and library handle pointers
        if (!statfs64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? statfs64_ptr = (libc_statfs64_t)dlsym (this->m_lib_handle, "statfs64")
                : statfs64_ptr = (libc_statfs64_t)dlsym (RTLD_NEXT, "statfs64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!statfs64_ptr) {
            statfs64_ptr = (libc_statfs64_t)dlsym (this->m_lib_handle, "statfs64");
        }
    }

    /**
     * hook_posix_fstatfs64:
     * @param fstatfs64_ptr
     */
    void hook_posix_fstatfs64 (libc_fstatfs64_t& fstatfs64_ptr)
    {
        // validate function and library handle pointers
        if (!fstatfs64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fstatfs64_ptr = (libc_fstatfs64_t)dlsym (this->m_lib_handle, "fstatfs64")
                : fstatfs64_ptr = (libc_fstatfs64_t)dlsym (RTLD_NEXT, "fstatfs64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fstatfs64_ptr) {
            fstatfs64_ptr = (libc_fstatfs64_t)dlsym (this->m_lib_handle, "fstatfs64");
        }
    }

    /**
     * hook_posix_link:
     * @param link_ptr
     */
    void hook_posix_link (libc_link_t& link_ptr)
    {
        // validate function and library handle pointers
        if (!link_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? link_ptr = (libc_link_t)dlsym (this->m_lib_handle, "link")
                : link_ptr = (libc_link_t)dlsym (RTLD_NEXT, "link");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!link_ptr) {
            link_ptr = (libc_link_t)dlsym (this->m_lib_handle, "link");
        }
    }

    /**
     * hook_posix_unlink:
     * @param unlink_ptr
     */
    void hook_posix_unlink (libc_unlink_t& unlink_ptr)
    {
        // validate function and library handle pointers
        if (!unlink_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? unlink_ptr = (libc_unlink_t)dlsym (this->m_lib_handle, "unlink")
                : unlink_ptr = (libc_unlink_t)dlsym (RTLD_NEXT, "unlink");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!unlink_ptr) {
            unlink_ptr = (libc_unlink_t)dlsym (this->m_lib_handle, "unlink");
        }
    }

    /**
     * hook_posix_linkat:
     * @param linkat_ptr
     */
    void hook_posix_linkat (libc_linkat_t& linkat_ptr)
    {
        // validate function and library handle pointers
        if (!linkat_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? linkat_ptr = (libc_linkat_t)dlsym (this->m_lib_handle, "linkat")
                : linkat_ptr = (libc_linkat_t)dlsym (RTLD_NEXT, "linkat");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!linkat_ptr) {
            linkat_ptr = (libc_linkat_t)dlsym (this->m_lib_handle, "linkat");
        }
    }

    /**
     * hook_posix_unlinkat:
     * @param unlinkat_ptr
     */
    void hook_posix_unlinkat (libc_unlinkat_t& unlinkat_ptr)
    {
        // validate function and library handle pointers
        if (!unlinkat_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? unlinkat_ptr = (libc_unlinkat_t)dlsym (this->m_lib_handle, "unlinkat")
                : unlinkat_ptr = (libc_unlinkat_t)dlsym (RTLD_NEXT, "unlinkat");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!unlinkat_ptr) {
            unlinkat_ptr = (libc_unlinkat_t)dlsym (this->m_lib_handle, "unlinkat");
        }
    }

    /**
     * hook_posix_rename:
     * @param rename_ptr
     */
    void hook_posix_rename (libc_rename_t& rename_ptr)
    {
        // validate function and library handle pointers
        if (!rename_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? rename_ptr = (libc_rename_t)dlsym (this->m_lib_handle, "rename")
                : rename_ptr = (libc_rename_t)dlsym (RTLD_NEXT, "rename");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!rename_ptr) {
            rename_ptr = (libc_rename_t)dlsym (this->m_lib_handle, "rename");
        }
    }

    /**
     * hook_posix_renameat:
     * @param renameat_ptr
     */
    void hook_posix_renameat (libc_renameat_t& renameat_ptr)
    {
        // validate function and library handle pointers
        if (!renameat_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? renameat_ptr = (libc_renameat_t)dlsym (this->m_lib_handle, "renameat")
                : renameat_ptr = (libc_renameat_t)dlsym (RTLD_NEXT, "renameat");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!renameat_ptr) {
            renameat_ptr = (libc_renameat_t)dlsym (this->m_lib_handle, "renameat");
        }
    }

    /**
     * hook_posix_symlink:
     * @param symlink_ptr
     */
    void hook_posix_symlink (libc_symlink_t& symlink_ptr)
    {
        // validate function and library handle pointers
        if (!symlink_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? symlink_ptr = (libc_symlink_t)dlsym (this->m_lib_handle, "symlink")
                : symlink_ptr = (libc_symlink_t)dlsym (RTLD_NEXT, "symlink");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!symlink_ptr) {
            symlink_ptr = (libc_symlink_t)dlsym (this->m_lib_handle, "symlink");
        }
    }

    /**
     * hook_posix_symlinkat:
     * @param symlinkat_ptr
     */
    void hook_posix_symlinkat (libc_symlinkat_t& symlinkat_ptr)
    {
        // validate function and library handle pointers
        if (!symlinkat_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? symlinkat_ptr = (libc_symlinkat_t)dlsym (this->m_lib_handle, "symlinkat")
                : symlinkat_ptr = (libc_symlinkat_t)dlsym (RTLD_NEXT, "symlinkat");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!symlinkat_ptr) {
            symlinkat_ptr = (libc_symlinkat_t)dlsym (this->m_lib_handle, "symlinkat");
        }
    }

    /**
     * hook_posix_readlink:
     * @param readlink_ptr
     */
    void hook_posix_readlink (libc_readlink_t& readlink_ptr)
    {
        // validate function and library handle pointers
        if (!readlink_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? readlink_ptr = (libc_readlink_t)dlsym (this->m_lib_handle, "readlink")
                : readlink_ptr = (libc_readlink_t)dlsym (RTLD_NEXT, "readlink");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!readlink_ptr) {
            readlink_ptr = (libc_readlink_t)dlsym (this->m_lib_handle, "readlink");
        }
    }

    /**
     * hook_posix_readlinkat:
     * @param readlinkat_ptr
     */
    void hook_posix_readlinkat (libc_readlinkat_t& readlinkat_ptr)
    {
        // validate function and library handle pointers
        if (!readlinkat_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? readlinkat_ptr = (libc_readlinkat_t)dlsym (this->m_lib_handle, "readlinkat")
                : readlinkat_ptr = (libc_readlinkat_t)dlsym (RTLD_NEXT, "readlinkat");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!readlinkat_ptr) {
            readlinkat_ptr = (libc_readlinkat_t)dlsym (this->m_lib_handle, "readlinkat");
        }
    }

    /**
     * hook_posix_fopen:
     * @param fopen_ptr
     */
    void hook_posix_fopen (libc_fopen_t& fopen_ptr)
    {
        // validate function and library handle pointers
        if (!fopen_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fopen_ptr = (libc_fopen_t)dlsym (this->m_lib_handle, "fopen")
                : fopen_ptr = (libc_fopen_t)dlsym (RTLD_NEXT, "fopen");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fopen_ptr) {
            fopen_ptr = (libc_fopen_t)dlsym (this->m_lib_handle, "fopen");
        }
    }

    /**
     * hook_posix_fopen64:
     * @param fopen64_ptr
     */
    void hook_posix_fopen64 (libc_fopen64_t& fopen64_ptr)
    {
        // validate function and library handle pointers
        if (!fopen64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fopen64_ptr = (libc_fopen64_t)dlsym (this->m_lib_handle, "fopen64")
                : fopen64_ptr = (libc_fopen64_t)dlsym (RTLD_NEXT, "fopen64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fopen64_ptr) {
            fopen64_ptr = (libc_fopen64_t)dlsym (this->m_lib_handle, "fopen64");
        }
    }

    /**
     * hook_posix_fdopen:
     * @param fdopen_ptr
     */
    void hook_posix_fdopen (libc_fdopen_t& fdopen_ptr)
    {
        // validate function and library handle pointers
        if (!fdopen_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fdopen_ptr = (libc_fdopen_t)dlsym (this->m_lib_handle, "fdopen")
                : fdopen_ptr = (libc_fdopen_t)dlsym (RTLD_NEXT, "fdopen");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fdopen_ptr) {
            fdopen_ptr = (libc_fdopen_t)dlsym (this->m_lib_handle, "fdopen");
        }
    }

    /**
     * hook_posix_freopen:
     * @param freopen_ptr
     */
    void hook_posix_freopen (libc_freopen_t& freopen_ptr)
    {
        // validate function and library handle pointers
        if (!freopen_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? freopen_ptr = (libc_freopen_t)dlsym (this->m_lib_handle, "freopen")
                : freopen_ptr = (libc_freopen_t)dlsym (RTLD_NEXT, "freopen");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!freopen_ptr) {
            freopen_ptr = (libc_freopen_t)dlsym (this->m_lib_handle, "freopen");
        }
    }

    /**
     * hook_posix_freopen64:
     * @param freopen64_ptr
     */
    void hook_posix_freopen64 (libc_freopen64_t& freopen64_ptr)
    {
        // validate function and library handle pointers
        if (!freopen64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? freopen64_ptr = (libc_freopen64_t)dlsym (this->m_lib_handle, "freopen64")
                : freopen64_ptr = (libc_freopen64_t)dlsym (RTLD_NEXT, "freopen64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!freopen64_ptr) {
            freopen64_ptr = (libc_freopen64_t)dlsym (this->m_lib_handle, "freopen64");
        }
    }

    /**
     * hook_posix_fclose:
     * @param fclose_ptr
     */
    void hook_posix_fclose (libc_fclose_t& fclose_ptr)
    {
        // validate function and library handle pointers
        if (!fclose_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fclose_ptr = (libc_fclose_t)dlsym (this->m_lib_handle, "fclose")
                : fclose_ptr = (libc_fclose_t)dlsym (RTLD_NEXT, "fclose");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fclose_ptr) {
            fclose_ptr = (libc_fclose_t)dlsym (this->m_lib_handle, "fclose");
        }
    }

    /**
     * hook_posix_fflush:
     * @param fflush_ptr
     */
    void hook_posix_fflush (libc_fflush_t& fflush_ptr)
    {
        // validate function and library handle pointers
        if (!fflush_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fflush_ptr = (libc_fflush_t)dlsym (this->m_lib_handle, "fflush")
                : fflush_ptr = (libc_fflush_t)dlsym (RTLD_NEXT, "fflush");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fflush_ptr) {
            fflush_ptr = (libc_fflush_t)dlsym (this->m_lib_handle, "fflush");
        }
    }

    /**
     * hook_posix_access:
     * @param access_ptr
     */
    void hook_posix_access (libc_access_t& access_ptr)
    {
        // validate function and library handle pointers
        if (!access_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? access_ptr = (libc_access_t)dlsym (this->m_lib_handle, "access")
                : access_ptr = (libc_access_t)dlsym (RTLD_NEXT, "access");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!access_ptr) {
            access_ptr = (libc_access_t)dlsym (this->m_lib_handle, "access");
        }
    }

    /**
     * hook_posix_faccessat:
     * @param faccessat_ptr
     */
    void hook_posix_faccessat (libc_faccessat_t& faccessat_ptr)
    {
        // validate function and library handle pointers
        if (!faccessat_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? faccessat_ptr = (libc_faccessat_t)dlsym (this->m_lib_handle, "faccessat")
                : faccessat_ptr = (libc_faccessat_t)dlsym (RTLD_NEXT, "faccessat");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!faccessat_ptr) {
            faccessat_ptr = (libc_faccessat_t)dlsym (this->m_lib_handle, "faccessat");
        }
    }

    /**
     * hook_posix_lseek:
     * @param lseek_ptr
     */
    void hook_posix_lseek (libc_lseek_t& lseek_ptr)
    {
        // validate function and library handle pointers
        if (!lseek_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? lseek_ptr = (libc_lseek_t)dlsym (this->m_lib_handle, "lseek")
                : lseek_ptr = (libc_lseek_t)dlsym (RTLD_NEXT, "lseek");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!lseek_ptr) {
            lseek_ptr = (libc_lseek_t)dlsym (this->m_lib_handle, "lseek");
        }
    }

    /**
     * hook_posix_fseek:
     * @param fseek_ptr
     */
    void hook_posix_fseek (libc_fseek_t& fseek_ptr)
    {
        // validate function and library handle pointers
        if (!fseek_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fseek_ptr = (libc_fseek_t)dlsym (this->m_lib_handle, "fseek")
                : fseek_ptr = (libc_fseek_t)dlsym (RTLD_NEXT, "fseek");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fseek_ptr) {
            fseek_ptr = (libc_fseek_t)dlsym (this->m_lib_handle, "fseek");
        }
    }

    /**
     * hook_posix_ftell:
     * @param ftell_ptr
     */
    void hook_posix_ftell (libc_ftell_t& ftell_ptr)
    {
        // validate function and library handle pointers
        if (!ftell_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? ftell_ptr = (libc_ftell_t)dlsym (this->m_lib_handle, "ftell")
                : ftell_ptr = (libc_ftell_t)dlsym (RTLD_NEXT, "ftell");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!ftell_ptr) {
            ftell_ptr = (libc_ftell_t)dlsym (this->m_lib_handle, "ftell");
        }
    }

    /**
     * hook_posix_lseek64:
     * @param lseek64_ptr
     */
    void hook_posix_lseek64 (libc_lseek64_t& lseek64_ptr)
    {
        // validate function and library handle pointers
        if (!lseek64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? lseek64_ptr = (libc_lseek64_t)dlsym (this->m_lib_handle, "lseek64")
                : lseek64_ptr = (libc_lseek64_t)dlsym (RTLD_NEXT, "lseek64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!lseek64_ptr) {
            lseek64_ptr = (libc_lseek64_t)dlsym (this->m_lib_handle, "lseek64");
        }
    }

    /**
     * hook_posix_fseeko64:
     * @param fseeko64_ptr
     */
    void hook_posix_fseeko64 (libc_fseeko64_t& fseeko64_ptr)
    {
        // validate function and library handle pointers
        if (!fseeko64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fseeko64_ptr = (libc_fseeko64_t)dlsym (this->m_lib_handle, "fseeko64")
                : fseeko64_ptr = (libc_fseeko64_t)dlsym (RTLD_NEXT, "fseeko64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fseeko64_ptr) {
            fseeko64_ptr = (libc_fseeko64_t)dlsym (this->m_lib_handle, "fseeko64");
        }
    }

    /**
     * hook_posix_ftello64:
     * @param ftello64_ptr
     */
    void hook_posix_ftello64 (libc_ftello64_t& ftello64_ptr)
    {
        // validate function and library handle pointers
        if (!ftello64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? ftello64_ptr = (libc_ftello64_t)dlsym (this->m_lib_handle, "ftello64")
                : ftello64_ptr = (libc_ftello64_t)dlsym (RTLD_NEXT, "ftello64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!ftello64_ptr) {
            ftello64_ptr = (libc_ftello64_t)dlsym (this->m_lib_handle, "ftello64");
        }
    }

    /**
     * hook_posix_mkdir:
     * @param mkdir_ptr
     */
    void hook_posix_mkdir (libc_mkdir_t& mkdir_ptr)
    {
        // validate function and library handle pointers
        if (!mkdir_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? mkdir_ptr = (libc_mkdir_t)dlsym (this->m_lib_handle, "mkdir")
                : mkdir_ptr = (libc_mkdir_t)dlsym (RTLD_NEXT, "mkdir");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!mkdir_ptr) {
            mkdir_ptr = (libc_mkdir_t)dlsym (this->m_lib_handle, "mkdir");
        }
    }

    /**
     * hook_posix_mkdirat:
     * @param mkdirat_ptr
     */
    void hook_posix_mkdirat (libc_mkdirat_t& mkdirat_ptr)
    {
        // validate function and library handle pointers
        if (!mkdirat_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? mkdirat_ptr = (libc_mkdirat_t)dlsym (this->m_lib_handle, "mkdirat")
                : mkdirat_ptr = (libc_mkdirat_t)dlsym (RTLD_NEXT, "mkdirat");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!mkdirat_ptr) {
            mkdirat_ptr = (libc_mkdirat_t)dlsym (this->m_lib_handle, "mkdirat");
        }
    }

    /**
     * hook_posix_readdir:
     * @param readdir_ptr
     */
    void hook_posix_readdir (libc_readdir_t& readdir_ptr)
    {
        // validate function and library handle pointers
        if (!readdir_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? readdir_ptr = (libc_readdir_t)dlsym (this->m_lib_handle, "readdir")
                : readdir_ptr = (libc_readdir_t)dlsym (RTLD_NEXT, "readdir");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!readdir_ptr) {
            readdir_ptr = (libc_readdir_t)dlsym (this->m_lib_handle, "readdir");
        }
    }

    /**
     * hook_posix_readdir64:
     * @param readdir64_ptr
     */
    void hook_posix_readdir64 (libc_readdir64_t& readdir64_ptr)
    {
        // validate function and library handle pointers
        if (!readdir64_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? readdir64_ptr = (libc_readdir64_t)dlsym (this->m_lib_handle, "readdir64")
                : readdir64_ptr = (libc_readdir64_t)dlsym (RTLD_NEXT, "readdir64");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!readdir64_ptr) {
            readdir64_ptr = (libc_readdir64_t)dlsym (this->m_lib_handle, "readdir64");
        }
    }

    /**
     * hook_posix_opendir:
     * @param opendir_ptr
     */
    void hook_posix_opendir (libc_opendir_t& opendir_ptr)
    {
        // validate function and library handle pointers
        if (!opendir_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? opendir_ptr = (libc_opendir_t)dlsym (this->m_lib_handle, "opendir")
                : opendir_ptr = (libc_opendir_t)dlsym (RTLD_NEXT, "opendir");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!opendir_ptr) {
            opendir_ptr = (libc_opendir_t)dlsym (this->m_lib_handle, "opendir");
        }
    }

    /**
     * hook_posix_fdopendir:
     * @param fdopendir_ptr
     */
    void hook_posix_fdopendir (libc_fdopendir_t& fdopendir_ptr)
    {
        // validate function and library handle pointers
        if (!fdopendir_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fdopendir_ptr = (libc_fdopendir_t)dlsym (this->m_lib_handle, "fdopendir")
                : fdopendir_ptr = (libc_fdopendir_t)dlsym (RTLD_NEXT, "fdopendir");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fdopendir_ptr) {
            fdopendir_ptr = (libc_fdopendir_t)dlsym (this->m_lib_handle, "fdopendir");
        }
    }

    /**
     * hook_posix_closedir:
     * @param closedir_ptr
     */
    void hook_posix_closedir (libc_closedir_t& closedir_ptr)
    {
        // validate function and library handle pointers
        if (!closedir_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? closedir_ptr = (libc_closedir_t)dlsym (this->m_lib_handle, "closedir")
                : closedir_ptr = (libc_closedir_t)dlsym (RTLD_NEXT, "closedir");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!closedir_ptr) {
            closedir_ptr = (libc_closedir_t)dlsym (this->m_lib_handle, "closedir");
        }
    }

    /**
     * hook_posix_rmdir:
     * @param rmdir_ptr
     */
    void hook_posix_rmdir (libc_rmdir_t& rmdir_ptr)
    {
        // validate function and library handle pointers
        if (!rmdir_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? rmdir_ptr = (libc_rmdir_t)dlsym (this->m_lib_handle, "rmdir")
                : rmdir_ptr = (libc_rmdir_t)dlsym (RTLD_NEXT, "rmdir");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!rmdir_ptr) {
            rmdir_ptr = (libc_rmdir_t)dlsym (this->m_lib_handle, "rmdir");
        }
    }

    /**
     * hook_posix_dirfd:
     * @param dirfd_ptr
     */
    void hook_posix_dirfd (libc_dirfd_t& dirfd_ptr)
    {
        // validate function and library handle pointers
        if (!dirfd_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? dirfd_ptr = (libc_dirfd_t)dlsym (this->m_lib_handle, "dirfd")
                : dirfd_ptr = (libc_dirfd_t)dlsym (RTLD_NEXT, "dirfd");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!dirfd_ptr) {
            dirfd_ptr = (libc_dirfd_t)dlsym (this->m_lib_handle, "dirfd");
        }
    }

    /**
     * hook_posix_getxattr:
     * @param getxattr_ptr
     */
    void hook_posix_getxattr (libc_getxattr_t& getxattr_ptr)
    {
        // validate function and library handle pointers
        if (!getxattr_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? getxattr_ptr = (libc_getxattr_t)dlsym (this->m_lib_handle, "getxattr")
                : getxattr_ptr = (libc_getxattr_t)dlsym (RTLD_NEXT, "getxattr");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!getxattr_ptr) {
            getxattr_ptr = (libc_getxattr_t)dlsym (this->m_lib_handle, "getxattr");
        }
    }

    /**
     * hook_posix_lgetxattr:
     * @param lgetxattr_ptr
     */
    void hook_posix_lgetxattr (libc_lgetxattr_t& lgetxattr_ptr)
    {
        // validate function and library handle pointers
        if (!lgetxattr_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? lgetxattr_ptr = (libc_lgetxattr_t)dlsym (this->m_lib_handle, "lgetxattr")
                : lgetxattr_ptr = (libc_lgetxattr_t)dlsym (RTLD_NEXT, "lgetxattr");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!lgetxattr_ptr) {
            lgetxattr_ptr = (libc_lgetxattr_t)dlsym (this->m_lib_handle, "lgetxattr");
        }
    }

    /**
     * hook_posix_fgetxattr:
     * @param fgetxattr_ptr
     */
    void hook_posix_fgetxattr (libc_fgetxattr_t& fgetxattr_ptr)
    {
        // validate function and library handle pointers
        if (!fgetxattr_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fgetxattr_ptr = (libc_fgetxattr_t)dlsym (this->m_lib_handle, "fgetxattr")
                : fgetxattr_ptr = (libc_fgetxattr_t)dlsym (RTLD_NEXT, "fgetxattr");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fgetxattr_ptr) {
            fgetxattr_ptr = (libc_fgetxattr_t)dlsym (this->m_lib_handle, "fgetxattr");
        }
    }

    /**
     * hook_posix_setxattr:
     * @param setxattr_ptr
     */
    void hook_posix_setxattr (libc_setxattr_t& setxattr_ptr)
    {
        // validate function and library handle pointers
        if (!setxattr_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? setxattr_ptr = (libc_setxattr_t)dlsym (this->m_lib_handle, "setxattr")
                : setxattr_ptr = (libc_setxattr_t)dlsym (RTLD_NEXT, "setxattr");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!setxattr_ptr) {
            setxattr_ptr = (libc_setxattr_t)dlsym (this->m_lib_handle, "setxattr");
        }
    }

    /**
     * hook_posix_lsetxattr:
     * @param lsetxattr_ptr
     */
    void hook_posix_lsetxattr (libc_lsetxattr_t& lsetxattr_ptr)
    {
        // validate function and library handle pointers
        if (!lsetxattr_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? lsetxattr_ptr = (libc_lsetxattr_t)dlsym (this->m_lib_handle, "lsetxattr")
                : lsetxattr_ptr = (libc_lsetxattr_t)dlsym (RTLD_NEXT, "lsetxattr");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!lsetxattr_ptr) {
            lsetxattr_ptr = (libc_lsetxattr_t)dlsym (this->m_lib_handle, "lsetxattr");
        }
    }

    /**
     * hook_posix_fsetxattr:
     * @param fsetxattr_ptr
     */
    void hook_posix_fsetxattr (libc_fsetxattr_t& fsetxattr_ptr)
    {
        // validate function and library handle pointers
        if (!fsetxattr_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fsetxattr_ptr = (libc_fsetxattr_t)dlsym (this->m_lib_handle, "fsetxattr")
                : fsetxattr_ptr = (libc_fsetxattr_t)dlsym (RTLD_NEXT, "fsetxattr");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fsetxattr_ptr) {
            fsetxattr_ptr = (libc_fsetxattr_t)dlsym (this->m_lib_handle, "fsetxattr");
        }
    }

    /**
     * hook_posix_listxattr:
     * @param listxattr_ptr
     */
    void hook_posix_listxattr (libc_listxattr_t& listxattr_ptr)
    {
        // validate function and library handle pointers
        if (!listxattr_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? listxattr_ptr = (libc_listxattr_t)dlsym (this->m_lib_handle, "listxattr")
                : listxattr_ptr = (libc_listxattr_t)dlsym (RTLD_NEXT, "listxattr");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!listxattr_ptr) {
            listxattr_ptr = (libc_listxattr_t)dlsym (this->m_lib_handle, "listxattr");
        }
    }

    /**
     * hook_posix_llistxattr:
     * @param llistxattr_ptr
     */
    void hook_posix_llistxattr (libc_llistxattr_t& llistxattr_ptr)
    {
        // validate function and library handle pointers
        if (!llistxattr_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? llistxattr_ptr = (libc_llistxattr_t)dlsym (this->m_lib_handle, "llistxattr")
                : llistxattr_ptr = (libc_llistxattr_t)dlsym (RTLD_NEXT, "llistxattr");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!llistxattr_ptr) {
            llistxattr_ptr = (libc_llistxattr_t)dlsym (this->m_lib_handle, "llistxattr");
        }
    }

    /**
     * hook_posix_flistxattr:
     * @param flistxattr_ptr
     */
    void hook_posix_flistxattr (libc_flistxattr_t& flistxattr_ptr)
    {
        // validate function and library handle pointers
        if (!flistxattr_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? flistxattr_ptr = (libc_flistxattr_t)dlsym (this->m_lib_handle, "flistxattr")
                : flistxattr_ptr = (libc_flistxattr_t)dlsym (RTLD_NEXT, "flistxattr");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!flistxattr_ptr) {
            flistxattr_ptr = (libc_flistxattr_t)dlsym (this->m_lib_handle, "flistxattr");
        }
    }

    /**
     * hook_posix_removexattr:
     * @param removexattr_ptr
     */
    void hook_posix_removexattr (libc_removexattr_t& removexattr_ptr)
    {
        // validate function and library handle pointers
        if (!removexattr_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? removexattr_ptr = (libc_removexattr_t)dlsym (this->m_lib_handle, "removexattr")
                : removexattr_ptr = (libc_removexattr_t)dlsym (RTLD_NEXT, "removexattr");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!removexattr_ptr) {
            removexattr_ptr = (libc_removexattr_t)dlsym (this->m_lib_handle, "removexattr");
        }
    }

    /**
     * hook_posix_lremovexattr:
     * @param lremovexattr_ptr
     */
    void hook_posix_lremovexattr (libc_lremovexattr_t& lremovexattr_ptr)
    {
        // validate function and library handle pointers
        if (!lremovexattr_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? lremovexattr_ptr = (libc_lremovexattr_t)dlsym (this->m_lib_handle, "lremovexattr")
                : lremovexattr_ptr = (libc_lremovexattr_t)dlsym (RTLD_NEXT, "lremovexattr");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!lremovexattr_ptr) {
            lremovexattr_ptr = (libc_lremovexattr_t)dlsym (this->m_lib_handle, "lremovexattr");
        }
    }

    /**
     * hook_posix_fremovexattr:
     * @param fremovexattr_ptr
     */
    void hook_posix_fremovexattr (libc_fremovexattr_t& fremovexattr_ptr)
    {
        // validate function and library handle pointers
        if (!fremovexattr_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fremovexattr_ptr = (libc_fremovexattr_t)dlsym (this->m_lib_handle, "fremovexattr")
                : fremovexattr_ptr = (libc_fremovexattr_t)dlsym (RTLD_NEXT, "fremovexattr");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fremovexattr_ptr) {
            fremovexattr_ptr = (libc_fremovexattr_t)dlsym (this->m_lib_handle, "fremovexattr");
        }
    }

    /**
     * hook_posix_chmod:
     * @param chmod_ptr
     */
    void hook_posix_chmod (libc_chmod_t& chmod_ptr)
    {
        // validate function and library handle pointers
        if (!chmod_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? chmod_ptr = (libc_chmod_t)dlsym (this->m_lib_handle, "chmod")
                : chmod_ptr = (libc_chmod_t)dlsym (RTLD_NEXT, "chmod");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!chmod_ptr) {
            chmod_ptr = (libc_chmod_t)dlsym (this->m_lib_handle, "chmod");
        }
    }

    /**
     * hook_posix_fchmod:
     * @param fchmod_ptr
     */
    void hook_posix_fchmod (libc_fchmod_t& fchmod_ptr)
    {
        // validate function and library handle pointers
        if (!fchmod_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fchmod_ptr = (libc_fchmod_t)dlsym (this->m_lib_handle, "fchmod")
                : fchmod_ptr = (libc_fchmod_t)dlsym (RTLD_NEXT, "fchmod");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fchmod_ptr) {
            fchmod_ptr = (libc_fchmod_t)dlsym (this->m_lib_handle, "fchmod");
        }
    }

    /**
     * hook_posix_fchmodat:
     * @param fchmodat_ptr
     */
    void hook_posix_fchmodat (libc_fchmodat_t& fchmodat_ptr)
    {
        // validate function and library handle pointers
        if (!fchmodat_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fchmodat_ptr = (libc_fchmodat_t)dlsym (this->m_lib_handle, "fchmodat")
                : fchmodat_ptr = (libc_fchmodat_t)dlsym (RTLD_NEXT, "fchmodat");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fchmodat_ptr) {
            fchmodat_ptr = (libc_fchmodat_t)dlsym (this->m_lib_handle, "fchmodat");
        }
    }

    /**
     * hook_posix_chown:
     * @param chown_ptr
     */
    void hook_posix_chown (libc_chown_t& chown_ptr)
    {
        // validate function and library handle pointers
        if (!chown_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? chown_ptr = (libc_chown_t)dlsym (this->m_lib_handle, "chown")
                : chown_ptr = (libc_chown_t)dlsym (RTLD_NEXT, "chown");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!chown_ptr) {
            chown_ptr = (libc_chown_t)dlsym (this->m_lib_handle, "chown");
        }
    }

    /**
     * hook_posix_lchown:
     * @param lchown_ptr
     */
    void hook_posix_lchown (libc_lchown_t& lchown_ptr)
    {
        // validate function and library handle pointers
        if (!lchown_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? lchown_ptr = (libc_chown_t)dlsym (this->m_lib_handle, "lchown")
                : lchown_ptr = (libc_chown_t)dlsym (RTLD_NEXT, "lchown");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!lchown_ptr) {
            lchown_ptr = (libc_chown_t)dlsym (this->m_lib_handle, "lchown");
        }
    }

    /**
     * hook_posix_fchown:
     * @param fchown_ptr
     */
    void hook_posix_fchown (libc_fchown_t& fchown_ptr)
    {
        // validate function and library handle pointers
        if (!fchown_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fchown_ptr = (libc_fchown_t)dlsym (this->m_lib_handle, "fchown")
                : fchown_ptr = (libc_fchown_t)dlsym (RTLD_NEXT, "fchown");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fchown_ptr) {
            fchown_ptr = (libc_fchown_t)dlsym (this->m_lib_handle, "fchown");
        }
    }

    /**
     * hook_posix_fchownat:
     * @param fchownat_ptr
     */
    void hook_posix_fchownat (libc_fchownat_t fchownat_ptr)
    {
        // validate function and library handle pointers
        if (!fchownat_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fchownat_ptr = (libc_fchownat_t)dlsym (this->m_lib_handle, "fchownat")
                : fchownat_ptr = (libc_fchownat_t)dlsym (RTLD_NEXT, "fchownat");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fchownat_ptr) {
            fchownat_ptr = (libc_fchownat_t)dlsym (this->m_lib_handle, "fchownat");
        }
    }
};

} // namespace padll

#endif // PADLL_DLSYM_HOOK_LIBC_HPP
