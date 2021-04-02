/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef PADLL_POSIX_PASSTHROUGH_HPP
#define PADLL_POSIX_PASSTHROUGH_HPP

#define _GNU_SOURCE 1

#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <iostream>
#include <padll/libraries/libc_operation_enums.hpp>
#include <padll/libraries/libc_operation_headers.hpp>
#include <padll/statistics/statistics.hpp>
#include <padll/utils/options.hpp>
#include <sstream>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace padll {

/**
 * PosixPassthrough class.
 *  https://www.gnu.org/software/libc/manual/html_node/Function-Index.html
 * missing: Complete me ...
 */
class PosixPassthrough {

private:
    std::mutex m_lock;
    libc_metadata m_metadata_operations {};
    libc_data m_data_operations {};
    libc_directory m_directory_operations {};
    libc_extattr m_extattr_operations {};
    libc_file_modes m_filemodes_operations {};
    std::string m_lib_name { "libc.so.6" };
    void* m_lib_handle { nullptr };
    std::atomic<bool> m_collect { option_default_statistic_collection };
    Statistics m_metadata_stats { "metadata", OperationType::metadata_calls };
    Statistics m_data_stats { "data", OperationType::data_calls };
    Statistics m_dir_stats { "directory", OperationType::directory_calls };
    Statistics m_ext_attr_stats { "ext-attr", OperationType::ext_attr_calls };
    Statistics m_file_mode_stats { "file-mode", OperationType::file_mode_calls };

    /**
     * initialize:
     */
    void initialize ();

    /**
     * dlopen_library_handle:
     * @return
     */
    bool dlopen_library_handle ();

public:
    /**
     * PosixPassthrough default constructor.
     */
    PosixPassthrough ();

    /**
     * PosixPassthrough parameterized constructor.
     * @param lib String that respects to the library that will be intercepted.
     * @param stat_collection Boolean that defines if statistic collection is enabled or disabled.
     */
    PosixPassthrough (const std::string& lib, bool stat_collection);

    /**
     * PosixPassthrough default destructor.
     */
    ~PosixPassthrough ();

    /**
     * get_statistic_entry:
     * @param operation_type
     * @param operation_entry
     * @return
     */
    StatisticEntry get_statistic_entry (const OperationType& operation_type,
        const int& operation_entry);

    /**
     * set_statistic_collection:
     * @param value
     * @return
     */
    void set_statistic_collection (bool value);

    /**
     * to_string:
     * @return
     */
    std::string to_string ();

    /**
     * passthrough_read:
     *  https://linux.die.net/man/2/read
     * @param fd
     * @param buf
     * @param counter
     * @return
     */
    ssize_t passthrough_read (int fd, void* buf, ssize_t counter);

    /**
     * passthrough_write:
     *  https://linux.die.net/man/2/write
     * @param fd
     * @param buf
     * @param counter
     * @return
     */
    ssize_t passthrough_write (int fd, const void* buf, ssize_t counter);

    /**
     * passthrough_pread:
     *  https://linux.die.net/man/2/pread
     * @param fd
     * @param buf
     * @param counter
     * @param offset
     * @return
     */
    ssize_t passthrough_pread (int fd, void* buf, ssize_t counter, off_t offset);

    /**
     * passthrough_pwrite:
     *  https://linux.die.net/man/2/pwrite
     * @param fd
     * @param buf
     * @param counter
     * @param offset
     * @return
     */
    ssize_t passthrough_pwrite (int fd, const void* buf, ssize_t counter, off_t offset);

    /**
     * passthrough_pread64:
     *  https://linux.die.net/man/2/pread64
     *  https://www.mkssoftware.com/docs/man3/read.3.asp
     * @param fd
     * @param buf
     * @param counter
     * @param offset
     * @return
     */
#if defined(__USE_LARGEFILE64)
    ssize_t passthrough_pread64 (int fd, void* buf, ssize_t counter, off64_t offset);
#endif

    /**
     * passthrough_pwrite:
     *  https://linux.die.net/man/2/pwrite64
     *  https://www.mkssoftware.com/docs/man3/write.3.asp
     * @param fd
     * @param buf
     * @param counter
     * @param offset
     * @return
     */
#if defined(__USE_LARGEFILE64)
    ssize_t passthrough_pwrite64 (int fd, const void* buf, ssize_t counter, off64_t offset);
#endif

    /**
     * passthrough_fread:
     *  https://linux.die.net/man/3/fread
     * @param ptr
     * @param size
     * @param nmemb
     * @param stream
     * @return
     */
    size_t passthrough_fread (void* ptr, size_t size, size_t nmemb, FILE* stream);

    /**
     * passthrough_fwrite:
     *  https://linux.die.net/man/3/fwrite
     * @param ptr
     * @param size
     * @param nmemb
     * @param stream
     * @return
     */
    size_t passthrough_fwrite (const void* ptr, size_t size, size_t nmemb, FILE* stream);

    /**
     * passthrough_open:
     *  https://linux.die.net/man/2/open
     * Notes:
     *  https://github.com/fritzw/ld-preload-open/blob/master/path-mapping.c
     *  https://github.com/poliva/ldpreloadhook/blob/master/hook.c
     * @param path
     * @param flags
     * @param mode
     * @return
     */
    int passthrough_open (const char* path, int flags, mode_t mode);

    /**
     * passthrough_open:
     *  https://linux.die.net/man/2/open
     * @param path
     * @param flags
     * @return
     */
    int passthrough_open (const char* path, int flags);

    /**
     * passthrough_creat:
     *  https://linux.die.net/man/2/creat
     * @param path
     * @param mode
     * @return
     */
    int passthrough_creat (const char* path, mode_t mode);

    /**
     * passthrough_creat64:
     *  https://www.unix.com/man-page/hpux/2/creat64/
     * @param path
     * @param mode
     * @return
     */
    int passthrough_creat64 (const char* path, mode_t mode);

    /**
     * passthrough_openat:
     *  https://linux.die.net/man/2/openat
     * @param dirfd
     * @param path
     * @param flags
     * @param mode
     * @return
     */
    int passthrough_openat (int dirfd, const char* path, int flags, mode_t mode);

    /**
     * passthrough_openat:
     *  https://linux.die.net/man/2/openat
     * @param dirfd
     * @param path
     * @param flags
     * @return
     */
    int passthrough_openat (int dirfd, const char* path, int flags);

    /**
     * passthrough_open64:
     *  https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/open64.c.html
     * @param path
     * @param flags
     * @param mode
     * @return
     */
    int passthrough_open64 (const char* path, int flags, mode_t mode);

    /**
     * passthrough_open64:
     *  https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/open64.c.html
     * @param path
     * @param flags
     * @return
     */
    int passthrough_open64 (const char* path, int flags);

    /**
     * passthrough_close:
     *  https://linux.die.net/man/2/close
     * @param fd
     * @return
     */
    int passthrough_close (int fd);

    /**
     * passthrough_fsync:
     *  https://linux.die.net/man/2/fsync
     * @param fd
     * @return
     */
    int passthrough_fsync (int fd);

    /**
     * passthrough_fdatasync:
     *  https://linux.die.net/man/2/fdatasync
     * @param fd
     * @return
     */
    int passthrough_fdatasync (int fd);

    /**
     * passthrough_sync:
     *  https://linux.die.net/man/2/sync
     */
    void passthrough_sync ();

    /**
     * passthrough_syncfs:
     *  https://linux.die.net/man/2/syncfs
     * @param fd
     * @return
     */
    int passthrough_syncfs (int fd);

    /**
     * passthrough_truncate:
     *  https://linux.die.net/man/2/truncate
     * @param path
     * @param length
     * @return
     */
    int passthrough_truncate (const char* path, off_t length);

    /**
     * passthrough_ftruncate:
     *  https://linux.die.net/man/2/ftruncate
     * @param fd
     * @param length
     * @return
     */
    int passthrough_ftruncate (int fd, off_t length);

    /**
     * passthrough_truncate64:
     *  https://linux.die.net/man/2/truncate64
     *  https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/truncate64.c.html
     * @param path
     * @param length
     * @return
     */
    int passthrough_truncate64 (const char* path, off_t length);

    /**
     * passthrough_ftruncate64:
     *  https://linux.die.net/man/2/ftruncate64
     *  https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/ftruncate64.c.html
     * @param fd
     * @param length
     * @return
     */
    int passthrough_ftruncate64 (int fd, off_t length);

    /**
     * passthrough_stat:
     *  This method respects to the stat call.
     *  https://refspecs.linuxbase.org/LSB_3.0.0/LSB-PDA/LSB-PDA/baselib-xstat-1.html
     *  https://linux.die.net/man/2/stat
     * @param version
     * @param path
     * @param statbuf
     * @return
     */
    int passthrough_xstat (int version, const char* path, struct stat* statbuf);

    /**
     * passthrough_lxstat:
     *  https://linux.die.net/man/2/lstat
     * @param version
     * @param path
     * @param statbuf
     * @return
     */
    int passthrough_lxstat (int version, const char* path, struct stat* statbuf);

    /**
     * passthrough_fxstat:
     *  https://linux.die.net/man/2/fstat
     * @param version
     * @param fd
     * @param statbuf
     * @return
     */
    int passthrough_fxstat (int version, int fd, struct stat* statbuf);

    /**
     * passthrough_fxstatat:
     *  https://refspecs.linuxbase.org/LSB_5.0.0/LSB-Core-generic/LSB-Core-generic/baselib---fxstatat-1.html
     *  https://linux.die.net/man/2/fstatat
     * @param version
     * @param dirfd
     * @param path
     * @param statbuf
     * @param flags
     * @return
     */
    int passthrough_fxstatat (int version,
        int dirfd,
        const char* path,
        struct stat* statbuf,
        int flags);

    /**
     * passthrough_stat64:
     *  This method respects to the stat call.
     *  https://refspecs.linuxbase.org/LSB_3.0.0/LSB-PDA/LSB-PDA/baselib-xstat-1.html
     *  https://linux.die.net/man/2/stat64
     * @param version
     * @param path
     * @param statbuf
     * @return
     */
    int passthrough_xstat64 (int version, const char* path, struct stat64* statbuf);

    /**
     * passthrough_lxstat64:
     *  https://linux.die.net/man/2/lstat64
     *  https://refspecs.linuxbase.org/LSB_3.0.0/LSB-PDA/LSB-PDA/baselib-xstat64-1.html
     * @param version
     * @param path
     * @param statbuf
     * @return
     */
    int passthrough_lxstat64 (int version, const char* path, struct stat64* statbuf);

    /**
     * passthrough_fxstat64:
     *  https://linux.die.net/man/2/fstat64
     *  https://refspecs.linuxbase.org/LSB_3.0.0/LSB-PDA/LSB-PDA/baselib-xstat64-1.html
     * @param version
     * @param fd
     * @param statbuf
     * @return
     */
    int passthrough_fxstat64 (int version, int fd, struct stat64* statbuf);

    /**
     * passthrough_fxstatat64:
     *  https://refspecs.linuxbase.org/LSB_5.0.0/LSB-Core-generic/LSB-Core-generic/baselib---fxstatat-1.html
     *  https://linux.die.net/man/2/fstatat
     * @param version
     * @param dirfd
     * @param path
     * @param statbuf
     * @param flags
     * @return
     */
    int passthrough_fxstatat64 (int version,
        int dirfd,
        const char* path,
        struct stat64* statbuf,
        int flags);

    /**
     * passthrough_statfs:
     *  https://linux.die.net/man/2/statfs
     * @param path
     * @param buf
     * @return
     */
    int passthrough_statfs (const char* path, struct statfs* buf);

    /**
     * passthrough_fstatfs
     *  https://linux.die.net/man/2/fstatfs
     * @param fd
     * @param buf
     * @return
     */
    int passthrough_fstatfs (int fd, struct statfs* buf);

    /**
     * passthrough_statfs64:
     *  https://linux.die.net/man/2/statfs64
     *  https://refspecs.linuxbase.org/LSB_5.0.0/LSB-Core-generic/LSB-Core-generic/baselib-statfs64.html
     * @param path
     * @param buf
     * @return
     */
    int passthrough_statfs64 (const char* path, struct statfs64* buf);

    /**
     * passthrough_fstatfs64
     *  https://linux.die.net/man/2/fstatfs64
     *  https://refspecs.linuxbase.org/LSB_5.0.0/LSB-Core-generic/LSB-Core-generic/baselib-fstatfs64.html
     * @param fd
     * @param buf
     * @return
     */
    int passthrough_fstatfs64 (int fd, struct statfs64* buf);

    /**
     * passthrough_link:
     *  https://linux.die.net/man/2/link
     * @param old_path
     * @param new_path
     * @return
     */
    int passthrough_link (const char* old_path, const char* new_path);

    /**
     * passthrough_unlink:
     *  https://linux.die.net/man/2/unlink
     * @param old_path
     * @return
     */
    int passthrough_unlink (const char* old_path);

    /**
     * passthrough_linkat:
     *  https://linux.die.net/man/2/linkat
     * @param olddirfd
     * @param old_path
     * @param newdirfd
     * @param new_path
     * @param flags
     * @return
     */
    int passthrough_linkat (int olddirfd,
        const char* old_path,
        int newdirfd,
        const char* new_path,
        int flags);

    /**
     * passthrough_unlinkat:
     *  https://linux.die.net/man/2/unlinkat
     * @param dirfd
     * @param pathname
     * @param flags
     * @return
     */
    int passthrough_unlinkat (int dirfd, const char* pathname, int flags);

    /**
     * passthrough_rename:
     *  https://linux.die.net/man/2/rename
     * @param old_path
     * @param new_path
     * @return
     */
    int passthrough_rename (const char* old_path, const char* new_path);

    /**
     * passthrough_renameat:
     *  https://linux.die.net/man/2/renameat
     * @param olddirfd
     * @param old_path
     * @param newdirfd
     * @param new_path
     * @return
     */
    int
    passthrough_renameat (int olddirfd, const char* old_path, int newdirfd, const char* new_path);

    /**
     * passthrough_symlink:
     *  https://linux.die.net/man/2/symlink
     * @param target
     * @param linkpath
     * @return
     */
    int passthrough_symlink (const char* target, const char* linkpath);

    /**
     * passthrough_symlinkat:
     *  https://linux.die.net/man/2/symlinkat
     * @param target
     * @param newdirfd
     * @param linkpath
     * @return
     */
    int passthrough_symlinkat (const char* target, int newdirfd, const char* linkpath);

    /**
     * passthrough_readlink:
     *  https://linux.die.net/man/2/readlink
     * @param path
     * @param buf
     * @param bufsize
     * @return
     */
    ssize_t passthrough_readlink (const char* path, char* buf, size_t bufsize);

    /**
     * passthrough_readlinkat:
     *  https://linux.die.net/man/2/readlinkat
     * @param dirfd
     * @param path
     * @param buf
     * @param bufsize
     * @return
     */
    ssize_t passthrough_readlinkat (int dirfd, const char* path, char* buf, size_t bufsize);

    /**
     * passthrough_fopen:
     *  https://linux.die.net/man/3/fopen
     * @param pathname
     * @param mode
     * @return
     */
    FILE* passthrough_fopen (const char* pathname, const char* mode);

    /**
     * passthrough_fopen64:
     *  https://sites.ualberta.ca/dept/chemeng/AIX-43/share/man/info/C/a_doc_lib/libs/basetrf1/fopen.htm
     * @param pathname
     * @param mode
     * @return
     */
    FILE* passthrough_fopen64 (const char* pathname, const char* mode);

    /**
     * passthrough_fdopen:
     *  https://linux.die.net/man/3/fdopen
     * @param fd
     * @param mode
     * @return
     */
    FILE* passthrough_fdopen (int fd, const char* mode);

    /**
     * passthrough_freopen:
     *  https://linux.die.net/man/3/freopen
     * @param pathname
     * @param mode
     * @param stream
     * @return
     */
    FILE* passthrough_freopen (const char* pathname, const char* mode, FILE* stream);

    /**
     * passthrough_freopen64:
     *  https://sites.ualberta.ca/dept/chemeng/AIX-43/share/man/info/C/a_doc_lib/libs/basetrf1/fopen.htm
     * @param pathname
     * @param mode
     * @param stream
     * @return
     */
    FILE* passthrough_freopen64 (const char* pathname, const char* mode, FILE* stream);

    /**
     * passthrough_fclose:
     *  https://linux.die.net/man/3/fclose
     * @param stream
     * @return
     */
    int passthrough_fclose (FILE* stream);

    /**
     * passthrough_fflush:
     *  https://linux.die.net/man/3/fflush
     * @param stream
     * @return
     */
    int passthrough_fflush (FILE* stream);

    /**
     * passthrough_access:
     *  https://linux.die.net/man/2/access
     * @param path
     * @param mode
     * @return
     */
    int passthrough_access (const char* path, int mode);

    /**
     * passthrough_faccessat:
     *  https://linux.die.net/man/2/faccessat
     * @param dirfd
     * @param path
     * @param mode
     * @param flags
     * @return
     */
    int passthrough_faccessat (int dirfd, const char* path, int mode, int flags);

    /**
     * passthrough_lseek:
     *  https://linux.die.net/man/2/lseek
     * @param fd
     * @param offset
     * @param whence
     * @return
     */
    off_t passthrough_lseek (int fd, off_t offset, int whence);

    /**
     * passthrough_fseek:
     *  https://linux.die.net/man/3/fseek
     * @param stream
     * @param offset
     * @param whence
     * @return
     */
    int passthrough_fseek (FILE* stream, long offset, int whence);

    /**
     * passthrough_ftell:
     *  https://linux.die.net/man/3/ftell
     * @param stream
     * @return
     */
    long passthrough_ftell (FILE* stream);

    /**
     * passthrough_lseek64:
     *  https://linux.die.net/man/2/lseek64
     * @param fd
     * @param offset
     * @param whence
     * @return
     */
    off_t passthrough_lseek64 (int fd, off_t offset, int whence);

    /**
     * passthrough_fseeko64:
     * @param stream
     * @param offset
     * @param whence
     * @return
     */
    int passthrough_fseeko64 (FILE* stream, off_t offset, int whence);

    /**
     * passthrough_ftello64:
     * @param stream
     * @return
     */
    off_t passthrough_ftello64 (FILE* stream);

    /**
     * passthrough_mkdir:
     *  https://linux.die.net/man/2/mkdir
     * @param path
     * @param mode
     * @return
     */
    int passthrough_mkdir (const char* path, mode_t mode);

    /**
     * passthrough_mkdirat:
     *  https://linux.die.net/man/2/mkdirat
     * @param dirfd
     * @param path
     * @param mode
     * @return
     */
    int passthrough_mkdirat (int dirfd, const char* path, mode_t mode);

    /**
     * passthrough_readdir:
     *  https://linux.die.net/man/3/readdir
     * @param dirp
     * @return
     */
    struct dirent* passthrough_readdir (DIR* dirp);

    /**
     * passthrough_readdir64:
     *  https://www.mkssoftware.com/docs/man3/readdir.3.asp
     * @param dirp
     * @return
     */
    struct dirent64* passthrough_readdir64 (DIR* dirp);

    /**
     * passthrough_opendir:
     *  https://linux.die.net/man/3/opendir
     * @param path
     * @return
     */
    DIR* passthrough_opendir (const char* path);

    /**
     * passthrough_fdopendir:
     *  https://linux.die.net/man/3/fdopendir
     * @param fd
     * @return
     */
    DIR* passthrough_fdopendir (int fd);

    /**
     * passthrough_closedir:
     *  https://linux.die.net/man/3/closedir
     * @param dirp
     * @return
     */
    int passthrough_closedir (DIR* dirp);

    /**
     * passthrough_rmdir:
     *  https://linux.die.net/man/3/rmdir
     * @param path
     * @return
     */
    int passthrough_rmdir (const char* path);

    /**
     * passthrough_dirfd:
     *  https://linux.die.net/man/3/dirfd
     * @param dirp
     * @return
     */
    int passthrough_dirfd (DIR* dirp);

    /**
     * passthrough_getxattr:
     *  https://linux.die.net/man/2/getxattr
     * @param path
     * @param name
     * @param value
     * @param size
     * @return
     */
    ssize_t passthrough_getxattr (const char* path, const char* name, void* value, size_t size);

    /**
     * passthrough_lgetxattr:
     *  https://linux.die.net/man/2/lgetxattr
     * @param path
     * @param name
     * @param value
     * @param size
     * @return
     */
    ssize_t passthrough_lgetxattr (const char* path, const char* name, void* value, size_t size);

    /**
     * passthrough_fgetxattr:
     *  https://linux.die.net/man/2/fgetxattr
     * @param fd
     * @param name
     * @param value
     * @param size
     * @return
     */
    ssize_t passthrough_fgetxattr (int fd, const char* name, void* value, size_t size);

    /**
     * passthrough_setxattr:
     *  https://linux.die.net/man/2/setxattr
     * @param path
     * @param name
     * @param value
     * @param size
     * @param flags
     * @return
     */
    int passthrough_setxattr (const char* path,
        const char* name,
        const void* value,
        size_t size,
        int flags);

    /**
     * passthrough_lsetxattr:
     *  https://linux.die.net/man/2/lsetxattr
     * @param path
     * @param name
     * @param value
     * @param size
     * @param flags
     * @return
     */
    int passthrough_lsetxattr (const char* path,
        const char* name,
        const void* value,
        size_t size,
        int flags);

    /**
     * passthrough_fsetxattr:
     *  https://linux.die.net/man/2/fsetxattr
     * @param fd
     * @param name
     * @param value
     * @param size
     * @param flags
     * @return
     */
    int passthrough_fsetxattr (int fd, const char* name, const void* value, size_t size, int flags);

    /**
     * passthrough_listxattr:
     *  https://linux.die.net/man/2/listxattr
     * @param path
     * @param list
     * @param size
     * @return
     */
    ssize_t passthrough_listxattr (const char* path, char* list, size_t size);

    /**
     * passthrough_llistxattr:
     *  https://linux.die.net/man/2/llistxattr
     * @param path
     * @param list
     * @param size
     * @return
     */
    ssize_t passthrough_llistxattr (const char* path, char* list, size_t size);

    /**
     * passthrough_flistxattr:
     *  https://linux.die.net/man/2/flistxattr
     * @param fd
     * @param list
     * @param size
     * @return
     */
    ssize_t passthrough_flistxattr (int fd, char* list, size_t size);

    /**
     * passthrough_removexattr:
     *  https://linux.die.net/man/2/removexattr
     * @param path
     * @param name
     * @return
     */
    int passthrough_removexattr (const char* path, const char* name);

    /**
     * passthrough_lremovexattr:
     *  https://linux.die.net/man/2/lremovexattr
     * @param path
     * @param name
     * @return
     */
    int passthrough_lremovexattr (const char* path, const char* name);

    /**
     * passthrough_fremovexattr:
     *  https://linux.die.net/man/2/fremovexattr
     * @param fd
     * @param name
     * @return
     */
    int passthrough_fremovexattr (int fd, const char* name);

    /**
     * passthrough_chmod:
     *  https://linux.die.net/man/2/chmod
     * @param path
     * @param mode
     * @return
     */
    int passthrough_chmod (const char* path, mode_t mode);

    /**
     * fchmod:
     *  https://linux.die.net/man/2/fchmod
     * @param fd
     * @param mode
     * @return
     */
    int passthrough_fchmod (int fd, mode_t mode);

    /**
     * passthrough_fchmodat:
     *  https://linux.die.net/man/2/fchmodat
     * @param dirfd
     * @param path
     * @param mode
     * @param flags
     * @return
     */
    int passthrough_fchmodat (int dirfd, const char* path, mode_t mode, int flags);

    /**
     * passthrough_chown:
     *  https://linux.die.net/man/2/chmod
     * @param pathname
     * @param owner
     * @param group
     * @return
     */
    int passthrough_chown (const char* pathname, uid_t owner, gid_t group);

    /**
     * passthrough_lchown:
     *  https://linux.die.net/man/2/lchmod
     * @param pathname
     * @param owner
     * @param group
     * @return
     */
    int passthrough_lchown (const char* pathname, uid_t owner, gid_t group);

    /**
     * passthrough_fchown:
     *  https://linux.die.net/man/2/fchmod
     * @param fd
     * @param owner
     * @param group
     * @return
     */
    int passthrough_fchown (int fd, uid_t owner, gid_t group);

    /**
     * passthrough_fchownat:
     *  https://linux.die.net/man/2/fchmodat
     * @param dirfd
     * @param pathname
     * @param owner
     * @param group
     * @param flags
     * @return
     */
    int passthrough_fchownat (int dirfd, const char* pathname, uid_t owner, gid_t group, int flags);
};
} // namespace padll

#endif // PADDL_POSIX_PASSTHROUGH_HPP
