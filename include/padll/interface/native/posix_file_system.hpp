/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
 **/

#ifndef PADLL_POSIX_FILE_SYSTEM_H
#define PADLL_POSIX_FILE_SYSTEM_H

#include <cstdarg>
#include <cstring>
#include <padll/configurations/libc_calls.hpp>
#include <padll/interface/ldpreloaded/ld_preloaded_posix.hpp>
#include <padll/interface/passthrough/posix_passthrough.hpp>
#include <thread>

namespace ldp = padll::interface::ldpreloaded;
namespace ptr = padll::interface::passthrough;
namespace opt = padll::options;

/**
 * LdPreloaded file system object.
 */
ldp::LdPreloadedPosix m_ld_preloaded_posix {};

/**
 * PosixPassthrough file system object.
 */
ptr::PosixPassthrough m_posix_passthrough {};

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
 * mmap:
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
    // return (posix_data_calls.padll_intercept_mmap)
    //     ? m_ld_preloaded_posix.ld_preloaded_posix_mmap (addr, length, prot, flags, fd, offset)
    //     : m_posix_passthrough.passthrough_posix_mmap (addr, length, prot, flags, fd, offset);
    return m_posix_passthrough.passthrough_posix_mmap (addr, length, prot, flags, fd, offset);
}

/**
 * munmap:
 * @param addr
 * @param length
 * @return
 */
extern "C" int munmap (void* addr, size_t length)
{
    // return (posix_data_calls.padll_intercept_munmap)
    // ? m_ld_preloaded_posix.ld_preloaded_posix_munmap (addr, length)
    // : m_posix_passthrough.passthrough_posix_munmap (addr, length);
    return m_posix_passthrough.passthrough_posix_munmap (addr, length);
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

        return (posix_metadata_calls.padll_intercept_open_var)
            ? m_ld_preloaded_posix.ld_preloaded_posix_open (path, flags, mode)
            : m_posix_passthrough.passthrough_posix_open (path, flags, mode);
    } else {
        return (posix_metadata_calls.padll_intercept_open)
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
    return (posix_metadata_calls.padll_intercept_creat)
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
    return (posix_metadata_calls.padll_intercept_creat64)
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

        return (posix_metadata_calls.padll_intercept_openat_var)
            ? m_ld_preloaded_posix.ld_preloaded_posix_openat (dirfd, path, flags, mode)
            : m_posix_passthrough.passthrough_posix_openat (dirfd, path, flags, mode);
    } else {
        return (posix_metadata_calls.padll_intercept_openat)
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

        return (posix_metadata_calls.padll_intercept_open64_var)
            ? m_ld_preloaded_posix.ld_preloaded_posix_open64 (path, flags, mode)
            : m_posix_passthrough.passthrough_posix_open64 (path, flags, mode);
    } else {
        return (posix_metadata_calls.padll_intercept_open64)
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
    return (posix_metadata_calls.padll_intercept_close)
        ? m_ld_preloaded_posix.ld_preloaded_posix_close (fd)
        : m_posix_passthrough.passthrough_posix_close (fd);
}

/**
 * sync:
 */
extern "C" void sync ()
{
    return (posix_metadata_calls.padll_intercept_sync)
        ? m_ld_preloaded_posix.ld_preloaded_posix_sync ()
        : m_posix_passthrough.passthrough_posix_sync ();
}

/**
 * statfs:
 * @param path
 * @param buf
 * @return
 */
extern "C" int statfs (const char* path, struct statfs* buf)
{
    return (posix_metadata_calls.padll_intercept_statfs)
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
    return (posix_metadata_calls.padll_intercept_fstatfs)
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
    return (posix_metadata_calls.padll_intercept_statfs64)
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
    return (posix_metadata_calls.padll_intercept_fstatfs64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fstatfs64 (fd, buf)
        : m_posix_passthrough.passthrough_posix_fstatfs64 (fd, buf);
}

/**
 * unlink:
 * @param path
 * @return
 */
extern "C" int unlink (const char* path)
{
    return (posix_metadata_calls.padll_intercept_unlink)
        ? m_ld_preloaded_posix.ld_preloaded_posix_unlink (path)
        : m_posix_passthrough.passthrough_posix_unlink (path);
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
    return (posix_metadata_calls.padll_intercept_unlinkat)
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
    return (posix_metadata_calls.padll_intercept_rename)
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
    return (posix_metadata_calls.padll_intercept_renameat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_renameat (olddirfd, old_path, newdirfd, new_path)
        : m_posix_passthrough.passthrough_posix_renameat (olddirfd, old_path, newdirfd, new_path);
}

/**
 * fopen:
 * @param pathname
 * @param mode
 * @return
 */
extern "C" FILE* fopen (const char* pathname, const char* mode)
{
    return (posix_metadata_calls.padll_intercept_fopen)
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
    return (posix_metadata_calls.padll_intercept_fopen64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fopen64 (pathname, mode)
        : m_posix_passthrough.passthrough_posix_fopen64 (pathname, mode);
}

/**
 * fclose:
 * @param stream
 * @return
 */
extern "C" int fclose (FILE* stream)
{
    return (posix_metadata_calls.padll_intercept_fclose)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fclose (stream)
        : m_posix_passthrough.passthrough_posix_fclose (stream);
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
 * mknod:
 * @param path
 * @param mode
 * @param dev
 * @return
 */
extern "C" int mknod (const char* path, mode_t mode, dev_t dev)
{
    // TODO: redirect to ld_preloaded_posix as well
    return m_posix_passthrough.passthrough_posix_mknod (path, mode, dev);
}

/**
 * mknodat:
 * @param dirfd
 * @param path
 * @param mode
 * @param dev
 * @return
 */
extern "C" int mknodat (int dirfd, const char* path, mode_t mode, dev_t dev)
{
    // TODO: redirect to ld_preloaded_posix as well
    return m_posix_passthrough.passthrough_posix_mknodat (dirfd, path, mode, dev);
}

/**
 * getxattr:
 * @param path
 * @param name
 * @param value
 * @param size
 * @return
 */
#ifdef __linux__
extern "C" ssize_t getxattr (const char* path, const char* name, void* value, size_t size)
{
    return (posix_extended_attributes_calls.padll_intercept_getxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_getxattr (path, name, value, size)
        : m_posix_passthrough.passthrough_posix_getxattr (path, name, value, size);
}
#endif

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
#ifdef __linux__
extern "C" ssize_t fgetxattr (int fd, const char* name, void* value, size_t size)
{
    return (posix_extended_attributes_calls.padll_intercept_fgetxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fgetxattr (fd, name, value, size)
        : m_posix_passthrough.passthrough_posix_fgetxattr (fd, name, value, size);
}
#endif

/**
 * setxattr:
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
    return (posix_extended_attributes_calls.padll_intercept_setxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_setxattr (path, name, value, size, flags)
        : m_posix_passthrough.passthrough_posix_setxattr (path, name, value, size, flags);
}
#endif

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
#ifdef __linux__
extern "C" int fsetxattr (int fd, const char* name, const void* value, size_t size, int flags)
{
    return (posix_extended_attributes_calls.padll_intercept_fsetxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fsetxattr (fd, name, value, size, flags)
        : m_posix_passthrough.passthrough_posix_fsetxattr (fd, name, value, size, flags);
}
#endif

/**
 * listxattr:
 * @param path
 * @param list
 * @param size
 * @return
 */
#ifdef __linux__
extern "C" ssize_t listxattr (const char* path, char* list, size_t size)
{
    return (posix_extended_attributes_calls.padll_intercept_listxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_listxattr (path, list, size)
        : m_posix_passthrough.passthrough_posix_listxattr (path, list, size);
}
#endif

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
#ifdef __linux__
extern "C" ssize_t flistxattr (int fd, char* list, size_t size)
{
    return (posix_extended_attributes_calls.padll_intercept_flistxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_flistxattr (fd, list, size)
        : m_posix_passthrough.passthrough_posix_flistxattr (fd, list, size);
}
#endif

#endif // PADLL_POSIX_FILE_SYSTEM_H
