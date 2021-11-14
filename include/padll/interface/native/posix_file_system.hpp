/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef PADLL_POSIX_FILE_SYSTEM_H
#define PADLL_POSIX_FILE_SYSTEM_H

#include <cstdarg>
#include <cstring>
#include <padll/configurations/libc_calls.hpp>
#include <padll/interface/ldpreloaded/ld_preloaded_posix.hpp>
#include <padll/interface/passthrough/posix_passthrough.hpp>
#include <padll/utils/logging.hpp>
#include <thread>

namespace ld = padll::interface::ldpreloaded;
namespace pass = padll::interface::passthrough;
namespace log = padll::utils::logging;
namespace opt = padll::options;

/**
 * Main logging object.
 */
std::shared_ptr<log::Logging> m_logger_ptr { std::make_shared<log::Logging> (
    opt::option_default_enable_debug_level,
    opt::option_default_enable_debug_with_ld_preload,
    std::string (opt::option_default_log_path)) };

/**
 * LdPreloaded file system object.
 */
ld::LdPreloadedPosix m_ld_preloaded_posix { m_logger_ptr };

/**
 * PosixPassthrough file system object.
 */
pass::PosixPassthrough m_posix_passthrough { m_logger_ptr };

/**
 * init_method: constructor of the PosixFileSystem.
 * This method is executed before the program executes its main (). Under shared objects, this
 * occurs at load time.
 * The method needs to use printf instead of std::cout due to a static initialization order problem.
 * (https://stackoverflow.com/questions/16746166/using-cout-in-constructor-gives-segmentation-fault)
 */
static __attribute__ ((constructor)) void init_method ()
{
    std::printf ("PosixFileSystem constructor\n");
    std::this_thread::sleep_for (std::chrono::seconds (1));
}

/**
 * destroy_method: destructor of the PosixFileSystem.
 * This method will execute once the main process (main ()) has returned or exit() is called.
 */
static __attribute__ ((destructor)) void destroy_method ()
{
    std::printf ("PosixFileSystem destructor\n");
    // _exit(EXIT_SUCCESS);
}

/**
 * read:
 * @param fd
 * @param buf
 * @param size
 * @return
 */
extern "C" ssize_t read (int fd, void* buf, size_t size)
{
    std::printf ("passei pelo read header\n");
    return (posix_data_calls.padll_intercept_read)
        ? m_ld_preloaded_posix.ld_preloaded_posix_read (fd, buf, size)
        : m_posix_passthrough.passthrough_posix_read (fd, buf, size);
}

/**
 * write:
 * @param fd
 * @param buf
 * @param size
 * @return
 */
extern "C" ssize_t write (int fd, const void* buf, size_t size)
{
    std::printf ("passei pelo write header\n");
    return (posix_data_calls.padll_intercept_write)
        ? m_ld_preloaded_posix.ld_preloaded_posix_write (fd, buf, size)
        : m_posix_passthrough.passthrough_posix_write (fd, buf, size);
}

/**
 * pread:
 * @param fd
 * @param buf
 * @param size
 * @param offset
 * @return
 */
extern "C" ssize_t pread (int fd, void* buf, size_t size, off_t offset)
{
    return (posix_data_calls.padll_intercept_pread)
        ? m_ld_preloaded_posix.ld_preloaded_posix_pread (fd, buf, size, offset)
        : m_posix_passthrough.passthrough_posix_pread (fd, buf, size, offset);
}

/**
 * pwrite:
 * @param fd
 * @param buf
 * @param size
 * @param offset
 * @return
 */
extern "C" ssize_t pwrite (int fd, const void* buf, size_t size, off_t offset)
{
    return (posix_data_calls.padll_intercept_pwrite)
        ? m_ld_preloaded_posix.ld_preloaded_posix_pwrite (fd, buf, size, offset)
        : m_posix_passthrough.passthrough_posix_pwrite (fd, buf, size, offset);
}

/**
 * pread64:
 * @param fd
 * @param buf
 * @param size
 * @param offset
 * @return
 */
#if defined(__USE_LARGEFILE64)
extern "C" ssize_t pread64 (int fd, void* buf, size_t size, off64_t offset)
{
    return (posix_data_calls.padll_intercept_pread64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_pread64 (fd, buf, size, offset)
        : m_posix_passthrough.passthrough_posix_pread64 (fd, buf, size, offset);
}
#endif

/**
 * pwrite64:
 * @param fd
 * @param buf
 * @param size
 * @param offset
 * @return
 */
#if defined(__USE_LARGEFILE64)
extern "C" ssize_t pwrite64 (int fd, const void* buf, size_t size, off64_t offset)
{
    return (posix_data_calls.padll_intercept_pwrite64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_pwrite64 (fd, buf, size, offset)
        : m_posix_passthrough.passthrough_posix_pwrite64 (fd, buf, size, offset);
}
#endif

/**
 * fread:
 * @param ptr
 * @param size
 * @param nmemb
 * @param stream
 * @return
 */
extern "C" size_t fread (void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    return (posix_data_calls.padll_intercept_fread)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fread (ptr, size, nmemb, stream)
        : m_posix_passthrough.passthrough_posix_fread (ptr, size, nmemb, stream);
}

/**
 * fwrite:
 * Note: Attention: this method will (as well as fflush) will through a segmentation fault when
 *  using the Logging library (even in the initialization, constructors, ...).
 * @param ptr
 * @param size
 * @param nmemb
 * @param stream
 * @return
 */
extern "C" size_t fwrite (const void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    return (posix_data_calls.padll_intercept_fwrite)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fwrite (ptr, size, nmemb, stream)
        : m_posix_passthrough.passthrough_posix_fwrite (ptr, size, nmemb, stream);
}

/**
 * open:
 * @param path
 * @param flags
 * @param ...
 * @return
 */
extern "C" int open (const char* path, int flags, ...)
{
    if (flags & O_CREAT) {
        va_list args;

        va_start (args, flags);
        mode_t mode = va_arg (args, int);
        va_end (args);

        return (posix_metadata_calls.paddl_intercept_open_var)
            ? m_ld_preloaded_posix.ld_preloaded_posix_open (path, flags, mode)
            : m_posix_passthrough.passthrough_posix_open (path, flags, mode);
    } else {
        return (posix_metadata_calls.paddl_intercept_open)
            ? m_ld_preloaded_posix.ld_preloaded_posix_open (path, flags)
            : m_posix_passthrough.passthrough_posix_open (path, flags);
    }
}

/**
 * creat:
 * @param path
 * @param mode
 * @return
 */
extern "C" int creat (const char* path, mode_t mode)
{
    return (posix_metadata_calls.paddl_intercept_creat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_creat (path, mode)
        : m_posix_passthrough.passthrough_posix_creat (path, mode);
}

/**
 * creat64:
 * @param path
 * @param mode
 * @return
 */
extern "C" int creat64 (const char* path, mode_t mode)
{
    return (posix_metadata_calls.paddl_intercept_creat64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_creat64 (path, mode)
        : m_posix_passthrough.passthrough_posix_creat64 (path, mode);
}

/**
 * openat:
 * @param dirfd
 * @param path
 * @param flags
 * @param ...
 * @return
 */
extern "C" int openat (int dirfd, const char* path, int flags, ...)
{
    if (flags & O_CREAT) {
        va_list args;

        va_start (args, flags);
        mode_t mode = va_arg (args, int);
        va_end (args);

        return (posix_metadata_calls.paddl_intercept_openat_var)
            ? m_ld_preloaded_posix.ld_preloaded_posix_openat (dirfd, path, flags, mode)
            : m_posix_passthrough.passthrough_posix_openat (dirfd, path, flags, mode);
    } else {
        return (posix_metadata_calls.paddl_intercept_openat)
            ? m_ld_preloaded_posix.ld_preloaded_posix_openat (dirfd, path, flags)
            : m_posix_passthrough.passthrough_posix_openat (dirfd, path, flags);
    }
}

/**
 * open64:
 * @param path
 * @param flags
 * @param ...
 * @return
 */
extern "C" int open64 (const char* path, int flags, ...)
{
    if (flags & O_CREAT) {
        va_list args;

        va_start (args, flags);
        mode_t mode = va_arg (args, int);
        va_end (args);

        return (posix_metadata_calls.paddl_intercept_open64_var)
            ? m_ld_preloaded_posix.ld_preloaded_posix_open64 (path, flags, mode)
            : m_posix_passthrough.passthrough_posix_open64 (path, flags, mode);
    } else {
        return (posix_metadata_calls.paddl_intercept_open64)
            ? m_ld_preloaded_posix.ld_preloaded_posix_open64 (path, flags)
            : m_posix_passthrough.passthrough_posix_open64 (path, flags);
    }
}

/**
 * close:
 * @param fd
 * @return
 */
extern "C" int close (int fd)
{
    return (posix_metadata_calls.paddl_intercept_close)
        ? m_ld_preloaded_posix.ld_preloaded_posix_close (fd)
        : m_posix_passthrough.passthrough_posix_close (fd);
}

/**
 * fsync:
 * @param fd
 * @return
 */
extern "C" int fsync (int fd)
{
    return (posix_metadata_calls.paddl_intercept_fsync)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fsync (fd)
        : m_posix_passthrough.passthrough_posix_fsync (fd);
}

/**
 * fdatasync:
 * @param fd
 * @return
 */
extern "C" int fdatasync (int fd)
{
    return (posix_metadata_calls.paddl_intercept_fdatasync)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fdatasync (fd)
        : m_posix_passthrough.passthrough_posix_fdatasync (fd);
}

/**
 * sync:
 */
extern "C" void sync ()
{
    return (posix_metadata_calls.paddl_intercept_sync)
        ? m_ld_preloaded_posix.ld_preloaded_posix_sync ()
        : m_posix_passthrough.passthrough_posix_sync ();
}

/**
 * syncfs:
 * @param fd
 */
extern "C" int syncfs (int fd)
{
    return (posix_metadata_calls.paddl_intercept_syncfs)
        ? m_ld_preloaded_posix.ld_preloaded_posix_syncfs (fd)
        : m_posix_passthrough.passthrough_posix_syncfs (fd);
}

/**
 * truncate:
 * @param path
 * @param length
 * @return
 */
extern "C" int truncate (const char* path, off_t length)
{
    return (posix_metadata_calls.paddl_intercept_truncate)
        ? m_ld_preloaded_posix.ld_preloaded_posix_truncate (path, length)
        : m_posix_passthrough.passthrough_posix_truncate (path, length);
}

/**
 * ftruncate:
 * @param fd
 * @param length
 * @return
 */
extern "C" int ftruncate (int fd, off_t length)
{
    return (posix_metadata_calls.paddl_intercept_ftruncate)
        ? m_ld_preloaded_posix.ld_preloaded_posix_ftruncate (fd, length)
        : m_posix_passthrough.passthrough_posix_ftruncate (fd, length);
}

/**
 * truncate64:
 * @param path
 * @param length
 * @return
 */
extern "C" int truncate64 (const char* path, off_t length)
{
    return (posix_metadata_calls.paddl_intercept_truncate64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_truncate64 (path, length)
        : m_posix_passthrough.passthrough_posix_truncate64 (path, length);
}

/**
 * ftruncate64:
 * @param fd
 * @param length
 * @return
 */
extern "C" int ftruncate64 (int fd, off_t length)
{
    return (posix_metadata_calls.paddl_intercept_ftruncate64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_ftruncate64 (fd, length)
        : m_posix_passthrough.passthrough_posix_ftruncate64 (fd, length);
}

/**
 * __xstat:
 *  This method respects to the stat call.
 * @param version
 * @param path
 * @param statbuf
 * @return
 */
extern "C" int __xstat (int version, const char* path, struct stat* statbuf)
{
    return (posix_metadata_calls.paddl_intercept_xstat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_xstat (version, path, statbuf)
        : m_posix_passthrough.passthrough_posix_xstat (version, path, statbuf);
}

/**
 * __lxstat:
 *  This method respects to the lstat call.
 * @param version
 * @param path
 * @param statbuf
 * @return
 */
extern "C" int __lxstat (int version, const char* path, struct stat* statbuf)
{
    return (posix_metadata_calls.paddl_intercept_lxstat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_lxstat (version, path, statbuf)
        : m_posix_passthrough.passthrough_posix_lxstat (version, path, statbuf);
}

/**
 * __fxstat:
 *  This method respects to the fstat call.
 * @param version
 * @param fd
 * @param statbuf
 * @return
 */
extern "C" int __fxstat (int version, int fd, struct stat* statbuf)
{
    return (posix_metadata_calls.paddl_intercept_fxstat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fxstat (version, fd, statbuf)
        : m_posix_passthrough.passthrough_posix_fxstat (version, fd, statbuf);
}

/**
 * fxstatat:
 *  This method respects to the fstatat call.
 * @param version
 * @param dirfd
 * @param path
 * @param statbuf
 * @param flags
 * @return
 */
extern "C" int
__fxstatat (int version, int dirfd, const char* path, struct stat* statbuf, int flags)
{
    return (posix_metadata_calls.paddl_intercept_fxstatat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fxstatat (version, dirfd, path, statbuf, flags)
        : m_posix_passthrough.passthrough_posix_fxstatat (version, dirfd, path, statbuf, flags);
}

/**
 * __xstat64:
 *  This method respects to the stat64 call.
 * @param version
 * @param path
 * @param statbuf
 * @return
 */
extern "C" int __xstat64 (int version, const char* path, struct stat64* statbuf)
{
    return (posix_metadata_calls.paddl_intercept_xstat64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_xstat64 (version, path, statbuf)
        : m_posix_passthrough.passthrough_posix_xstat64 (version, path, statbuf);
}

/**
 * __lxstat64:
 *  This method respects to the lstat64 call.
 * @param version
 * @param path
 * @param statbuf
 * @return
 */
extern "C" int __lxstat64 (int version, const char* path, struct stat64* statbuf)
{
    return (posix_metadata_calls.paddl_intercept_lxstat64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_lxstat64 (version, path, statbuf)
        : m_posix_passthrough.passthrough_posix_lxstat64 (version, path, statbuf);
}

/**
 * __fxstat64:
 *  This method respects to the fstat64 call.
 * @param version
 * @param fd
 * @param statbuf
 * @return
 */
extern "C" int __fxstat64 (int version, int fd, struct stat64* statbuf)
{
    return (posix_metadata_calls.paddl_intercept_fxstat64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fxstat64 (version, fd, statbuf)
        : m_posix_passthrough.passthrough_posix_fxstat64 (version, fd, statbuf);
}

/**
 * fxstatat64:
 *  This method respects to the fstatat64 call.
 * @param version
 * @param dirfd
 * @param path
 * @param statbuf
 * @param flags
 * @return
 */
extern "C" int
__fxstatat64 (int version, int dirfd, const char* path, struct stat64* statbuf, int flags)
{
    return (posix_metadata_calls.paddl_intercept_fxstatat64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fxstatat64 (version, dirfd, path, statbuf, flags)
        : m_posix_passthrough.passthrough_posix_fxstatat64 (version, dirfd, path, statbuf, flags);
}

/**
 * statfs:
 * @param path
 * @param buf
 * @return
 */
extern "C" int statfs (const char* path, struct statfs* buf)
{
    return (posix_metadata_calls.paddl_intercept_statfs)
        ? m_ld_preloaded_posix.ld_preloaded_posix_statfs (path, buf)
        : m_posix_passthrough.passthrough_posix_statfs (path, buf);
}

/**
 * fstatfs:
 * @param fd
 * @param buf
 * @return
 */
extern "C" int fstatfs (int fd, struct statfs* buf)
{
    return (posix_metadata_calls.paddl_intercept_fstatfs)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fstatfs (fd, buf)
        : m_posix_passthrough.passthrough_posix_fstatfs (fd, buf);
}

/**
 * statfs64:
 * @param path
 * @param buf
 * @return
 */
extern "C" int statfs64 (const char* path, struct statfs64* buf)
{
    return (posix_metadata_calls.paddl_intercept_statfs64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_statfs64 (path, buf)
        : m_posix_passthrough.passthrough_posix_statfs64 (path, buf);
}

/**
 * fstatfs64:
 * @param fd
 * @param buf
 * @return
 */
extern "C" int fstatfs64 (int fd, struct statfs64* buf)
{
    return (posix_metadata_calls.paddl_intercept_fstatfs64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fstatfs64 (fd, buf)
        : m_posix_passthrough.passthrough_posix_fstatfs64 (fd, buf);
}

/**
 * link:
 * @param old_path
 * @param new_path
 * @return
 */
extern "C" int link (const char* old_path, const char* new_path)
{
    return (posix_metadata_calls.paddl_intercept_link)
        ? m_ld_preloaded_posix.ld_preloaded_posix_link (old_path, new_path)
        : m_posix_passthrough.passthrough_posix_link (old_path, new_path);
}

/**
 * unlink:
 * @param path
 * @return
 */
extern "C" int unlink (const char* path)
{
    return (posix_metadata_calls.paddl_intercept_unlink)
        ? m_ld_preloaded_posix.ld_preloaded_posix_unlink (path)
        : m_posix_passthrough.passthrough_posix_unlink (path);
}

/**
 * linkat:
 * @param olddirfd
 * @param old_path
 * @param newdirfd
 * @param new_path
 * @param flags
 * @return
 */
extern "C" int
linkat (int olddirfd, const char* old_path, int newdirfd, const char* new_path, int flags)
{
    return (posix_metadata_calls.paddl_intercept_linkat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_linkat (olddirfd,
            old_path,
            newdirfd,
            new_path,
            flags)
        : m_posix_passthrough.passthrough_posix_linkat (olddirfd,
            old_path,
            newdirfd,
            new_path,
            flags);
}

/**
 * unlinkat:
 * @param dirfd
 * @param pathname
 * @param flags
 * @return
 */
extern "C" int unlinkat (int dirfd, const char* pathname, int flags)
{
    return (posix_metadata_calls.paddl_intercept_unlinkat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_unlinkat (dirfd, pathname, flags)
        : m_posix_passthrough.passthrough_posix_unlinkat (dirfd, pathname, flags);
}

/**
 * rename:
 *  https://man7.org/linux/man-pages/man2/rename.2.html
 * @param old_path
 * @param new_path
 * @return
 */
extern "C" int rename (const char* old_path, const char* new_path)
{
    return (posix_metadata_calls.paddl_intercept_rename)
        ? m_ld_preloaded_posix.ld_preloaded_posix_rename (old_path, new_path)
        : m_posix_passthrough.passthrough_posix_rename (old_path, new_path);
}

/**
 * renameat:
 *  https://man7.org/linux/man-pages/man2/renameat.2.html
 * @param olddirfd
 * @param old_path
 * @param newdirfd
 * @param new_path
 * @return
 */
extern "C" int renameat (int olddirfd, const char* old_path, int newdirfd, const char* new_path)
{
    return (posix_metadata_calls.paddl_intercept_renameat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_renameat (olddirfd, old_path, newdirfd, new_path)
        : m_posix_passthrough.passthrough_posix_renameat (olddirfd, old_path, newdirfd, new_path);
}

/**
 * symlink:
 * @param target
 * @param linkpath
 * @return
 */
extern "C" int symlink (const char* target, const char* linkpath)
{
    return (posix_metadata_calls.paddl_intercept_symlink)
        ? m_ld_preloaded_posix.ld_preloaded_posix_symlink (target, linkpath)
        : m_posix_passthrough.passthrough_posix_symlink (target, linkpath);
}

/**
 * symlinkat:
 * @param target
 * @param newdirfd
 * @param linkpath
 * @return
 */
extern "C" int symlinkat (const char* target, int newdirfd, const char* linkpath)
{
    return (posix_metadata_calls.paddl_intercept_symlinkat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_symlinkat (target, newdirfd, linkpath)
        : m_posix_passthrough.passthrough_posix_symlinkat (target, newdirfd, linkpath);
}

/**
 * readlink:
 * @param path
 * @param buf
 * @param bufsize
 * @return
 */
extern "C" ssize_t readlink (const char* path, char* buf, size_t bufsize)
{
    return (posix_metadata_calls.paddl_intercept_readlink)
        ? m_ld_preloaded_posix.ld_preloaded_posix_readlink (path, buf, bufsize)
        : m_posix_passthrough.passthrough_posix_readlink (path, buf, bufsize);
}

/**
 * readlinkat:
 * @param dirfd
 * @param path
 * @param buf
 * @param bufsize
 * @return
 */
extern "C" ssize_t readlinkat (int dirfd, const char* path, char* buf, size_t bufsize)
{
    return (posix_metadata_calls.paddl_intercept_readlinkat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_readlinkat (dirfd, path, buf, bufsize)
        : m_posix_passthrough.passthrough_posix_readlinkat (dirfd, path, buf, bufsize);
}

/**
 * fopen:
 * @param pathname
 * @param mode
 * @return
 */
extern "C" FILE* fopen (const char* pathname, const char* mode)
{
    return (posix_metadata_calls.paddl_intercept_fopen)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fopen (pathname, mode)
        : m_posix_passthrough.passthrough_posix_fopen (pathname, mode);
}

/**
 * fopen64:
 * @param pathname
 * @param mode
 * @return
 */
extern "C" FILE* fopen64 (const char* pathname, const char* mode)
{
    return (posix_metadata_calls.paddl_intercept_fopen64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fopen64 (pathname, mode)
        : m_posix_passthrough.passthrough_posix_fopen64 (pathname, mode);
}

/**
 * fdopen:
 * @param fd
 * @param mode
 * @return
 */
extern "C" FILE* fdopen (int fd, const char* mode)
{
    return (posix_metadata_calls.paddl_intercept_fdopen)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fdopen (fd, mode)
        : m_posix_passthrough.passthrough_posix_fdopen (fd, mode);
}

/**
 * freopen:
 * @param pathname
 * @param mode
 * @param stream
 * @return
 */
extern "C" FILE* freopen (const char* pathname, const char* mode, FILE* stream)
{
    return (posix_metadata_calls.paddl_intercept_freopen)
        ? m_ld_preloaded_posix.ld_preloaded_posix_freopen (pathname, mode, stream)
        : m_posix_passthrough.passthrough_posix_freopen (pathname, mode, stream);
}

/**
 * freopen64:
 * @param pathname
 * @param mode
 * @param stream
 * @return
 */
extern "C" FILE* freopen64 (const char* pathname, const char* mode, FILE* stream)
{
    return (posix_metadata_calls.paddl_intercept_freopen64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_freopen64 (pathname, mode, stream)
        : m_posix_passthrough.passthrough_posix_freopen64 (pathname, mode, stream);
}

/**
 * fclose:
 * @param stream
 * @return
 */
extern "C" int fclose (FILE* stream)
{
    return (posix_metadata_calls.paddl_intercept_fclose)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fclose (stream)
        : m_posix_passthrough.passthrough_posix_fclose (stream);
}

/**
 * fflush:
 * Note: Attention: this method will (as well as fwrite) will through a segmentation fault when
 *  using the Logging library (even in the initialization, constructors, ...).
 * @param stream
 * @return
 */
extern "C" int fflush (FILE* stream)
{
    return (posix_metadata_calls.paddl_intercept_fflush)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fflush (stream)
        : m_posix_passthrough.passthrough_posix_fflush (stream);
}

/**
 * access:
 * @param path
 * @param mode
 * @return
 */
extern "C" int access (const char* path, int mode)
{
    return (posix_metadata_calls.paddl_intercept_access)
        ? m_ld_preloaded_posix.ld_preloaded_posix_access (path, mode)
        : m_posix_passthrough.passthrough_posix_access (path, mode);
}

/**
 * faccessat:
 * @param dirfd
 * @param path
 * @param mode
 * @param flags
 * @return
 */
extern "C" int faccessat (int dirfd, const char* path, int mode, int flags)
{
    return (posix_metadata_calls.paddl_intercept_faccessat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_faccessat (dirfd, path, mode, flags)
        : m_posix_passthrough.passthrough_posix_faccessat (dirfd, path, mode, flags);
}

/**
 * lseek:
 * @param fd
 * @param offset
 * @param whence
 * @return
 */
extern "C" off_t lseek (int fd, off_t offset, int whence)
{
    return (posix_metadata_calls.paddl_intercept_lseek)
        ? m_ld_preloaded_posix.ld_preloaded_posix_lseek (fd, offset, whence)
        : m_posix_passthrough.passthrough_posix_lseek (fd, offset, whence);
}

/**
 * fseek:
 * @param stream
 * @param offset
 * @param whence
 * @return
 */
extern "C" int fseek (FILE* stream, long offset, int whence)
{
    return (posix_metadata_calls.paddl_intercept_fseek)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fseek (stream, offset, whence)
        : m_posix_passthrough.passthrough_posix_fseek (stream, offset, whence);
}

/**
 * ftell:
 * @param stream
 * @return
 */
extern "C" long ftell (FILE* stream)
{
    return (posix_metadata_calls.paddl_intercept_ftell)
        ? m_ld_preloaded_posix.ld_preloaded_posix_ftell (stream)
        : m_posix_passthrough.passthrough_posix_ftell (stream);
}

/**
 * lseek64:
 * @param fd
 * @param offset
 * @param whence
 * @return
 */
extern "C" off_t lseek64 (int fd, off_t offset, int whence)
{
    return (posix_metadata_calls.paddl_intercept_lseek64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_lseek64 (fd, offset, whence)
        : m_posix_passthrough.passthrough_posix_lseek64 (fd, offset, whence);
}

/**
 * fseeko64:
 * @param stream
 * @param offset
 * @param whence
 * @return
 */
extern "C" int fseeko64 (FILE* stream, off_t offset, int whence)
{
    return (posix_metadata_calls.paddl_intercept_fseeko64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fseeko64 (stream, offset, whence)
        : m_posix_passthrough.passthrough_posix_fseeko64 (stream, offset, whence);
}

/**
 * ftello64:
 * @param stream
 * @return
 */
extern "C" off_t ftello64 (FILE* stream)
{
    return (posix_metadata_calls.paddl_intercept_ftello64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_ftello64 (stream)
        : m_posix_passthrough.passthrough_posix_ftello64 (stream);
}

/**
 * mkdir:
 * @param path
 * @param mode
 * @return
 */
extern "C" int mkdir (const char* path, mode_t mode)
{
    return (posix_directory_calls.padll_intercept_mkdir)
        ? m_ld_preloaded_posix.ld_preloaded_posix_mkdir (path, mode)
        : m_posix_passthrough.passthrough_posix_mkdir (path, mode);
}

/**
 * mkdirat:
 * @param dirfd
 * @param path
 * @param mode
 * @return
 */
extern "C" int mkdirat (int dirfd, const char* path, mode_t mode)
{
    return (posix_directory_calls.padll_intercept_mkdirat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_mkdirat (dirfd, path, mode)
        : m_posix_passthrough.passthrough_posix_mkdirat (dirfd, path, mode);
}

/**
 * readdir:
 * @param dirp
 * @return
 */
extern "C" struct dirent* readdir (DIR* dirp)
{
    return (posix_directory_calls.padll_intercept_readdir)
        ? m_ld_preloaded_posix.ld_preloaded_posix_readdir (dirp)
        : m_posix_passthrough.passthrough_posix_readdir (dirp);
}

/**
 * readdir64:
 * @param dirp
 * @return
 */
extern "C" struct dirent64* readdir64 (DIR* dirp)
{
#if defined(__unix__) || defined(__linux)
    return (posix_directory_calls.padll_intercept_readdir64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_readdir64 (dirp)
        : m_posix_passthrough.passthrough_posix_readdir64 (dirp);
#endif
    return nullptr;
}

/**
 * opendir:
 * @param path
 * @return
 */
extern "C" DIR* opendir (const char* path)
{
    return (posix_directory_calls.padll_intercept_opendir)
        ? m_ld_preloaded_posix.ld_preloaded_posix_opendir (path)
        : m_posix_passthrough.passthrough_posix_opendir (path);
}

/**
 * fdopendir:
 * @param fd
 * @return
 */
extern "C" DIR* fdopendir (int fd)
{
    return (posix_directory_calls.padll_intercept_fdopendir)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fdopendir (fd)
        : m_posix_passthrough.passthrough_posix_fdopendir (fd);
}

/**
 * closedir:
 * @param dirp
 * @return
 */
extern "C" int closedir (DIR* dirp)
{
    return (posix_directory_calls.padll_intercept_closedir)
        ? m_ld_preloaded_posix.ld_preloaded_posix_closedir (dirp)
        : m_posix_passthrough.passthrough_posix_closedir (dirp);
}

/**
 * rmdir:
 * @param path
 * @return
 */
extern "C" int rmdir (const char* path)
{
    return (posix_directory_calls.padll_intercept_rmdir)
        ? m_ld_preloaded_posix.ld_preloaded_posix_rmdir (path)
        : m_posix_passthrough.passthrough_posix_rmdir (path);
}

/**
 * dirfd:
 * @param dirp
 * @return
 */
extern "C" int dirfd (DIR* dirp)
{
    return (posix_directory_calls.padll_intercept_dirfd)
        ? m_ld_preloaded_posix.ld_preloaded_posix_dirfd (dirp)
        : m_posix_passthrough.passthrough_posix_dirfd (dirp);
}

/**
 * getxattr:
 * @param path
 * @param name
 * @param value
 * @param size
 * @return
 */
extern "C" ssize_t getxattr (const char* path, const char* name, void* value, size_t size)
{
    return (posix_extended_attributes_calls.padll_intercept_getxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_getxattr (path, name, value, size)
        : m_posix_passthrough.passthrough_posix_getxattr (path, name, value, size);
}

/**
 * lgetxattr:
 * @param path
 * @param name
 * @param value
 * @param size
 * @return
 */
extern "C" ssize_t lgetxattr (const char* path, const char* name, void* value, size_t size)
{
    return (posix_extended_attributes_calls.padll_intercept_lgetxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_lgetxattr (path, name, value, size)
        : m_posix_passthrough.passthrough_posix_lgetxattr (path, name, value, size);
}

/**
 * fgetxattr:
 * @param fd
 * @param name
 * @param value
 * @param size
 * @return
 */
extern "C" ssize_t fgetxattr (int fd, const char* name, void* value, size_t size)
{
    return (posix_extended_attributes_calls.padll_intercept_fgetxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fgetxattr (fd, name, value, size)
        : m_posix_passthrough.passthrough_posix_fgetxattr (fd, name, value, size);
}

/**
 * setxattr:
 * @param path
 * @param name
 * @param value
 * @param size
 * @param flags
 * @return
 */
extern "C" int
setxattr (const char* path, const char* name, const void* value, size_t size, int flags)
{
    return (posix_extended_attributes_calls.padll_intercept_setxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_setxattr (path, name, value, size, flags)
        : m_posix_passthrough.passthrough_posix_setxattr (path, name, value, size, flags);
}

/**
 * lsetxattr:
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
    return (posix_extended_attributes_calls.padll_intercept_lsetxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_lsetxattr (path, name, value, size, flags)
        : m_posix_passthrough.passthrough_posix_lsetxattr (path, name, value, size, flags);
}

/**
 * fsetxattr:
 * @param fd
 * @param name
 * @param value
 * @param size
 * @param flags
 * @return
 */
extern "C" int fsetxattr (int fd, const char* name, const void* value, size_t size, int flags)
{
    return (posix_extended_attributes_calls.padll_intercept_fsetxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fsetxattr (fd, name, value, size, flags)
        : m_posix_passthrough.passthrough_posix_fsetxattr (fd, name, value, size, flags);
}

/**
 * listxattr:
 * @param path
 * @param list
 * @param size
 * @return
 */
extern "C" ssize_t listxattr (const char* path, char* list, size_t size)
{
    return (posix_extended_attributes_calls.padll_intercept_listxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_listxattr (path, list, size)
        : m_posix_passthrough.passthrough_posix_listxattr (path, list, size);
}

/**
 * llistxattr:
 * @param path
 * @param list
 * @param size
 * @return
 */
extern "C" ssize_t llistxattr (const char* path, char* list, size_t size)
{
    return (posix_extended_attributes_calls.padll_intercept_llistxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_llistxattr (path, list, size)
        : m_posix_passthrough.passthrough_posix_llistxattr (path, list, size);
}

/**
 * flistxattr:
 * @param fd
 * @param list
 * @param size
 * @return
 */
extern "C" ssize_t flistxattr (int fd, char* list, size_t size)
{
    return (posix_extended_attributes_calls.padll_intercept_flistxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_flistxattr (fd, list, size)
        : m_posix_passthrough.passthrough_posix_flistxattr (fd, list, size);
}

/**
 * removexattr:
 * @param path
 * @param name
 * @return
 */
extern "C" int removexattr (const char* path, const char* name)
{
    return (posix_extended_attributes_calls.padll_intercept_removexattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_removexattr (path, name)
        : m_posix_passthrough.passthrough_posix_removexattr (path, name);
}

/**
 * lremovexattr:
 * @param path
 * @param name
 * @return
 */
extern "C" int lremovexattr (const char* path, const char* name)
{
    return (posix_extended_attributes_calls.padll_intercept_lremovexattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_lremovexattr (path, name)
        : m_posix_passthrough.passthrough_posix_lremovexattr (path, name);
}

/**
 * fremovexattr:
 * @param fd
 * @param name
 * @return
 */
extern "C" int fremovexattr (int fd, const char* name)
{
    return (posix_extended_attributes_calls.padll_intercept_fremovexattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fremovexattr (fd, name)
        : m_posix_passthrough.passthrough_posix_fremovexattr (fd, name);
}

/**
 * chmod:
 * @param pathname
 * @param mode
 * @return
 */
extern "C" int chmod (const char* pathname, mode_t mode)
{
    return (posix_file_modes_calls.padll_intercept_chmod)
        ? m_ld_preloaded_posix.ld_preloaded_posix_chmod (pathname, mode)
        : m_posix_passthrough.passthrough_posix_chmod (pathname, mode);
}

/**
 * fchmod:
 * @param fd
 * @param mode
 * @return
 */
extern "C" int fchmod (int fd, mode_t mode)
{
    return (posix_file_modes_calls.padll_intercept_fchmod)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fchmod (fd, mode)
        : m_posix_passthrough.passthrough_posix_fchmod (fd, mode);
}

/**
 * fchmodat:
 * @param dirfd
 * @param pathname
 * @param mode
 * @param flags
 * @return
 */
extern "C" int fchmodat (int dirfd, const char* pathname, mode_t mode, int flags)
{
    return (posix_file_modes_calls.padll_intercept_fchmodat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fchmodat (dirfd, pathname, mode, flags)
        : m_posix_passthrough.passthrough_posix_fchmodat (dirfd, pathname, mode, flags);
}

/**
 * chown:
 * @param pathname
 * @param owner
 * @param group
 * @return
 */
extern "C" int chown (const char* pathname, uid_t owner, gid_t group)
{
    return (posix_file_modes_calls.padll_intercept_chown)
        ? m_ld_preloaded_posix.ld_preloaded_posix_chown (pathname, owner, group)
        : m_posix_passthrough.passthrough_posix_chown (pathname, owner, group);
}

/**
 * lchown:
 * @param pathname
 * @param owner
 * @param group
 * @return
 */
extern "C" int lchown (const char* pathname, uid_t owner, gid_t group)
{
    return (posix_file_modes_calls.padll_intercept_lchown)
        ? m_ld_preloaded_posix.ld_preloaded_posix_lchown (pathname, owner, group)
        : m_posix_passthrough.passthrough_posix_lchown (pathname, owner, group);
}

/**
 * fchown:
 * @param fd
 * @param owner
 * @param group
 * @return
 */
extern "C" int fchown (int fd, uid_t owner, gid_t group)
{
    return (posix_file_modes_calls.padll_intercept_fchown)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fchown (fd, owner, group)
        : m_posix_passthrough.passthrough_posix_fchown (fd, owner, group);
}

/**
 * fchownat:
 * @param dirfd
 * @param pathname
 * @param owner
 * @param group
 * @param flags
 * @return
 */
extern "C" int fchownat (int dirfd, const char* pathname, uid_t owner, gid_t group, int flags)
{
    return (posix_file_modes_calls.padll_intercept_fchownat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fchownat (dirfd, pathname, owner, group, flags)
        : m_posix_passthrough.passthrough_posix_fchownat (dirfd, pathname, owner, group, flags);
}

#endif // PADLL_POSIX_FILE_SYSTEM_H
