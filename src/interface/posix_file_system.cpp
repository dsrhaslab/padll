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

// fread call. (...)
size_t fread (void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    return m_posix_passthrough.passthrough_fread (ptr, size, nmemb, stream);
}

// fwrite call. (...)
size_t fwrite (const void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    return m_posix_passthrough.passthrough_fwrite (ptr, size, nmemb, stream);
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

// sync call. (...)
void sync ()
{
    return m_posix_passthrough.passthrough_sync ();
}

// syncfs call. (...)
int syncfs (int fd)
{
    return m_posix_passthrough.passthrough_syncfs (fd);
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

// lstat call. (...)
int lstat (const char* path, struct stat* statbuf)
{
    return m_posix_passthrough.passthrough_lstat (path, statbuf);
}

// fstat call. (...)
int fstat (int fd, struct stat* statbuf)
{
    return m_posix_passthrough.passthrough_fstat (fd, statbuf);
}

// fstatat call. (...)
int fstatat (int dirfd, const char* path, struct stat* statbuf, int flags)
{
    return m_posix_passthrough.passthrough_fstatat (dirfd, path, statbuf, flags);
}

// statfs call. (...)
int statfs (const char* path, struct statfs* buf)
{
    return m_posix_passthrough.passthrough_statfs (path, buf);
}

// fstatfs call. (...)
int fstatfs (int fd, struct statfs* buf)
{
    return m_posix_passthrough.passthrough_fstatfs (fd, buf);
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

// renameat call. (...)
int renameat (int olddirfd, const char* old_path, int newdirfd, const char* new_path)
{
    return m_posix_passthrough.passthrough_renameat (olddirfd, old_path, newdirfd, new_path);
}

// symlink call. (...)
int symlink (const char* target, const char* linkpath)
{
    return m_posix_passthrough.passthrough_symlink (target, linkpath);
}

// symlinkat call. (...)
int symlinkat (const char* target, int newdirfd, const char* linkpath)
{
    return m_posix_passthrough.passthrough_symlinkat (target, newdirfd, linkpath);
}

// readlink call. (...)
ssize_t readlink (const char* path, char* buf, size_t bufsize)
{
    return m_posix_passthrough.passthrough_readlink (path, buf, bufsize);
}

// readlinkat call. (...)
ssize_t readlinkat (int dirfd, const char* path, char* buf, size_t bufsize)
{
    return m_posix_passthrough.passthrough_readlinkat (dirfd, path, buf, bufsize);
}

// mkdir call. (...)
int mkdir (const char* path, mode_t mode)
{
    return m_posix_passthrough.passthrough_mkdir (path, mode);
}

// mkdirat call. (...)
int mkdirat (int dirfd, const char* path, mode_t mode)
{
    return m_posix_passthrough.passthrough_mkdirat (dirfd, path, mode);
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

// fdopendir call. (...)
DIR* fdopendir (int fd)
{
    return m_posix_passthrough.passthrough_fdopendir (fd);
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

// dirfd call. (...)
int dirfd (DIR* dirp)
{
    return m_posix_passthrough.passthrough_dirfd (dirp);
}

// getxattr call. (...)
ssize_t getxattr (const char* path, const char* name, void* value, size_t size)
{
    return m_posix_passthrough.passthrough_getxattr (path, name, value, size);
}

// lgetxattr call. (...)
ssize_t lgetxattr (const char* path, const char* name, void* value, size_t size)
{
    return m_posix_passthrough.passthrough_lgetxattr (path, name, value, size);
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

// lsetxattr call. (...)
int lsetxattr (const char* path, const char* name, const void* value, size_t size, int flags)
{
    return m_posix_passthrough.passthrough_lsetxattr (path, name, value, size, flags);
}

// fsetxaatr call. (...)
int fsetxattr (int fd, const char* name, const void* value, size_t size, int flags)
{
    return m_posix_passthrough.passthrough_fsetxattr (fd, name, value, size, flags);
}

// listxattr call. (...)
ssize_t listxattr (const char* path, char* list, size_t size)
{
    return m_posix_passthrough.passthrough_listxattr (path, list, size);
}

// llistxattr call. (...)
ssize_t llistxattr (const char* path, char* list, size_t size)
{
    return m_posix_passthrough.passthrough_llistxattr (path, list, size);
}

// flistxattr call. (...)
ssize_t flistxattr (int fd, char* list, size_t size)
{
    return m_posix_passthrough.passthrough_flistxattr (fd, list, size);
}

// removexattr call. (...)
int removexattr (const char* path, const char* name)
{
    return m_posix_passthrough.passthrough_removexattr (path, name);
}

// lremovexattr call. (...)
int lremovexattr (const char* path, const char* name)
{
    return m_posix_passthrough.passthrough_lremovexattr (path, name);
}

// fremovexattr call. (...)
int fremovexattr (int fd, const char* name)
{
    return m_posix_passthrough.passthrough_fremovexattr (fd, name);
}

// chmod call. (...)
int chmod (const char* pathname, mode_t mode)
{
    return m_posix_passthrough.passthrough_chmod (pathname, mode);
}

// fchmod call. (...)
int fchmod (int fd, mode_t mode)
{
    return m_posix_passthrough.passthrough_fchmod (fd, mode);
}

// fchmodat call. (...)
int fchmodat (int dirfd, const char* pathname, mode_t mode, int flags)
{
    return m_posix_passthrough.passthrough_fchmodat (dirfd, pathname, mode, flags);
}

// chown call. (...)
int chown (const char* pathname, uid_t owner, gid_t group)
{
    return m_posix_passthrough.passthrough_chown (pathname, owner, group);
}

// lchown call. (...)
int lchown (const char* pathname, uid_t owner, gid_t group)
{
    return m_posix_passthrough.passthrough_lchown (pathname, owner, group);
}

// fchown call. (...)
int fchown (int fd, uid_t owner, gid_t group)
{
    return m_posix_passthrough.passthrough_fchown (fd, owner, group);
}

// fchownat call. (...)
int fchownat (int dirfd, const char* pathname, uid_t owner, gid_t group, int flags)
{
    return m_posix_passthrough.passthrough_fchownat (dirfd, pathname, owner, group, flags);
}