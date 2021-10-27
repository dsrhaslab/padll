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
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
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
            // open library handle, and assign the operation pointer through m_lib_handle if the open
            // was successful, or through the next operation link.
            (this->dlopen_library_handle ())
            ? fdopen_ptr = (libc_fdopen_t)dlsym (this->m_lib_handle, "fdopen")
            : fdopen_ptr = (libc_fdopen_t)dlsym (RTLD_NEXT, "fdopen");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fdopen_ptr) {
            fdopen_ptr = (libc_fdopen_t)dlsym (this->m_lib_handle, "fdopen");
        }
    }
};

} // namespace padll

#endif // PADLL_DLSYM_HOOK_LIBC_HPP
