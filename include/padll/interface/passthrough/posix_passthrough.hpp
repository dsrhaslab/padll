/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
 **/

#ifndef PADLL_POSIX_PASSTHROUGH_H
#define PADLL_POSIX_PASSTHROUGH_H

#include <atomic>
#include <mutex>
#include <padll/library_headers/libc_headers.hpp>
#include <padll/options/options.hpp>
#include <padll/statistics/statistics.hpp>
#include <string>
#include <unistd.h>

using namespace padll::headers;
using namespace padll::options;
using namespace padll::stats;

namespace padll::interface::passthrough {

class PosixPassthrough {

private:
    std::mutex m_lock;
    std::string m_lib_name { option_library_name };
    void* m_lib_handle { nullptr };
    std::atomic<bool> m_collect { option_default_statistic_collection };
    Statistics m_metadata_stats { "metadata", OperationType::metadata_calls };
    Statistics m_data_stats { "data", OperationType::data_calls };
    Statistics m_dir_stats { "directory", OperationType::directory_calls };
    Statistics m_ext_attr_stats { "ext-attr", OperationType::ext_attr_calls };

    /**
     * initialize:
     */
    void initialize ();

    /**
     * dlopen_library_handle:
     * @return
     */
    bool dlopen_library_handle ();

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
     * @return std::string
     */
    std::string to_string ();

public:
    /**
     * PosixPassthrough default constructor.
     */
    PosixPassthrough ();

    /**
     * PosixPassthrough parameterized constructor.
     * @param lib
     */
    explicit PosixPassthrough (std::string lib);

    /**
     * PosixPassthrough default destructor.
     */
    ~PosixPassthrough ();

    /**
     * passthrough_posix_read:
     *  https://linux.die.net/man/2/read
     * @param fd
     * @param buf
     * @param counter
     * @return
     */
    ssize_t passthrough_posix_read (int fd, void* buf, size_t counter);

    /**
     * passthrough_posix_write:
     *  https://linux.die.net/man/2/write
     * @param fd
     * @param buf
     * @param counter
     * @return
     */
    ssize_t passthrough_posix_write (int fd, const void* buf, size_t counter);

    /**
     * passthrough_posix_pread:
     *  https://linux.die.net/man/2/pread
     * @param fd
     * @param buf
     * @param counter
     * @param offset
     * @return
     */
    ssize_t passthrough_posix_pread (int fd, void* buf, size_t counter, off_t offset);

    /**
     * passthrough_posix_pwrite:
     *  https://linux.die.net/man/2/pwrite
     * @param fd
     * @param buf
     * @param counter
     * @param offset
     * @return
     */
    ssize_t passthrough_posix_pwrite (int fd, const void* buf, size_t counter, off_t offset);

    /**
     * passthrough_posix_pread64:
     *  https://linux.die.net/man/2/pread64
     *  https://www.mkssoftware.com/docs/man3/read.3.asp
     * @param fd
     * @param buf
     * @param counter
     * @param offset
     * @return
     */
#if defined(__USE_LARGEFILE64)
    ssize_t passthrough_posix_pread64 (int fd, void* buf, size_t counter, off64_t offset);
#endif

    /**
     * passthrough_posix_pwrite64:
     *  https://linux.die.net/man/2/pwrite64
     *  https://www.mkssoftware.com/docs/man3/write.3.asp
     * @param fd
     * @param buf
     * @param counter
     * @param offset
     * @return
     */
#if defined(__USE_LARGEFILE64)
    ssize_t passthrough_posix_pwrite64 (int fd, const void* buf, size_t counter, off64_t offset);
#endif

    /**
     * passthrough_posix_open:
     *  https://linux.die.net/man/2/open
     * Notes:
     *  https://github.com/fritzw/ld-preload-open/blob/master/path-mapping.c
     *  https://github.com/poliva/ldpreloadhook/blob/master/hook.c
     * @param path
     * @param flags
     * @param mode
     * @return
     */
    int passthrough_posix_open (const char* path, int flags, mode_t mode);

    /**
     * passthrough_posix_open:
     *  https://linux.die.net/man/2/open
     * @param path
     * @param flags
     * @return
     */
    int passthrough_posix_open (const char* path, int flags);

    /**
     * passthrough_posix_creat:
     *  https://linux.die.net/man/2/creat
     * @param path
     * @param mode
     * @return
     */
    int passthrough_posix_creat (const char* path, mode_t mode);

    /**
     * passthrough_posix_creat64:
     *  https://www.unix.com/man-page/hpux/2/creat64/
     * @param path
     * @param mode
     * @return
     */
    int passthrough_posix_creat64 (const char* path, mode_t mode);

    /**
     * passthrough_posix_openat:
     *  https://linux.die.net/man/2/openat
     * @param dirfd
     * @param path
     * @param flags
     * @param mode
     * @return
     */
    int passthrough_posix_openat (int dirfd, const char* path, int flags, mode_t mode);

    /**
     * passthrough_posix_openat:
     *  https://linux.die.net/man/2/openat
     * @param dirfd
     * @param path
     * @param flags
     * @return
     */
    int passthrough_posix_openat (int dirfd, const char* path, int flags);

    /**
     * passthrough_posix_open64:
     *  https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/open64.c.html
     * @param path
     * @param flags
     * @param mode
     * @return
     */
    int passthrough_posix_open64 (const char* path, int flags, mode_t mode);

    /**
     * passthrough_posix_open64:
     *  https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/open64.c.html
     * @param path
     * @param flags
     * @return
     */
    int passthrough_posix_open64 (const char* path, int flags);

    /**
     * passthrough_posix_close:
     *  https://linux.die.net/man/2/close
     * @param fd
     * @return
     */
    int passthrough_posix_close (int fd);

    /**
     * passthrough_posix_sync:
     *  https://linux.die.net/man/2/sync
     */
    void passthrough_posix_sync ();

    // /**
    //  * passthrough_posix_stat:
    //  *  This method respects to the stat call.
    //  *  https://refspecs.linuxbase.org/LSB_3.0.0/LSB-PDA/LSB-PDA/baselib-xstat-1.html
    //  *  https://linux.die.net/man/2/stat
    //  * @param version
    //  * @param path
    //  * @param statbuf
    //  * @return
    //  */
    // int passthrough_posix_xstat (int version, const char* path, struct stat* statbuf);

    // /**
    //  * passthrough_posix_lxstat:
    //  *  https://linux.die.net/man/2/lstat
    //  * @param version
    //  * @param path
    //  * @param statbuf
    //  * @return
    //  */
    // int passthrough_posix_lxstat (int version, const char* path, struct stat* statbuf);

    // /**
    //  * passthrough_posix_fxstat:
    //  *  https://linux.die.net/man/2/fstat
    //  * @param version
    //  * @param fd
    //  * @param statbuf
    //  * @return
    //  */
    // int passthrough_posix_fxstat (int version, int fd, struct stat* statbuf);

    // /**
    //  * passthrough_posix_fxstatat:
    //  *  https://refspecs.linuxbase.org/LSB_5.0.0/LSB-Core-generic/LSB-Core-generic/baselib---fxstatat-1.html
    //  *  https://linux.die.net/man/2/fstatat
    //  * @param version
    //  * @param dirfd
    //  * @param path
    //  * @param statbuf
    //  * @param flags
    //  * @return
    //  */
    // int passthrough_posix_fxstatat (int version,
    //     int dirfd,
    //     const char* path,
    //     struct stat* statbuf,
    //     int flags);

    // /**
    //  * passthrough_posix_stat64:
    //  *  This method respects to the stat call.
    //  *  https://refspecs.linuxbase.org/LSB_3.0.0/LSB-PDA/LSB-PDA/baselib-xstat-1.html
    //  *  https://linux.die.net/man/2/stat64
    //  * @param version
    //  * @param path
    //  * @param statbuf
    //  * @return
    //  */
    // int passthrough_posix_xstat64 (int version, const char* path, struct stat64* statbuf);

    // /**
    //  * passthrough_posix_lxstat64:
    //  *  https://linux.die.net/man/2/lstat64
    //  *  https://refspecs.linuxbase.org/LSB_3.0.0/LSB-PDA/LSB-PDA/baselib-xstat64-1.html
    //  * @param version
    //  * @param path
    //  * @param statbuf
    //  * @return
    //  */
    // int passthrough_posix_lxstat64 (int version, const char* path, struct stat64* statbuf);

    // /**
    //  * passthrough_posix_fxstat64:
    //  *  https://linux.die.net/man/2/fstat64
    //  *  https://refspecs.linuxbase.org/LSB_3.0.0/LSB-PDA/LSB-PDA/baselib-xstat64-1.html
    //  * @param version
    //  * @param fd
    //  * @param statbuf
    //  * @return
    //  */
    // int passthrough_posix_fxstat64 (int version, int fd, struct stat64* statbuf);

    // /**
    //  * passthrough_posix_fxstatat64:
    //  *  https://refspecs.linuxbase.org/LSB_5.0.0/LSB-Core-generic/LSB-Core-generic/baselib---fxstatat-1.html
    //  *  https://linux.die.net/man/2/fstatat
    //  * @param version
    //  * @param dirfd
    //  * @param path
    //  * @param statbuf
    //  * @param flags
    //  * @return
    //  */
    // int passthrough_posix_fxstatat64 (int version,
    //     int dirfd,
    //     const char* path,
    //     struct stat64* statbuf,
    //     int flags);

    /**
     * passthrough_posix_statfs:
     *  https://linux.die.net/man/2/statfs
     * @param path
     * @param buf
     * @return
     */
    int passthrough_posix_statfs (const char* path, struct statfs* buf);

    /**
     * passthrough_posix_fstatfs
     *  https://linux.die.net/man/2/fstatfs
     * @param fd
     * @param buf
     * @return
     */
    int passthrough_posix_fstatfs (int fd, struct statfs* buf);

    /**
     * passthrough_posix_statfs64:
     *  https://linux.die.net/man/2/statfs64
     *  https://refspecs.linuxbase.org/LSB_5.0.0/LSB-Core-generic/LSB-Core-generic/baselib-statfs64.html
     * @param path
     * @param buf
     * @return
     */
    int passthrough_posix_statfs64 (const char* path, struct statfs64* buf);

    /**
     * passthrough_posix_fstatfs64
     *  https://linux.die.net/man/2/fstatfs64
     *  https://refspecs.linuxbase.org/LSB_5.0.0/LSB-Core-generic/LSB-Core-generic/baselib-fstatfs64.html
     * @param fd
     * @param buf
     * @return
     */
    int passthrough_posix_fstatfs64 (int fd, struct statfs64* buf);

    /**
     * passthrough_posix_link:
     *  https://linux.die.net/man/2/link
     * @param old_path
     * @param new_path
     * @return
     */
    int passthrough_posix_link (const char* old_path, const char* new_path);

    /**
     * passthrough_posix_unlink:
     *  https://linux.die.net/man/2/unlink
     * @param old_path
     * @return
     */
    int passthrough_posix_unlink (const char* old_path);

    /**
     * passthrough_posix_unlinkat:
     *  https://linux.die.net/man/2/unlinkat
     * @param dirfd
     * @param pathname
     * @param flags
     * @return
     */
    int passthrough_posix_unlinkat (int dirfd, const char* pathname, int flags);

    /**
     * passthrough_posix_rename:
     *  https://linux.die.net/man/2/rename
     * @param old_path
     * @param new_path
     * @return
     */
    int passthrough_posix_rename (const char* old_path, const char* new_path);

    /**
     * passthrough_posix_renameat:
     *  https://linux.die.net/man/2/renameat
     * @param olddirfd
     * @param old_path
     * @param newdirfd
     * @param new_path
     * @return
     */
    int passthrough_posix_renameat (int olddirfd,
        const char* old_path,
        int newdirfd,
        const char* new_path);

    /**
     * passthrough_posix_fopen:
     *  https://linux.die.net/man/3/fopen
     * @param pathname
     * @param mode
     * @return
     */
    FILE* passthrough_posix_fopen (const char* pathname, const char* mode);

    /**
     * passthrough_posix_fopen64:
     *  https://sites.ualberta.ca/dept/chemeng/AIX-43/share/man/info/C/a_doc_lib/libs/basetrf1/fopen.htm
     * @param pathname
     * @param mode
     * @return
     */
    FILE* passthrough_posix_fopen64 (const char* pathname, const char* mode);

    /**
     * passthrough_posix_fclose:
     *  https://linux.die.net/man/3/fclose
     * @param stream
     * @return
     */
    int passthrough_posix_fclose (FILE* stream);

    /**
     * passthrough_posix_mkdir:
     *  https://linux.die.net/man/2/mkdir
     * @param path
     * @param mode
     * @return
     */
    int passthrough_posix_mkdir (const char* path, mode_t mode);

    /**
     * passthrough_posix_mkdirat:
     *  https://linux.die.net/man/2/mkdirat
     * @param dirfd
     * @param path
     * @param mode
     * @return
     */
    int passthrough_posix_mkdirat (int dirfd, const char* path, mode_t mode);

    /**
     * passthrough_posix_rmdir:
     *  https://linux.die.net/man/3/rmdir
     * @param path
     * @return
     */
    int passthrough_posix_rmdir (const char* path);

    /**
     * passthrough_posix_getxattr:
     *  https://linux.die.net/man/2/getxattr
     * @param path
     * @param name
     * @param value
     * @param size
     * @return
     */
    ssize_t
    passthrough_posix_getxattr (const char* path, const char* name, void* value, size_t size);

    /**
     * passthrough_posix_lgetxattr:
     *  https://linux.die.net/man/2/lgetxattr
     * @param path
     * @param name
     * @param value
     * @param size
     * @return
     */
    ssize_t
    passthrough_posix_lgetxattr (const char* path, const char* name, void* value, size_t size);

    /**
     * passthrough_posix_fgetxattr:
     *  https://linux.die.net/man/2/fgetxattr
     * @param fd
     * @param name
     * @param value
     * @param size
     * @return
     */
    ssize_t passthrough_posix_fgetxattr (int fd, const char* name, void* value, size_t size);

    /**
     * passthrough_posix_setxattr:
     *  https://linux.die.net/man/2/setxattr
     * @param path
     * @param name
     * @param value
     * @param size
     * @param flags
     * @return
     */
    int passthrough_posix_setxattr (const char* path,
        const char* name,
        const void* value,
        size_t size,
        int flags);

    /**
     * passthrough_posix_lsetxattr:
     *  https://linux.die.net/man/2/lsetxattr
     * @param path
     * @param name
     * @param value
     * @param size
     * @param flags
     * @return
     */
    int passthrough_posix_lsetxattr (const char* path,
        const char* name,
        const void* value,
        size_t size,
        int flags);

    /**
     * passthrough_posix_fsetxattr:
     *  https://linux.die.net/man/2/fsetxattr
     * @param fd
     * @param name
     * @param value
     * @param size
     * @param flags
     * @return
     */
    int passthrough_posix_fsetxattr (int fd,
        const char* name,
        const void* value,
        size_t size,
        int flags);

    /**
     * passthrough_posix_listxattr:
     *  https://linux.die.net/man/2/listxattr
     * @param path
     * @param list
     * @param size
     * @return
     */
    ssize_t passthrough_posix_listxattr (const char* path, char* list, size_t size);

    /**
     * passthrough_posix_llistxattr:
     *  https://linux.die.net/man/2/llistxattr
     * @param path
     * @param list
     * @param size
     * @return
     */
    ssize_t passthrough_posix_llistxattr (const char* path, char* list, size_t size);

    /**
     * passthrough_posix_flistxattr:
     *  https://linux.die.net/man/2/flistxattr
     * @param fd
     * @param list
     * @param size
     * @return
     */
    ssize_t passthrough_posix_flistxattr (int fd, char* list, size_t size);
};
} // namespace padll::interface::passthrough
#endif // PADLL_POSIX_PASSTHROUGH_H
