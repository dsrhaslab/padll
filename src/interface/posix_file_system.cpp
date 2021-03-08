/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <ldpaio/interface/posix_file_system.hpp>

// read call. (...)
ssize_t read (int fd, void* buf, size_t size)
{
    return m_posix_passthrough.passthrough_read (fd, buf, size);
}

// write call. (...)
ssize_t write (int fd, const void* buf, size_t size)
{
    return m_posix_passthrough.passthrough_write (fd, buf, size);
}

// pread call. (...)
ssize_t pread (int fd, void* buf, size_t size, off_t offset)
{
    return m_posix_passthrough.passthrough_pread (fd, buf, size, offset);
}

// pwrite call. (...)
ssize_t pwrite (int fd, const void* buf, size_t size, off_t offset)
{
    return m_posix_passthrough.passthrough_pwrite (fd, buf, size, offset);
}

// open call. (...)
int open (const char* path, int flags, ...)
{
    if (flags & O_CREAT) {
        va_list args;

        va_start (args, flags);
        mode_t mode = va_arg (args, int);
        va_end (args);

        return m_posix_passthrough.passthrough_open (path, flags, mode);
    } else {
        return m_posix_passthrough.passthrough_open (path, flags);
    }
}

// creat call. (...)
int creat (const char* path, mode_t mode)
{
    return m_posix_passthrough.passthrough_creat (path, mode);
}

// openat call. (...)
int openat (int dirfd, const char* path, int flags, ...)
{
    if (flags & O_CREAT) {
        va_list args;

        va_start (args, flags);
        mode_t mode = va_arg (args, int);
        va_end (args);

        return m_posix_passthrough.passthrough_openat (dirfd, path, flags, mode);
    } else {
        return m_posix_passthrough.passthrough_openat (dirfd, path, flags);
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

        return m_posix_passthrough.passthrough_open64 (path, flags, mode);
    } else {
        return m_posix_passthrough.passthrough_open64 (path, flags);
    }
}

// close call. (...)
int close (int fd)
{
    return m_posix_passthrough.passthrough_close (fd);
}

// fsync call. (...)
int fsync (int fd)
{
    return m_posix_passthrough.passthrough_fsync (fd);
}

// fdatasync call. (...)
int fdatasync (int fd)
{
    return m_posix_passthrough.passthrough_fdatasync (fd);
}

// truncate call. (...)
int truncate (const char* path, off_t length)
{
    return m_posix_passthrough.passthrough_truncate (path, length);
}

// ftruncate call. (...)
int ftruncate (int fd, off_t length)
{
    return m_posix_passthrough.passthrough_ftruncate (fd, length);
}

// stat call. (...)
int stat (const char* path, struct stat* statbuf)
{
    return m_posix_passthrough.passthrough_stat (path, statbuf);
}

// fstat call. (...)
int fstat (int fd, struct stat* statbuf)
{
    return m_posix_passthrough.passthrough_fstat (fd, statbuf);
}

// link call. (...)
int link (const char* old_path, const char* new_path)
{
    return m_posix_passthrough.passthrough_link (old_path, new_path);
}

// unlink call. (...)
int unlink (const char* path)
{
    return m_posix_passthrough.passthrough_unlink (path);
}

// linkat call. (...)
int linkat (int olddirfd, const char* old_path, int newdirfd, const char* new_path, int flags)
{
    return m_posix_passthrough.passthrough_linkat (olddirfd, old_path, newdirfd, new_path, flags);
}

// unlinkat call. (...)
int unlinkat (int dirfd, const char* pathname, int flags)
{
    return m_posix_passthrough.passthrough_unlinkat (dirfd, pathname, flags);
}

// rename call. (...)
int rename (const char* old_path, const char* new_path)
{
    return m_posix_passthrough.passthrough_rename (old_path, new_path);
}

// mkdir call. (...)
int mkdir (const char* path)
{
    return m_posix_passthrough.passthrough_mkdir (path);
}

// readdir call. (...)
struct dirent* readdir (DIR* dirp)
{
    return m_posix_passthrough.passthrough_readdir (dirp);
}

// opendir call. (...)
DIR* opendir (const char* path)
{
    return m_posix_passthrough.passthrough_opendir (path);
}

// closedir call. (...)
int closedir (DIR* dirp)
{
    return m_posix_passthrough.passthrough_closedir (dirp);
}

// rmdir call. (...)
int rmdir (const char* path)
{
    return m_posix_passthrough.passthrough_rmdir (path);
}

// getxattr call. (...)
ssize_t getxattr (const char* path, const char* name, void* value, size_t size)
{
    return m_posix_passthrough.passthrough_getxattr (path, name, value, size);
}

// fgetxattr call. (...)
ssize_t fgetxattr (int fd, const char* name, void* value, size_t size)
{
    return m_posix_passthrough.passthrough_fgetxattr (fd, name, value, size);
}

// setxattr call. (...)
int setxattr (const char* path, const char* name, const void* value, size_t size, int flags)
{
    return m_posix_passthrough.passthrough_setxattr (path, name, value, size, flags);
}

// fsetxaatr call. (...)
int fsetxattr (int fd, const char* name, const void* value, size_t size, int flags)
{
    return m_posix_passthrough.passthrough_fsetxattr (fd, name, value, size, flags);
}

// fread call. (...)
size_t fread (void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    return m_posix_passthrough.passthrough_fread (ptr, size, nmemb, stream);
}