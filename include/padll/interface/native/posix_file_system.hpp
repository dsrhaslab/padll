/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#ifndef PADLL_POSIX_FILE_SYSTEM_H
#define PADLL_POSIX_FILE_SYSTEM_H

#include <cstdarg>
#include <cstring>
#include <padll/configurations/libc_calls.hpp>
#include <padll/interface/ldpreloaded/ld_preloaded_posix.hpp>
#include <padll/interface/passthrough/posix_passthrough.hpp>
#include <padll/utils/log.hpp>
#include <thread>

namespace ldp = padll::interface::ldpreloaded;
namespace ptr = padll::interface::passthrough;
namespace opt = padll::options;
namespace lgr = padll::utils::log;

/**
 * s_logger: static logging object.
 */
const std::shared_ptr<lgr::Log> m_logger_ptr { std::make_shared<lgr::Log> (
    opt::option_default_enable_debug_level,
    opt::option_default_enable_debug_with_ld_preload,
    std::string { opt::option_default_log_path }) };

/**
 * PosixPassthrough file system object.
 */
ptr::PosixPassthrough m_posix_passthrough { std::string (option_library_name), m_logger_ptr };

/**
 * Shared atomic object to define if the LdPreloadedPosix object is fully initialized (including
 * syscall pointers, mountpoint table,  logging, ...)
 */
std::shared_ptr<std::atomic<bool>> m_ldp_loaded { std::make_shared<std::atomic<bool>> (false) };

/**
 * LdPreloaded file system object.
 */
ldp::LdPreloadedPosix m_ld_preloaded_posix { std::string (option_library_name),
    option_default_statistic_collection,
    m_logger_ptr,
    m_ldp_loaded };

/**
 * init_method: constructor of the PosixFileSystem.
 * This method is executed before the program executes its main (). Under shared objects, this
 * occurs at load time.
 */
static __attribute__ ((constructor)) void init_method ()
{
    std::printf ("PosixFileSystem constructor (%d, %d)\n", ::getpid (), getppid ());
}

/**
 * destroy_method: destructor of the PosixFileSystem.
 * This method will execute once the main process (main ()) has returned or exit() is called.
 */
static __attribute__ ((destructor)) void destroy_method ()
{
    std::printf ("PosixFileSystem destructor (%d, %d)\n", ::getpid (), getppid ());
}

/**
 * read: intercept POSIX read. Operation will be submitted to passthrough or enforced (rate limited)
 * depending on the PosixDataCalls configurations.
 * @param fd
 * @param buf
 * @param size
 * @return
 */
extern "C" ssize_t read (int fd, void* buf, size_t size)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__,
        std::string_view { std::to_string (fd) },
        std::string_view { std::to_string (size) });
#endif

    return (posix_data_calls.padll_intercept_read && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_read (fd, buf, size)
        : m_posix_passthrough.passthrough_posix_read (fd, buf, size);
}

/**
 * write: intercept POSIX write. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the PosixDataCalls configurations.
 * @param fd
 * @param buf
 * @param size
 * @return
 */
extern "C" ssize_t write (int fd, const void* buf, size_t size)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__,
        std::string_view { std::to_string (fd) },
        std::string_view { std::to_string (size) });
#endif

    return (posix_data_calls.padll_intercept_write && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_write (fd, buf, size)
        : m_posix_passthrough.passthrough_posix_write (fd, buf, size);
}

/**
 * pread: intercept POSIX pread. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the PosixDataCalls configurations.
 * @param fd
 * @param buf
 * @param size
 * @param offset
 * @return
 */
extern "C" ssize_t pread (int fd, void* buf, size_t size, off_t offset)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__,
        std::string_view { std::to_string (fd) },
        std::string_view { std::to_string (size) });
#endif

    return (posix_data_calls.padll_intercept_pread && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_pread (fd, buf, size, offset)
        : m_posix_passthrough.passthrough_posix_pread (fd, buf, size, offset);
}

/**
 * pwrite: intercept POSIX pwrite. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the PosixDataCalls configurations.
 * @param fd
 * @param buf
 * @param size
 * @param offset
 * @return
 */
extern "C" ssize_t pwrite (int fd, const void* buf, size_t size, off_t offset)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__,
        std::string_view { std::to_string (fd) },
        std::string_view { std::to_string (size) });
#endif

    return (posix_data_calls.padll_intercept_pwrite && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_pwrite (fd, buf, size, offset)
        : m_posix_passthrough.passthrough_posix_pwrite (fd, buf, size, offset);
}

/**
 * pread64: intercept POSIX pread64. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the PosixDataCalls configurations.
 * @param fd
 * @param buf
 * @param size
 * @param offset
 * @return
 */
#if defined(__USE_LARGEFILE64)
extern "C" ssize_t pread64 (int fd, void* buf, size_t size, off64_t offset)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__,
        std::string_view { std::to_string (fd) },
        std::string_view { std::to_string (size) });
#endif

    return (posix_data_calls.padll_intercept_pread64 && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_pread64 (fd, buf, size, offset)
        : m_posix_passthrough.passthrough_posix_pread64 (fd, buf, size, offset);
}
#endif

/**
 * pwrite64: intercept POSIX pwrite64. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the PosixDataCalls configurations.
 * @param fd
 * @param buf
 * @param size
 * @param offset
 * @return
 */
#if defined(__USE_LARGEFILE64)
extern "C" ssize_t pwrite64 (int fd, const void* buf, size_t size, off64_t offset)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__,
        std::string_view { std::to_string (fd) },
        std::string_view { std::to_string (size) });
#endif

    return (posix_data_calls.padll_intercept_pwrite64 && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_pwrite64 (fd, buf, size, offset)
        : m_posix_passthrough.passthrough_posix_pwrite64 (fd, buf, size, offset);
}
#endif

/**
 * mmap: intercept POSIX mmap. Operation will be submitted to passthrough or enforced (rate limited)
 * depending on the PosixDataCalls configurations.
 * @param addr
 * @param length
 * @param prot
 * @param flags
 * @param fd
 * @param offset
 * @return
 */
extern "C" void* mmap (void* addr, size_t length, int prot, int flags, int fd, off_t offset)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, std::string_view { std::to_string (fd) });
#endif

    return (posix_data_calls.padll_intercept_mmap && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_mmap (addr, length, prot, flags, fd, offset)
        : m_posix_passthrough.passthrough_posix_mmap (addr, length, prot, flags, fd, offset);
}

/**
 * munmap: intercept POSIX munmap. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the PosixDataCalls configurations.
 * @param addr
 * @param length
 * @return
 */
extern "C" int munmap (void* addr, size_t length)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, "?");
#endif

    return (posix_data_calls.padll_intercept_munmap && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_munmap (addr, length)
        : m_posix_passthrough.passthrough_posix_munmap (addr, length);
}

/**
 * open: intercept POSIX open. Operation will be submitted to passthrough or enforced (rate limited)
 * depending on the MetadataDataCalls configurations.
 * @param path
 * @param flags
 * @param ...
 * @return
 */
extern "C" int open (const char* path, int flags, ...)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, path);
#endif

    if (flags & O_CREAT) {
        va_list args;

        va_start (args, flags);
        auto mode = static_cast<mode_t> (va_arg (args, int));
        va_end (args);

        return (posix_metadata_calls.padll_intercept_open_var && m_ldp_loaded->load ())
            ? m_ld_preloaded_posix.ld_preloaded_posix_open (path, flags, mode)
            : m_posix_passthrough.passthrough_posix_open (path, flags, mode);
    } else {
        return (posix_metadata_calls.padll_intercept_open && m_ldp_loaded->load ())
            ? m_ld_preloaded_posix.ld_preloaded_posix_open (path, flags)
            : m_posix_passthrough.passthrough_posix_open (path, flags);
    }
}

/**
 * creat: intercept POSIX creat. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the MetadataDataCalls configurations.
 * @param path
 * @param mode
 * @return
 */
extern "C" int creat (const char* path, mode_t mode)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, path);
#endif

    return (posix_metadata_calls.padll_intercept_creat && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_creat (path, mode)
        : m_posix_passthrough.passthrough_posix_creat (path, mode);
}

/**
 * creat64: intercept POSIX creat64. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the MetadataDataCalls configurations.
 * @param path
 * @param mode
 * @return
 */
extern "C" int creat64 (const char* path, mode_t mode)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, path);
#endif

    return (posix_metadata_calls.padll_intercept_creat64 && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_creat64 (path, mode)
        : m_posix_passthrough.passthrough_posix_creat64 (path, mode);
}

/**
 * openat: intercept POSIX openat. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the MetadataDataCalls configurations.
 * @param dirfd
 * @param path
 * @param flags
 * @param ...
 * @return
 */
extern "C" int openat (int dirfd, const char* path, int flags, ...)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__,
        std::string_view { std::to_string (dirfd) },
        path);
#endif

    if (flags & O_CREAT) {
        va_list args;

        va_start (args, flags);
        auto mode = static_cast<mode_t> (va_arg (args, int));
        va_end (args);

        return (posix_metadata_calls.padll_intercept_openat_var && m_ldp_loaded->load ())
            ? m_ld_preloaded_posix.ld_preloaded_posix_openat (dirfd, path, flags, mode)
            : m_posix_passthrough.passthrough_posix_openat (dirfd, path, flags, mode);
    } else {
        return (posix_metadata_calls.padll_intercept_openat && m_ldp_loaded->load ())
            ? m_ld_preloaded_posix.ld_preloaded_posix_openat (dirfd, path, flags)
            : m_posix_passthrough.passthrough_posix_openat (dirfd, path, flags);
    }
}

/**
 * open64: intercept POSIX open64. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the MetadataDataCalls configurations.
 * @param path
 * @param flags
 * @param ...
 * @return
 */
extern "C" int open64 (const char* path, int flags, ...)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, path);
#endif

    if (flags & O_CREAT) {
        va_list args;

        va_start (args, flags);
        auto mode = static_cast<mode_t> (va_arg (args, int));
        va_end (args);

        return (posix_metadata_calls.padll_intercept_open64_var && m_ldp_loaded->load ())
            ? m_ld_preloaded_posix.ld_preloaded_posix_open64 (path, flags, mode)
            : m_posix_passthrough.passthrough_posix_open64 (path, flags, mode);
    } else {
        return (posix_metadata_calls.padll_intercept_open64 && m_ldp_loaded->load ())
            ? m_ld_preloaded_posix.ld_preloaded_posix_open64 (path, flags)
            : m_posix_passthrough.passthrough_posix_open64 (path, flags);
    }
}

/**
 * close: intercept POSIX close. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the MetadataDataCalls configurations.
 * @param fd
 * @return
 */
extern "C" int close (int fd)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, std::string_view { std::to_string (fd) });
#endif

    return (posix_metadata_calls.padll_intercept_close && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_close (fd)
        : m_posix_passthrough.passthrough_posix_close (fd);
}

/**
 * sync: intercept POSIX sync. Operation will be submitted to passthrough or enforced (rate limited)
 * depending on the MetadataDataCalls configurations.
 */
extern "C" void sync ()
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, "?");
#endif

    return (posix_metadata_calls.padll_intercept_sync && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_sync ()
        : m_posix_passthrough.passthrough_posix_sync ();
}

/**
 * statfs: intercept POSIX statfs. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the MetadataDataCalls configurations.
 * @param path
 * @param buf
 * @return
 */
extern "C" int statfs (const char* path, struct statfs* buf)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, path);
#endif

    return (posix_metadata_calls.padll_intercept_statfs && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_statfs (path, buf)
        : m_posix_passthrough.passthrough_posix_statfs (path, buf);
}

/**
 * fstatfs: intercept POSIX fstatfs. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the MetadataDataCalls configurations.
 * @param fd
 * @param buf
 * @return
 */
extern "C" int fstatfs (int fd, struct statfs* buf)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, std::string_view { std::to_string (fd) });
#endif

    return (posix_metadata_calls.padll_intercept_fstatfs && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_fstatfs (fd, buf)
        : m_posix_passthrough.passthrough_posix_fstatfs (fd, buf);
}

/**
 * statfs64: intercept POSIX statfs64. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the MetadataDataCalls configurations.
 * @param path
 * @param buf
 * @return
 */
extern "C" int statfs64 (const char* path, struct statfs64* buf)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, path);
#endif

    return (posix_metadata_calls.padll_intercept_statfs64 && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_statfs64 (path, buf)
        : m_posix_passthrough.passthrough_posix_statfs64 (path, buf);
}

/**
 * fstatfs64: intercept POSIX fstatfs64. Operation will be submitted to passthrough or enforced
 * (rate limited) depending on the MetadataDataCalls configurations.
 * @param fd
 * @param buf
 * @return
 */
extern "C" int fstatfs64 (int fd, struct statfs64* buf)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, std::string_view { std::to_string (fd) });
#endif

    return (posix_metadata_calls.padll_intercept_fstatfs64 && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_fstatfs64 (fd, buf)
        : m_posix_passthrough.passthrough_posix_fstatfs64 (fd, buf);
}

/**
 * unlink: intercept POSIX unlink. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the MetadataDataCalls configurations.
 * @param path
 * @return
 */
extern "C" int unlink (const char* path)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, path);
#endif

    return (posix_metadata_calls.padll_intercept_unlink && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_unlink (path)
        : m_posix_passthrough.passthrough_posix_unlink (path);
}

/**
 * unlinkat: intercept POSIX unlinkat. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the MetadataDataCalls configurations.
 * @param dirfd
 * @param pathname
 * @param flags
 * @return
 */
extern "C" int unlinkat (int dirfd, const char* pathname, int flags)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__,
        std::string_view { std::to_string (dirfd) },
        pathname);
#endif

    return (posix_metadata_calls.padll_intercept_unlinkat && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_unlinkat (dirfd, pathname, flags)
        : m_posix_passthrough.passthrough_posix_unlinkat (dirfd, pathname, flags);
}

/**
 * rename: intercept POSIX rename. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the MetadataDataCalls configurations.
 *  https://man7.org/linux/man-pages/man2/rename.2.html
 * @param old_path
 * @param new_path
 * @return
 */
extern "C" int rename (const char* old_path, const char* new_path)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, old_path, new_path);
#endif

    return (posix_metadata_calls.padll_intercept_rename && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_rename (old_path, new_path)
        : m_posix_passthrough.passthrough_posix_rename (old_path, new_path);
}

/**
 * renameat: intercept POSIX renameat. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the MetadataDataCalls configurations.
 *  https://man7.org/linux/man-pages/man2/renameat.2.html
 * @param olddirfd
 * @param old_path
 * @param newdirfd
 * @param new_path
 * @return
 */
extern "C" int renameat (int olddirfd, const char* old_path, int newdirfd, const char* new_path)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, old_path, new_path);
#endif

    return (posix_metadata_calls.padll_intercept_renameat && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_renameat (olddirfd, old_path, newdirfd, new_path)
        : m_posix_passthrough.passthrough_posix_renameat (olddirfd, old_path, newdirfd, new_path);
}

/**
 * fopen: intercept POSIX fopen. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the MetadataDataCalls configurations.
 * @param pathname
 * @param mode
 * @return
 */
extern "C" FILE* fopen (const char* pathname, const char* mode)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, pathname);
#endif

    return (posix_metadata_calls.padll_intercept_fopen && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_fopen (pathname, mode)
        : m_posix_passthrough.passthrough_posix_fopen (pathname, mode);
}

/**
 * fopen64: intercept POSIX fopen64. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the MetadataDataCalls configurations.
 * @param pathname
 * @param mode
 * @return
 */
extern "C" FILE* fopen64 (const char* pathname, const char* mode)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, pathname);
#endif

    return (posix_metadata_calls.padll_intercept_fopen64 && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_fopen64 (pathname, mode)
        : m_posix_passthrough.passthrough_posix_fopen64 (pathname, mode);
}

/**
 * fclose: intercept POSIX fclose. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the MetadataDataCalls configurations.
 * @param stream
 * @return
 */
extern "C" int fclose (FILE* stream)
{
    return (posix_metadata_calls.padll_intercept_fclose && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_fclose (stream)
        : m_posix_passthrough.passthrough_posix_fclose (stream);
}

/**
 * mkdir: intercept POSIX mkdir. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the DirectoryCalls configurations.
 * @param path
 * @param mode
 * @return
 */
extern "C" int mkdir (const char* path, mode_t mode)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, path);
#endif

    return (posix_directory_calls.padll_intercept_mkdir && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_mkdir (path, mode)
        : m_posix_passthrough.passthrough_posix_mkdir (path, mode);
}

/**
 * mkdirat: intercept POSIX mkdirat. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the DirectoryCalls configurations.
 * @param dirfd
 * @param path
 * @param mode
 * @return
 */
extern "C" int mkdirat (int dirfd, const char* path, mode_t mode)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__,
        std::string_view { std::to_string (dirfd) },
        path);
#endif

    return (posix_directory_calls.padll_intercept_mkdirat && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_mkdirat (dirfd, path, mode)
        : m_posix_passthrough.passthrough_posix_mkdirat (dirfd, path, mode);
}

/**
 * rmdir: intercept POSIX rmdir. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the DirectoryCalls configurations.
 * @param path
 * @return
 */
extern "C" int rmdir (const char* path)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, path);
#endif

    return (posix_directory_calls.padll_intercept_rmdir && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_rmdir (path)
        : m_posix_passthrough.passthrough_posix_rmdir (path);
}

/**
 * mknod: intercept POSIX mknod. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the DirectoryCalls configurations.
 * @param path
 * @param mode
 * @param dev
 * @return
 */
extern "C" int mknod (const char* path, mode_t mode, dev_t dev)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, path);
#endif

    return (posix_directory_calls.padll_intercept_mknod && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_mknod (path, mode, dev)
        : m_posix_passthrough.passthrough_posix_mknod (path, mode, dev);
}

/**
 * mknodat: intercept POSIX mknodat. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the DirectoryCalls configurations.
 * @param dirfd
 * @param path
 * @param mode
 * @param dev
 * @return
 */
extern "C" int mknodat (int dirfd, const char* path, mode_t mode, dev_t dev)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__,
        std::string_view { std::to_string (dirfd) },
        path);
#endif

    return (posix_directory_calls.padll_intercept_mknodat && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_mknodat (dirfd, path, mode, dev)
        : m_posix_passthrough.passthrough_posix_mknodat (dirfd, path, mode, dev);
}

/**
 * getxattr: intercept POSIX getxattr. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the ExtendedAttributesCalls configurations.
 * @param path
 * @param name
 * @param value
 * @param size
 * @return
 */
#ifdef __linux__
extern "C" ssize_t getxattr (const char* path, const char* name, void* value, size_t size)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, path, name);
#endif

    return (posix_extended_attributes_calls.padll_intercept_getxattr && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_getxattr (path, name, value, size)
        : m_posix_passthrough.passthrough_posix_getxattr (path, name, value, size);
}
#endif

/**
 * lgetxattr: intercept POSIX lgetxattr. Operation will be submitted to passthrough or enforced
 * (rate limited) depending on the ExtendedAttributesCalls configurations.
 * @param path
 * @param name
 * @param value
 * @param size
 * @return
 */
extern "C" ssize_t lgetxattr (const char* path, const char* name, void* value, size_t size)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, path, name);
#endif

    return (posix_extended_attributes_calls.padll_intercept_lgetxattr && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_lgetxattr (path, name, value, size)
        : m_posix_passthrough.passthrough_posix_lgetxattr (path, name, value, size);
}

/**
 * fgetxattr: intercept POSIX fgetxattr. Operation will be submitted to passthrough or enforced
 * (rate limited) depending on the ExtendedAttributesCalls configurations.
 * @param fd
 * @param name
 * @param value
 * @param size
 * @return
 */
#ifdef __linux__
extern "C" ssize_t fgetxattr (int fd, const char* name, void* value, size_t size)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__,
        std::string_view { std::to_string (fd) },
        name);
#endif

    return (posix_extended_attributes_calls.padll_intercept_fgetxattr && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_fgetxattr (fd, name, value, size)
        : m_posix_passthrough.passthrough_posix_fgetxattr (fd, name, value, size);
}
#endif

/**
 * setxattr: intercept POSIX setxattr. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the ExtendedAttributesCalls configurations.
 * @param path
 * @param name
 * @param value
 * @param size
 * @param flags
 * @return
 */
#ifdef __linux__
extern "C" int
setxattr (const char* path, const char* name, const void* value, size_t size, int flags)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, path, name);
#endif

    return (posix_extended_attributes_calls.padll_intercept_setxattr && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_setxattr (path, name, value, size, flags)
        : m_posix_passthrough.passthrough_posix_setxattr (path, name, value, size, flags);
}
#endif

/**
 * lsetxattr: intercept POSIX lsetxattr. Operation will be submitted to passthrough or enforced
 * (rate limited) depending on the ExtendedAttributesCalls configurations.
 * @param path
 * @param name
 * @param value
 * @param size
 * @param flags
 * @return
 */
extern "C" int
lsetxattr (const char* path, const char* name, const void* value, size_t size, int flags)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, path, name);
#endif

    return (posix_extended_attributes_calls.padll_intercept_lsetxattr && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_lsetxattr (path, name, value, size, flags)
        : m_posix_passthrough.passthrough_posix_lsetxattr (path, name, value, size, flags);
}

/**
 * fsetxattr: intercept POSIX fsetxattr. Operation will be submitted to passthrough or enforced
 * (rate limited) depending on the ExtendedAttributesCalls configurations.
 * @param fd
 * @param name
 * @param value
 * @param size
 * @param flags
 * @return
 */
#ifdef __linux__
extern "C" int fsetxattr (int fd, const char* name, const void* value, size_t size, int flags)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__,
        std::string_view { std::to_string (fd) },
        name);
#endif

    return (posix_extended_attributes_calls.padll_intercept_fsetxattr && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_fsetxattr (fd, name, value, size, flags)
        : m_posix_passthrough.passthrough_posix_fsetxattr (fd, name, value, size, flags);
}
#endif

/**
 * listxattr: intercept POSIX listxattr. Operation will be submitted to passthrough or enforced
 * (rate limited) depending on the ExtendedAttributesCalls configurations.
 * @param path
 * @param list
 * @param size
 * @return
 */
#ifdef __linux__
extern "C" ssize_t listxattr (const char* path, char* list, size_t size)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, path);
#endif

    return (posix_extended_attributes_calls.padll_intercept_listxattr && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_listxattr (path, list, size)
        : m_posix_passthrough.passthrough_posix_listxattr (path, list, size);
}
#endif

/**
 * llistxattr: intercept POSIX llistxattr. Operation will be submitted to passthrough or enforced
 * (rate limited) depending on the ExtendedAttributesCalls configurations.
 * @param path
 * @param list
 * @param size
 * @return
 */
extern "C" ssize_t llistxattr (const char* path, char* list, size_t size)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, path);
#endif

    return (posix_extended_attributes_calls.padll_intercept_llistxattr && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_llistxattr (path, list, size)
        : m_posix_passthrough.passthrough_posix_llistxattr (path, list, size);
}

/**
 * flistxattr: intercept POSIX flistxattr. Operation will be submitted to passthrough or enforced
 * (rate limited) depending on the ExtendedAttributesCalls configurations.
 * @param fd
 * @param list
 * @param size
 * @return
 */
#ifdef __linux__
extern "C" ssize_t flistxattr (int fd, char* list, size_t size)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, std::string_view { std::to_string (fd) });
#endif

    return (posix_extended_attributes_calls.padll_intercept_flistxattr && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_flistxattr (fd, list, size)
        : m_posix_passthrough.passthrough_posix_flistxattr (fd, list, size);
}
#endif

/**
 * socket: intercept POSIX socket. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the SpecialCalls configurations.
 * @param domain
 * @param type
 * @param protocol
 * @return
 */
extern "C" int socket (int domain, int type, int protocol)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__, "?");
#endif

    return (posix_special_calls.padll_intercept_socket && m_ldp_loaded->load ())
        ? m_ld_preloaded_posix.ld_preloaded_posix_socket (domain, type, protocol)
        : m_posix_passthrough.passthrough_posix_socket (domain, type, protocol);
}

/**
 * fcntl: intercept POSIX fcntl. Operation will be submitted to passthrough or enforced (rate
 * limited) depending on the SpecialCalls configurations.
 * @param fd
 * @param cmd
 */
extern "C" int fcntl (int fd, int cmd, ...)
{
// detailed logging message
#if OPTION_DETAILED_LOGGING
    m_logger_ptr->create_routine_log_message (__func__,
        std::string_view { std::to_string (fd) },
        std::string_view { std::to_string (cmd) });
#endif

    va_list ap;
    void* arg;
    va_start (ap, cmd);
    arg = va_arg (ap, void*);
    va_end (ap);

    // submit fcntl call to ld_preloaded_posix
    int result = m_ld_preloaded_posix.ld_preloaded_posix_fcntl (fd, cmd, arg);

    return result;
}

#endif // PADLL_POSIX_FILE_SYSTEM_H
