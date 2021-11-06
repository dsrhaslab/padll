/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <padll/interface/native/posix_file_system.hpp>

// read call. (...)
ssize_t read (int fd, void* buf, size_t size)
{
    return (posix_data_calls.padll_intercept_read)
        ? m_ld_preloaded_posix.ld_preloaded_posix_read (fd, buf, size)
        : m_posix_passthrough.passthrough_posix_read (fd, buf, size);
}

// write call. (...)
ssize_t write (int fd, const void* buf, size_t size)
{
    return (posix_data_calls.padll_intercept_write)
        ? m_ld_preloaded_posix.ld_preloaded_posix_write (fd, buf, size)
        : m_posix_passthrough.passthrough_posix_write (fd, buf, size);
}

// pread call. (...)
ssize_t pread (int fd, void* buf, size_t size, off_t offset)
{
    return (posix_data_calls.padll_intercept_pread)
        ? m_ld_preloaded_posix.ld_preloaded_posix_pread (fd, buf, size, offset)
        : m_posix_passthrough.passthrough_posix_pread (fd, buf, size, offset);
}

// pwrite call. (...)
ssize_t pwrite (int fd, const void* buf, size_t size, off_t offset)
{
    return (posix_data_calls.padll_intercept_pwrite)
        ? m_ld_preloaded_posix.ld_preloaded_posix_pwrite (fd, buf, size, offset)
        : m_posix_passthrough.passthrough_posix_pwrite (fd, buf, size, offset);
}

// pread64 call. (...)
#if defined(__USE_LARGEFILE64)
ssize_t pread64 (int fd, void* buf, size_t size, off64_t offset)
{
    return (posix_data_calls.padll_intercept_pread64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_pread64 (fd, buf, size, offset)
        : m_posix_passthrough.passthrough_posix_pread64 (fd, buf, size, offset);
}
#endif

// pwrite64 call. (...)
#if defined(__USE_LARGEFILE64)
ssize_t pwrite64 (int fd, const void* buf, size_t size, off64_t offset)
{
    return (posix_data_calls.padll_intercept_pwrite64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_pwrite64 (fd, buf, size, offset)
        : m_posix_passthrough.passthrough_posix_pwrite64 (fd, buf, size, offset);
}
#endif

// fread call. (...)
size_t fread (void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    return (posix_data_calls.padll_intercept_fread)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fread (ptr, size, nmemb, stream)
        : m_posix_passthrough.passthrough_posix_fread (ptr, size, nmemb, stream);
}

// fwrite call. (...)
size_t fwrite (const void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    return (posix_data_calls.padll_intercept_fwrite)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fwrite (ptr, size, nmemb, stream)
        : m_posix_passthrough.passthrough_posix_fwrite (ptr, size, nmemb, stream);
}

// open call. (...)
int open (const char* path, int flags, ...)
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

// creat call. (...)
int creat (const char* path, mode_t mode)
{
    return (posix_metadata_calls.paddl_intercept_creat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_creat (path, mode)
        : m_posix_passthrough.passthrough_posix_creat (path, mode);
}

// creat64 call. (...)
int creat64 (const char* path, mode_t mode)
{
    return (posix_metadata_calls.paddl_intercept_creat64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_creat64 (path, mode)
        : m_posix_passthrough.passthrough_posix_creat64 (path, mode);
}

// openat call. (...)
int openat (int dirfd, const char* path, int flags, ...)
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

// open64 call. (...)
int open64 (const char* path, int flags, ...)
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

// close call. (...)
int close (int fd)
{
    return (posix_metadata_calls.paddl_intercept_close)
        ? m_ld_preloaded_posix.ld_preloaded_posix_close (fd)
        : m_posix_passthrough.passthrough_posix_close (fd);
}

// fsync call. (...)
int fsync (int fd)
{
    return (posix_metadata_calls.paddl_intercept_fsync)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fsync (fd)
        : m_posix_passthrough.passthrough_posix_fsync (fd);
}

// fdatasync call. (...)
int fdatasync (int fd)
{
    return (posix_metadata_calls.paddl_intercept_fdatasync)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fdatasync (fd)
        : m_posix_passthrough.passthrough_posix_fdatasync (fd);
}

// sync call. (...)
void sync ()
{
    return (posix_metadata_calls.paddl_intercept_sync)
        ? m_ld_preloaded_posix.ld_preloaded_posix_sync ()
        : m_posix_passthrough.passthrough_posix_sync ();
}

// syncfs call. (...)
int syncfs (int fd)
{
    return (posix_metadata_calls.paddl_intercept_syncfs)
        ? m_ld_preloaded_posix.ld_preloaded_posix_syncfs (fd)
        : m_posix_passthrough.passthrough_posix_syncfs (fd);
}

// truncate call. (...)
int truncate (const char* path, off_t length)
{
    return (posix_metadata_calls.paddl_intercept_truncate)
        ? m_ld_preloaded_posix.ld_preloaded_posix_truncate (path, length)
        : m_posix_passthrough.passthrough_posix_truncate (path, length);
}

// ftruncate call. (...)
int ftruncate (int fd, off_t length)
{
    return (posix_metadata_calls.paddl_intercept_ftruncate)
        ? m_ld_preloaded_posix.ld_preloaded_posix_ftruncate (fd, length)
        : m_posix_passthrough.passthrough_posix_ftruncate (fd, length);
}

// truncate64 call. (...)
int truncate64 (const char* path, off_t length)
{
    return (posix_metadata_calls.paddl_intercept_truncate64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_truncate64 (path, length)
        : m_posix_passthrough.passthrough_posix_truncate64 (path, length);
}

// ftruncate64 call. (...)
int ftruncate64 (int fd, off_t length)
{
    return (posix_metadata_calls.paddl_intercept_ftruncate64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_ftruncate64 (fd, length)
        : m_posix_passthrough.passthrough_posix_ftruncate64 (fd, length);
}

// __xstat call. (...)
int __xstat (int version, const char* path, struct stat* statbuf)
{
    return (posix_metadata_calls.paddl_intercept_xstat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_xstat (version, path, statbuf)
        : m_posix_passthrough.passthrough_posix_xstat (version, path, statbuf);
}

// __lxstat call. (...)
int __lxstat (int version, const char* path, struct stat* statbuf)
{
    return (posix_metadata_calls.paddl_intercept_lxstat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_lxstat (version, path, statbuf)
        : m_posix_passthrough.passthrough_posix_lxstat (version, path, statbuf);
}

// __fxstat call. (...)
int __fxstat (int version, int fd, struct stat* statbuf)
{
    return (posix_metadata_calls.paddl_intercept_fxstat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fxstat (version, fd, statbuf)
        : m_posix_passthrough.passthrough_posix_fxstat (version, fd, statbuf);
}

// __fxstatat call. (...)
int __fxstatat (int version, int dirfd, const char* path, struct stat* statbuf, int flags)
{
    return (posix_metadata_calls.paddl_intercept_fxstatat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fxstatat (version, dirfd, path, statbuf, flags)
        : m_posix_passthrough.passthrough_posix_fxstatat (version, dirfd, path, statbuf, flags);
}

// __xstat64 call. (...)
int __xstat64 (int version, const char* path, struct stat64* statbuf)
{
    return (posix_metadata_calls.paddl_intercept_xstat64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_xstat64 (version, path, statbuf)
        : m_posix_passthrough.passthrough_posix_xstat64 (version, path, statbuf);
}

// __lxstat64 call. (...)
int __lxstat64 (int version, const char* path, struct stat64* statbuf)
{
    return (posix_metadata_calls.paddl_intercept_lxstat64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_lxstat64 (version, path, statbuf)
        : m_posix_passthrough.passthrough_posix_lxstat64 (version, path, statbuf);
}

// __fxstat64 call. (...)
int __fxstat64 (int version, int fd, struct stat64* statbuf)
{
    return (posix_metadata_calls.paddl_intercept_fxstat64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fxstat64 (version, fd, statbuf)
        : m_posix_passthrough.passthrough_posix_fxstat64 (version, fd, statbuf);
}

// __fxstatat64 call. (...)
int __fxstatat64 (int version, int dirfd, const char* path, struct stat64* statbuf, int flags)
{
    return (posix_metadata_calls.paddl_intercept_fxstatat64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fxstatat64 (version, dirfd, path, statbuf, flags)
        : m_posix_passthrough.passthrough_posix_fxstatat64 (version, dirfd, path, statbuf, flags);
}

// statfs call. (...)
int statfs (const char* path, struct statfs* buf)
{
    return (posix_metadata_calls.paddl_intercept_statfs)
        ? m_ld_preloaded_posix.ld_preloaded_posix_statfs (path, buf)
        : m_posix_passthrough.passthrough_posix_statfs (path, buf);
}

// fstatfs call. (...)
int fstatfs (int fd, struct statfs* buf)
{
    return (posix_metadata_calls.paddl_intercept_fstatfs)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fstatfs (fd, buf)
        : m_posix_passthrough.passthrough_posix_fstatfs (fd, buf);
}

// statfs64 call. (...)
int statfs64 (const char* path, struct statfs64* buf)
{
    return (posix_metadata_calls.paddl_intercept_statfs64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_statfs64 (path, buf)
        : m_posix_passthrough.passthrough_posix_statfs64 (path, buf);
}

// fstatfs64 call. (...)
int fstatfs64 (int fd, struct statfs64* buf)
{
    return (posix_metadata_calls.paddl_intercept_fstatfs64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fstatfs64 (fd, buf)
        : m_posix_passthrough.passthrough_posix_fstatfs64 (fd, buf);
}

// link call. (...)
int link (const char* old_path, const char* new_path)
{
    return (posix_metadata_calls.paddl_intercept_link)
        ? m_ld_preloaded_posix.ld_preloaded_posix_link (old_path, new_path)
        : m_posix_passthrough.passthrough_posix_link (old_path, new_path);
}

// unlink call. (...)
int unlink (const char* path)
{
    return (posix_metadata_calls.paddl_intercept_unlink)
        ? m_ld_preloaded_posix.ld_preloaded_posix_unlink (path)
        : m_posix_passthrough.passthrough_posix_unlink (path);
}

// linkat call. (...)
int linkat (int olddirfd, const char* old_path, int newdirfd, const char* new_path, int flags)
{
    return (posix_metadata_calls.paddl_intercept_linkat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_linkat (olddirfd, old_path, newdirfd, new_path, flags)
        : m_posix_passthrough.passthrough_posix_linkat (olddirfd, old_path, newdirfd, new_path, flags);
}

// unlinkat call. (...)
int unlinkat (int dirfd, const char* pathname, int flags)
{
    return (posix_metadata_calls.paddl_intercept_unlinkat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_unlinkat (dirfd, pathname, flags)
        : m_posix_passthrough.passthrough_posix_unlinkat (dirfd, pathname, flags);
}

// rename call. (...)
int rename (const char* old_path, const char* new_path)
{
    return (posix_metadata_calls.paddl_intercept_rename)
        ? m_ld_preloaded_posix.ld_preloaded_posix_rename (old_path, new_path)
        : m_posix_passthrough.passthrough_posix_rename (old_path, new_path);
}

// renameat call. (...)
int renameat (int olddirfd, const char* old_path, int newdirfd, const char* new_path)
{
    return (posix_metadata_calls.paddl_intercept_renameat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_renameat (olddirfd, old_path, newdirfd, new_path)
        : m_posix_passthrough.passthrough_posix_renameat (olddirfd, old_path, newdirfd, new_path);
}

// symlink call. (...)
int symlink (const char* target, const char* linkpath)
{
    return (posix_metadata_calls.paddl_intercept_symlink)
        ? m_ld_preloaded_posix.ld_preloaded_posix_symlink (target, linkpath)
        : m_posix_passthrough.passthrough_posix_symlink (target, linkpath);
}

// symlinkat call. (...)
int symlinkat (const char* target, int newdirfd, const char* linkpath)
{
    return (posix_metadata_calls.paddl_intercept_symlinkat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_symlinkat (target, newdirfd, linkpath)
        : m_posix_passthrough.passthrough_posix_symlinkat (target, newdirfd, linkpath);
}

// readlink call. (...)
ssize_t readlink (const char* path, char* buf, size_t bufsize)
{
    return (posix_metadata_calls.paddl_intercept_readlink)
        ? m_ld_preloaded_posix.ld_preloaded_posix_readlink (path, buf, bufsize)
        : m_posix_passthrough.passthrough_posix_readlink (path, buf, bufsize);
}

// readlinkat call. (...)
ssize_t readlinkat (int dirfd, const char* path, char* buf, size_t bufsize)
{
    return (posix_metadata_calls.paddl_intercept_readlinkat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_readlinkat (dirfd, path, buf, bufsize)
        : m_posix_passthrough.passthrough_posix_readlinkat (dirfd, path, buf, bufsize);
}

// fopen call. (...)
FILE* fopen (const char* pathname, const char* mode)
{
    return (posix_metadata_calls.paddl_intercept_fopen)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fopen (pathname, mode)
        : m_posix_passthrough.passthrough_posix_fopen (pathname, mode);
}

// fopen64 call. (...)
FILE* fopen64 (const char* pathname, const char* mode)
{
    return (posix_metadata_calls.paddl_intercept_fopen64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fopen64 (pathname, mode)
        : m_posix_passthrough.passthrough_posix_fopen64 (pathname, mode);
}

// fdopen call. (...)
FILE* fdopen (int fd, const char* mode)
{
    return (posix_metadata_calls.paddl_intercept_fdopen)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fdopen (fd, mode)
        : m_posix_passthrough.passthrough_posix_fdopen (fd, mode);
}

// freopen call. (...)
FILE* freopen (const char* pathname, const char* mode, FILE* stream)
{
    return (posix_metadata_calls.paddl_intercept_freopen)
        ? m_ld_preloaded_posix.ld_preloaded_posix_freopen (pathname, mode, stream)
        : m_posix_passthrough.passthrough_posix_freopen (pathname, mode, stream);
}

// freopen64 call. (...)
FILE* freopen64 (const char* pathname, const char* mode, FILE* stream)
{
    return (posix_metadata_calls.paddl_intercept_freopen64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_freopen64 (pathname, mode, stream)
        : m_posix_passthrough.passthrough_posix_freopen64 (pathname, mode, stream);
}

// fclose call. (...)
int fclose (FILE* stream)
{
    return (posix_metadata_calls.paddl_intercept_fclose)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fclose (stream)
        : m_posix_passthrough.passthrough_posix_fclose (stream);
}

// fflush call. (...)
int fflush (FILE* stream)
{
    return (posix_metadata_calls.paddl_intercept_fflush)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fflush (stream)
        : m_posix_passthrough.passthrough_posix_fflush (stream);
}

// access call. (...)
int access (const char* path, int mode)
{
    return (posix_metadata_calls.paddl_intercept_access)
        ? m_ld_preloaded_posix.ld_preloaded_posix_access (path, mode)
        : m_posix_passthrough.passthrough_posix_access (path, mode);
}

// faccessat call. (...)
int faccessat (int dirfd, const char* path, int mode, int flags)
{
    return (posix_metadata_calls.paddl_intercept_faccessat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_faccessat (dirfd, path, mode, flags)
        : m_posix_passthrough.passthrough_posix_faccessat (dirfd, path, mode, flags);
}

// lseek call. (...)
off_t lseek (int fd, off_t offset, int whence)
{
    return (posix_metadata_calls.paddl_intercept_lseek)
        ? m_ld_preloaded_posix.ld_preloaded_posix_lseek (fd, offset, whence)
        : m_posix_passthrough.passthrough_posix_lseek (fd, offset, whence);
}

// fseek call. (...)
int fseek (FILE* stream, long offset, int whence)
{
    return (posix_metadata_calls.paddl_intercept_fseek)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fseek (stream, offset, whence)
        : m_posix_passthrough.passthrough_posix_fseek (stream, offset, whence);
}

// ftell call. (...)
long ftell (FILE* stream)
{
    return (posix_metadata_calls.paddl_intercept_ftell)
        ? m_ld_preloaded_posix.ld_preloaded_posix_ftell (stream)
        : m_posix_passthrough.passthrough_posix_ftell (stream);
}

// lseek64 call. (...)
off_t lseek64 (int fd, off_t offset, int whence)
{
    return (posix_metadata_calls.paddl_intercept_lseek64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_lseek64 (fd, offset, whence)
        : m_posix_passthrough.passthrough_posix_lseek64 (fd, offset, whence);
}

// fseeko64 call. (...)
int fseeko64 (FILE* stream, off_t offset, int whence)
{
    return (posix_metadata_calls.paddl_intercept_fseeko64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fseeko64 (stream, offset, whence)
        : m_posix_passthrough.passthrough_posix_fseeko64 (stream, offset, whence);
}

// ftello64 call. (...)
off_t ftello64 (FILE* stream)
{
    return (posix_metadata_calls.paddl_intercept_ftello64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_ftello64 (stream)
        : m_posix_passthrough.passthrough_posix_ftello64 (stream);
}

// mkdir call. (...)
int mkdir (const char* path, mode_t mode)
{
    return (posix_directory_calls.padll_intercept_mkdir)
        ? m_ld_preloaded_posix.ld_preloaded_posix_mkdir (path, mode)
        : m_posix_passthrough.passthrough_posix_mkdir (path, mode);
}

// mkdirat call. (...)
int mkdirat (int dirfd, const char* path, mode_t mode)
{
    return (posix_directory_calls.padll_intercept_mkdirat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_mkdirat (dirfd, path, mode)
        : m_posix_passthrough.passthrough_posix_mkdirat (dirfd, path, mode);
}

// readdir call. (...)
struct dirent* readdir (DIR* dirp)
{
    return (posix_directory_calls.padll_intercept_readdir)
        ? m_ld_preloaded_posix.ld_preloaded_posix_readdir (dirp)
        : m_posix_passthrough.passthrough_posix_readdir (dirp);
}

// readdir64 call. (...)
struct dirent64* readdir64 (DIR* dirp)
{
#if defined(__unix__) || defined(__linux)
    return (posix_directory_calls.padll_intercept_readdir64)
        ? m_ld_preloaded_posix.ld_preloaded_posix_readdir64 (dirp)
        : m_posix_passthrough.passthrough_posix_readdir64 (dirp);
#endif
    return nullptr;
}

// opendir call. (...)
DIR* opendir (const char* path)
{
    return (posix_directory_calls.padll_intercept_opendir)
        ? m_ld_preloaded_posix.ld_preloaded_posix_opendir (path)
        : m_posix_passthrough.passthrough_posix_opendir (path);
}

// fdopendir call. (...)
DIR* fdopendir (int fd)
{
    return (posix_directory_calls.padll_intercept_fdopendir)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fdopendir (fd)
        : m_posix_passthrough.passthrough_posix_fdopendir (fd);
}

// closedir call. (...)
int closedir (DIR* dirp)
{
    return (posix_directory_calls.padll_intercept_closedir)
        ? m_ld_preloaded_posix.ld_preloaded_posix_closedir (dirp)
        : m_posix_passthrough.passthrough_posix_closedir (dirp);
}

// rmdir call. (...)
int rmdir (const char* path)
{
    return (posix_directory_calls.padll_intercept_rmdir)
        ? m_ld_preloaded_posix.ld_preloaded_posix_rmdir (path)
        : m_posix_passthrough.passthrough_posix_rmdir (path);
}

// dirfd call. (...)
int dirfd (DIR* dirp)
{
    return (posix_directory_calls.padll_intercept_dirfd)
        ? m_ld_preloaded_posix.ld_preloaded_posix_dirfd (dirp)
        : m_posix_passthrough.passthrough_posix_dirfd (dirp);
}

// getxattr call. (...)
ssize_t getxattr (const char* path, const char* name, void* value, size_t size)
{
    return (posix_extended_attributes_calls.padll_intercept_getxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_getxattr (path, name, value, size)
        : m_posix_passthrough.passthrough_posix_getxattr (path, name, value, size);
}

// lgetxattr call. (...)
ssize_t lgetxattr (const char* path, const char* name, void* value, size_t size)
{
    return (posix_extended_attributes_calls.padll_intercept_lgetxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_lgetxattr (path, name, value, size)
        : m_posix_passthrough.passthrough_posix_lgetxattr (path, name, value, size);
}

// fgetxattr call. (...)
ssize_t fgetxattr (int fd, const char* name, void* value, size_t size)
{
    return (posix_extended_attributes_calls.padll_intercept_fgetxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fgetxattr (fd, name, value, size)
        : m_posix_passthrough.passthrough_posix_fgetxattr (fd, name, value, size);
}

// setxattr call. (...)
int setxattr (const char* path, const char* name, const void* value, size_t size, int flags)
{
    return (posix_extended_attributes_calls.padll_intercept_setxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_setxattr (path, name, value, size, flags)
        : m_posix_passthrough.passthrough_posix_setxattr (path, name, value, size, flags);
}

// lsetxattr call. (...)
int lsetxattr (const char* path, const char* name, const void* value, size_t size, int flags)
{
    return (posix_extended_attributes_calls.padll_intercept_lsetxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_lsetxattr (path, name, value, size, flags)
        : m_posix_passthrough.passthrough_posix_lsetxattr (path, name, value, size, flags);
}

// fsetxaatr call. (...)
int fsetxattr (int fd, const char* name, const void* value, size_t size, int flags)
{
    return (posix_extended_attributes_calls.padll_intercept_fsetxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fsetxattr (fd, name, value, size, flags)
        : m_posix_passthrough.passthrough_posix_fsetxattr (fd, name, value, size, flags);
}

// listxattr call. (...)
ssize_t listxattr (const char* path, char* list, size_t size)
{
    return (posix_extended_attributes_calls.padll_intercept_listxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_listxattr (path, list, size)
        : m_posix_passthrough.passthrough_posix_listxattr (path, list, size);
}

// llistxattr call. (...)
ssize_t llistxattr (const char* path, char* list, size_t size)
{
    return (posix_extended_attributes_calls.padll_intercept_llistxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_llistxattr (path, list, size)
        : m_posix_passthrough.passthrough_posix_llistxattr (path, list, size);
}

// flistxattr call. (...)
ssize_t flistxattr (int fd, char* list, size_t size)
{
    return (posix_extended_attributes_calls.padll_intercept_flistxattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_flistxattr (fd, list, size)
        : m_posix_passthrough.passthrough_posix_flistxattr (fd, list, size);
}

// removexattr call. (...)
int removexattr (const char* path, const char* name)
{
    return (posix_extended_attributes_calls.padll_intercept_removexattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_removexattr (path, name)
        : m_posix_passthrough.passthrough_posix_removexattr (path, name);
}

// lremovexattr call. (...)
int lremovexattr (const char* path, const char* name)
{
    return (posix_extended_attributes_calls.padll_intercept_lremovexattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_lremovexattr (path, name)
        : m_posix_passthrough.passthrough_posix_lremovexattr (path, name);
}

// fremovexattr call. (...)
int fremovexattr (int fd, const char* name)
{
    return (posix_extended_attributes_calls.padll_intercept_fremovexattr)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fremovexattr (fd, name)
        : m_posix_passthrough.passthrough_posix_fremovexattr (fd, name);
}

// chmod call. (...)
int chmod (const char* pathname, mode_t mode)
{
    return (posix_file_modes_calls.padll_intercept_chmod)
        ? m_ld_preloaded_posix.ld_preloaded_posix_chmod (pathname, mode)
        : m_posix_passthrough.passthrough_posix_chmod (pathname, mode);
}

// fchmod call. (...)
int fchmod (int fd, mode_t mode)
{
    return (posix_file_modes_calls.padll_intercept_fchmod)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fchmod (fd, mode)
        : m_posix_passthrough.passthrough_posix_fchmod (fd, mode);
}

// fchmodat call. (...)
int fchmodat (int dirfd, const char* pathname, mode_t mode, int flags)
{
    return (posix_file_modes_calls.padll_intercept_fchmod)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fchmodat (dirfd, pathname, mode, flags)
        : m_posix_passthrough.passthrough_posix_fchmodat (dirfd, pathname, mode, flags);
}

// chown call. (...)
int chown (const char* pathname, uid_t owner, gid_t group)
{
    return (posix_file_modes_calls.padll_intercept_chown)
        ? m_ld_preloaded_posix.ld_preloaded_posix_chown (pathname, owner, group)
        : m_posix_passthrough.passthrough_posix_chown (pathname, owner, group);
}

// lchown call. (...)
int lchown (const char* pathname, uid_t owner, gid_t group)
{
    return (posix_file_modes_calls.padll_intercept_chown)
        ? m_ld_preloaded_posix.ld_preloaded_posix_lchown (pathname, owner, group)
        : m_posix_passthrough.passthrough_posix_lchown (pathname, owner, group);
}

// fchown call. (...)
int fchown (int fd, uid_t owner, gid_t group)
{
    return (posix_file_modes_calls.padll_intercept_fchown)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fchown (fd, owner, group)
        : m_posix_passthrough.passthrough_posix_fchown (fd, owner, group);
}

// fchownat call. (...)
int fchownat (int dirfd, const char* pathname, uid_t owner, gid_t group, int flags)
{
    return (posix_file_modes_calls.padll_intercept_fchownat)
        ? m_ld_preloaded_posix.ld_preloaded_posix_fchownat (dirfd, pathname, owner, group, flags)
        : m_posix_passthrough.passthrough_posix_fchownat (dirfd, pathname, owner, group, flags);
}
