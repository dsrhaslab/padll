/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#ifndef PADLL_DLSYM_HOOK_LIBC_HPP
#define PADLL_DLSYM_HOOK_LIBC_HPP

#include <mutex>
#include <padll/library_headers/libc_headers.hpp>
#include <padll/options/options.hpp>
#include <padll/utils/log.hpp>
#include <utility>

using namespace padll::headers;
using namespace padll::options;
using namespace padll::utils::log;

namespace padll::interface::ldpreloaded {

/**
 * DlsymHookLibc class.
 * This class implements all libc hooks and logic for LD_PRELOAD libc calls.
 */
class DlsymHookLibc {

private:
    std::mutex m_lock;
    std::string m_lib_name { option_library_name };
    void* m_lib_handle { nullptr };
    std::shared_ptr<Log> m_log { nullptr };

    /**
     *  initialize: initialize the DlSymHook class, by opening the libc library handle; print a
     * logging (error) message if the library was not successfully opened.
     */
    void initialize ()
    {
        // open library and assign pointer to m_lib_handle
        if (!this->dlopen_library_handle ()) {
            this->m_log->log_error ("DlymHook::Error while dlopen'ing "
                + (this->m_lib_name.empty () ? "<undefined lib>" : this->m_lib_name) + ".");
            return;
        }
    }

    /**
     * dlopen_library_handle: open the libc library and store the resulting pointer.
     * @return returns a boolean defining if the dynamic library was successfully opened or not.
     */
    bool dlopen_library_handle ()
    {
        // unique_lock over mutex
        std::unique_lock lock (this->m_lock);
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
    DlsymHookLibc () :
        m_log { std::make_shared<Log> (option_default_enable_debug_level,
            option_default_enable_debug_with_ld_preload,
            std::string { option_default_log_path }) }
    {
        // initialize library handle pointer
        this->initialize ();
    }

    /**
     * DlsymHookLibc parameterized constructor.
     * @param library_path path to the dynamic library to be opened
     * @param log_ptr shared pointer to a logging object
     */
    DlsymHookLibc (const std::string_view& library_path, std::shared_ptr<Log> log_ptr) :
        m_lib_name { library_path },
        m_log { log_ptr }
    {
        // validate if 'lib' is valid
        if (library_path.empty ()) {
            this->m_log->log_error ("Library path not valid.");
            return;
        }

        // initialize library handle pointer
        this->initialize ();
    }

    /**
     * DlsymHookLibc default destructor.
     */
    ~DlsymHookLibc () = default;
    // ~DlsymHookLibc ()
    // {
    //     // unique_lock over mutex
    //     std::unique_lock lock (this->m_lock);
    //     // validate if library handle is valid and close dynamic linking
    //     if (this->m_lib_handle != nullptr) {
    //         // close dynamic linking to intercepted library.
    //         // It decrements the reference count on the dynamically loaded shared object,
    //         referred
    //         // to by handle m_lib_handle. If the reference count drops to zero, then the object
    //         is
    //         // unloaded. All shared objects that were automatically loaded when dlopen () was
    //         // invoked on the object referred to by handle are recursively closed in the same
    //         // manner.
    //         int dlclose_result = ::dlclose (this->m_lib_handle);

    //         // validate result from dlclose
    //         if (dlclose_result != 0) {
    //             std::printf ("Error while closing dynamic link (%d).\n", dlclose_result);
    //         }
    //     }
    // }

    /**
     * hook_posix_read: function to hook libc's read function pointer.
     * @param read_ptr function pointer with the same header as libc's read.
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
     * hook_posix_write: function to hook libc's write function pointer.
     * @param write_ptr function pointer with the same header as libc's write.
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
     * hook_posix_pread: function to hook libc's pread function pointer.
     * @param pread_ptr function pointer with the same header as libc's pread.
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
     * hook_posix_pwrite: function to hook libc's pwrite function pointer.
     * @param pwrite_ptr function pointer with the same header as libc's pwrite.
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
 * hook_posix_pread64: function to hook libc's pread64 function pointer.
 * @param pread64_ptr function pointer with the same header as libc's pread64.
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
 * hook_posix_pwrite64: function to hook libc's pwrite64 function pointer.
 * @param pwrite64_ptr function pointer with the same header as libc's pwrite64.
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
     * hook_posix_mmap: function to hook libc's mmap function pointer.
     * @param mmap_ptr function pointer with the same header as libc's mmap.
     */
    void hook_posix_mmap (libc_mmap_t& mmap_ptr)
    {
        // validate function and library handle pointers
        if (!mmap_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? mmap_ptr = (libc_mmap_t)dlsym (this->m_lib_handle, "mmap")
                : mmap_ptr = (libc_mmap_t)dlsym (RTLD_NEXT, "mmap");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!mmap_ptr) {
            mmap_ptr = (libc_mmap_t)dlsym (this->m_lib_handle, "mmap");
        }
    }

    /**
     * hook_posix_munmap: function to hook libc's munmap function pointer.
     * @param munmap_ptr function pointer with the same header as libc's munmap.
     */
    void hook_posix_munmap (libc_munmap_t& munmap_ptr)
    {
        // validate function and library handle pointers
        if (!munmap_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? munmap_ptr = (libc_munmap_t)dlsym (this->m_lib_handle, "munmap")
                : munmap_ptr = (libc_munmap_t)dlsym (RTLD_NEXT, "munmap");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!munmap_ptr) {
            munmap_ptr = (libc_munmap_t)dlsym (this->m_lib_handle, "munmap");
        }
    }

    /**
     * hook_posix_openvar: function to hook libc's open variadic function pointer.
     * @param open_ptr function pointer with the same header as libc's open variadic.
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
     * hook_posix_open: function to hook libc's open function pointer.
     * @param open_ptr function pointer with the same header as libc's open.
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
     * hook_posix_creat: function to hook libc's creat function pointer.
     * @param creat_ptr function pointer with the same header as libc's creat.
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
     * hook_posix_creat64: function to hook libc's creat64 function pointer.
     * @param creat64_ptr function pointer with the same header as libc's creat64.
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
     * hook_posix_openat_var: function to hook libc's openat variadic function pointer.
     * @param openat_ptr function pointer with the same header as libc's openat variadic.
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
     * hook_posix_openat: function to hook libc's openat function pointer.
     * @param openat_ptr function pointer with the same header as libc's openat.
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
     * hook_posix_open64_variadic: function to hook libc's open64 variadic function pointer.
     * @param open64_ptr function pointer with the same header as libc's open64 variadic.
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
     * hook_posix_open64: function to hook libc's open64 function pointer.
     * @param open64_ptr function pointer with the same header as libc's open64.
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
     * hook_posix_close: function to hook libc's close function pointer.
     * @param close_ptr function pointer with the same header as libc's close.
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
     * hook_posix_sync: function to hook libc's sync function pointer.
     * @param sync_ptr function pointer with the same header as libc's sync.
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
     * hook_posix_statfs: function to hook libc's statfs function pointer.
     * @param statfs_ptr function pointer with the same header as libc's statfs.
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
     * hook_posix_fstatfs: function to hook libc's fstatfs function pointer.
     * @param fstatfs_ptr function pointer with the same header as libc's fstatfs.
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
     * hook_posix_statfs64: function to hook libc's statfs64 function pointer.
     * @param statfs64_ptr function pointer with the same header as libc's statfs64.
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
     * hook_posix_fstatfs64: function to hook libc's fstatfs64 function pointer.
     * @param fstatfs64_ptr function pointer with the same header as libc's fstatfs64.
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
     * hook_posix_unlink: function to hook libc's unlink function pointer.
     * @param unlink_ptr function pointer with the same header as libc's unlink.
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
     * hook_posix_unlinkat: function to hook libc's unlinkat function pointer.
     * @param unlinkat_ptr function pointer with the same header as libc's unlinkat.
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
     * hook_posix_rename: function to hook libc's rename function pointer.
     * @param rename_ptr function pointer with the same header as libc's rename.
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
     * hook_posix_renameat: function to hook libc's renameat function pointer.
     * @param renameat_ptr function pointer with the same header as libc's renameat.
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
     * hook_posix_fopen: function to hook libc's fopen function pointer.
     * @param fopen_ptr function pointer with the same header as libc's fopen.
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
     * hook_posix_fopen64: function to hook libc's fopen64 function pointer.
     * @param fopen64_ptr function pointer with the same header as libc's fopen64.
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
     * hook_posix_fclose: function to hook libc's fclose function pointer.
     * @param fclose_ptr function pointer with the same header as libc's fclose.
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
     * hook_posix_mkdir: function to hook libc's mkdir function pointer.
     * @param mkdir_ptr function pointer with the same header as libc's mkdir.
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
     * hook_posix_mkdirat: function to hook libc's mkdirat function pointer.
     * @param mkdirat_ptr function pointer with the same header as libc's mkdirat.
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
     * hook_posix_rmdir: function to hook libc's rmdir function pointer.
     * @param rmdir_ptr function pointer with the same header as libc's rmdir.
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
     * hook_posix_mknod: function to hook libc's mknod function pointer.
     * @param mknod_ptr function pointer with the same header as libc's mknod.
     */
    void hook_posix_mknod (libc_mknod_t& mknod_ptr)
    {
        // validate function and library handle pointers
        if (!mknod_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? mknod_ptr = (libc_mknod_t)dlsym (this->m_lib_handle, "mknod")
                : mknod_ptr = (libc_mknod_t)dlsym (RTLD_NEXT, "mknod");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!mknod_ptr) {
            mknod_ptr = (libc_mknod_t)dlsym (this->m_lib_handle, "mknod");
        }
    }

    /**
     * hook_posix_mknodat: function to hook libc's mknodat function pointer.
     * @param mknodat_ptr function pointer with the same header as libc's mknodat.
     */
    void hook_posix_mknodat (libc_mknodat_t& mknodat_ptr)
    {
        // validate function and library handle pointers
        if (!mknodat_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? mknodat_ptr = (libc_mknodat_t)dlsym (this->m_lib_handle, "mknodat")
                : mknodat_ptr = (libc_mknodat_t)dlsym (RTLD_NEXT, "mknodat");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!mknodat_ptr) {
            mknodat_ptr = (libc_mknodat_t)dlsym (this->m_lib_handle, "mknodat");
        }
    }

    /**
     * hook_posix_getxattr: function to hook libc's getxattr function pointer.
     * @param getxattr_ptr function pointer with the same header as libc's getxattr.
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
     * hook_posix_lgetxattr: function to hook libc's lgetxattr function pointer.
     * @param lgetxattr_ptr function pointer with the same header as libc's lgetxattr.
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
     * hook_posix_fgetxattr: function to hook libc's fgetxattr function pointer.
     * @param fgetxattr_ptr function pointer with the same header as libc's fgetxattr.
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
     * hook_posix_setxattr: function to hook libc's setxattr function pointer.
     * @param setxattr_ptr function pointer with the same header as libc's setxattr.
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
     * hook_posix_lsetxattr: function to hook libc's lsetxattr function pointer.
     * @param lsetxattr_ptr function pointer with the same header as libc's lsetxattr.
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
     * hook_posix_fsetxattr: function to hook libc's fsetxattr function pointer.
     * @param fsetxattr_ptr function pointer with the same header as libc's fsetxattr.
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
     * hook_posix_listxattr: function to hook libc's listxattr function pointer.
     * @param listxattr_ptr function pointer with the same header as libc's listxattr.
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
     * hook_posix_llistxattr: function to hook libc's llistxattr function pointer.
     * @param llistxattr_ptr function pointer with the same header as libc's llistxattr.
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
     * hook_posix_flistxattr: function to hook libc's flistxattr function pointer.
     * @param flistxattr_ptr function pointer with the same header as libc's flistxattr.
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
     * hook_posix_socket: function to hook libc's socket function pointer.
     * @param socket_ptr function pointer with the same header as libc's socket.
     */
    void hook_posix_socket (libc_socket_t& socket_ptr)
    {
        // validate function and library handle pointers
        if (!socket_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? socket_ptr = (libc_socket_t)dlsym (this->m_lib_handle, "socket")
                : socket_ptr = (libc_socket_t)dlsym (RTLD_NEXT, "socket");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!socket_ptr) {
            socket_ptr = (libc_socket_t)dlsym (this->m_lib_handle, "socket");
        }
    }

    /**
     * hook_posix_fcntl: function to hook libc's fcntl function pointer.
     * @param fcntl_ptr function pointer with the same header as libc's fcntl.
     */
    void hook_posix_fcntl (libc_fcntl_t& fcntl_ptr)
    {
        // validate function and library handle pointers
        if (!fcntl_ptr && !this->m_lib_handle) {
            // open library handle, and assign the operation pointer through m_lib_handle if the
            // open was successful, or through the next operation link.
            (this->dlopen_library_handle ())
                ? fcntl_ptr = (libc_fcntl_t)dlsym (this->m_lib_handle, "fcntl")
                : fcntl_ptr = (libc_fcntl_t)dlsym (RTLD_NEXT, "fcntl");

            // in case the library handle pointer is valid, assign the operation pointer
        } else if (!fcntl_ptr) {
            fcntl_ptr = (libc_fcntl_t)dlsym (this->m_lib_handle, "fcntl");
        }
    }
};

} // namespace padll::interface::ldpreloaded

#endif // PADLL_DLSYM_HOOK_LIBC_HPP
