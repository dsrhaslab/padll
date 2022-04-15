/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
 **/

#ifndef PADLL_LD_PRELOADED_POSIX_HPP
#define PADLL_LD_PRELOADED_POSIX_HPP

#define _GNU_SOURCE 1

#include <iostream>
#include <padll/interface/ldpreloaded/dlsym_hook_libc.hpp>
#include <padll/library_headers/libc_enums.hpp>
#include <padll/library_headers/libc_headers.hpp>
#include <padll/stage/data_plane_stage.hpp>
#include <padll/stage/mount_point_table.hpp>
#include <padll/statistics/statistics.hpp>
#include <padll/utils/log.hpp>
#include <unistd.h>

using namespace padll::headers;
using namespace padll::stage;
using namespace padll::stats;
using namespace padll::utils::log;

namespace padll::interface::ldpreloaded {

/**
 * LdPreloadedPosix class.
 *  https://www.gnu.org/software/libc/manual/html_node/Function-Index.html
 * missing: Complete me ...
 */
class LdPreloadedPosix {

private:
    std::mutex m_lock;
    libc_metadata m_metadata_operations {};
    libc_data m_data_operations {};
    libc_directory m_directory_operations {};
    libc_extattr m_extattr_operations {};
    libc_special m_special_operations {};
    std::shared_ptr<Log> m_log { nullptr };
    DlsymHookLibc m_dlsym_hook {};

    std::atomic<bool> m_collect { option_default_statistic_collection };
    Statistics m_metadata_stats { "metadata", OperationType::metadata_calls };
    Statistics m_data_stats { "data", OperationType::data_calls };
    Statistics m_dir_stats { "directory", OperationType::directory_calls };
    Statistics m_ext_attr_stats { "ext-attr", OperationType::ext_attr_calls };
    Statistics m_special_stats { "special", OperationType::special_calls };

    // data plane stage configurations
    std::unique_ptr<DataPlaneStage> m_stage { std::make_unique<DataPlaneStage> () };
    MountPointTable m_mount_point_table { this->m_log };

    /**
     * enforce_request:
     */
    [[nodiscard]] bool enforce_request (const std::string_view& function_name,
        const uint32_t& workflow_id,
        const int& operation_type,
        const int& operation_context,
        const int& payload);

    /**
     * update_statistic_entry_data:
     * @param operation
     * @param result
     * @param enforced
     */
    void
    update_statistic_entry_data (const int& operation, const ssize_t& bytes, const bool& enforced);

    /**
     * update_statistic_entry_metadata:
     * @param operation
     * @param result
     * @param enforced
     */
    void update_statistic_entry_metadata (const int& operation,
        const int& result,
        const bool& enforced);

    /**
     * update_statistic_entry_dir:
     * @param operation
     * @param result
     * @param enforced
     */
    void
    update_statistic_entry_dir (const int& operation, const int& result, const bool& enforced);

    /**
     * update_statistic_entry_ext_attr:
     * @param operation
     * @param result
     * @param enforced
     */
    void update_statistic_entry_ext_attr (const int& operation,
        const ssize_t& bytes,
        const bool& enforced);

    /**
     * update_statistic_entry_special:
     * @param operation
     * @param result
     * @param enforced
     */
    void
    update_statistic_entry_special (const int& operation, const int& result, const bool& enforced);

    /**
     * update_staitistcs:
     * @param operation_type
     * @param operation
     * @param result
     * @param enforced
     */
    void update_statistics (const OperationType& operation_type,
        const int& operation,
        const long& result,
        const bool& enforced);

public:
    /**
     * LdPreloadedPosix default constructor.
     */
    LdPreloadedPosix ();

    /**
     * LdPreloadedPosix parameterized constructor.
     * @param lib String that respects to the library that will be intercepted.
     * @param stat_collection Boolean that defines if statistic collection is enabled or disabled.
     * @param log_ptr
     */
    LdPreloadedPosix (const std::string& lib,
        const bool& stat_collection,
        std::shared_ptr<Log> log_ptr);

    /**
     * LdPreloadedPosix default destructor.
     */
    ~LdPreloadedPosix ();

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
     * ld_preloaded_posix_read:
     *  https://linux.die.net/man/2/read
     * @param fd
     * @param buf
     * @param counter
     * @return
     */
    ssize_t ld_preloaded_posix_read (int fd, void* buf, size_t counter);

    /**
     * ld_preloaded_posix_write:
     *  https://linux.die.net/man/2/write
     * @param fd
     * @param buf
     * @param counter
     * @return
     */
    ssize_t ld_preloaded_posix_write (int fd, const void* buf, size_t counter);

    /**
     * ld_preloaded_posix_pread:
     *  https://linux.die.net/man/2/pread
     * @param fd
     * @param buf
     * @param counter
     * @param offset
     * @return
     */
    ssize_t ld_preloaded_posix_pread (int fd, void* buf, size_t counter, off_t offset);

    /**
     * ld_preloaded_posix_pwrite:
     *  https://linux.die.net/man/2/pwrite
     * @param fd
     * @param buf
     * @param counter
     * @param offset
     * @return
     */
    ssize_t ld_preloaded_posix_pwrite (int fd, const void* buf, size_t counter, off_t offset);

    /**
     * ld_preloaded_posix_pread64:
     *  https://linux.die.net/man/2/pread64
     *  https://www.mkssoftware.com/docs/man3/read.3.asp
     * @param fd
     * @param buf
     * @param counter
     * @param offset
     * @return
     */
#if defined(__USE_LARGEFILE64)
    ssize_t ld_preloaded_posix_pread64 (int fd, void* buf, size_t counter, off64_t offset);
#endif

    /**
     * ld_preloaded_posix_pwrite:
     *  https://linux.die.net/man/2/pwrite64
     *  https://www.mkssoftware.com/docs/man3/write.3.asp
     * @param fd
     * @param buf
     * @param counter
     * @param offset
     * @return
     */
#if defined(__USE_LARGEFILE64)
    ssize_t ld_preloaded_posix_pwrite64 (int fd, const void* buf, size_t counter, off64_t offset);
#endif

    /**
     * ld_preloaded_posix_open:
     *  https://linux.die.net/man/2/open
     * Notes:
     *  https://github.com/fritzw/ld-preload-open/blob/master/path-mapping.c
     *  https://github.com/poliva/ldpreloadhook/blob/master/hook.c
     * @param path
     * @param flags
     * @param mode
     * @return
     */
    int ld_preloaded_posix_open (const char* path, int flags, mode_t mode);

    /**
     * ld_preloaded_posix_open:
     *  https://linux.die.net/man/2/open
     * @param path
     * @param flags
     * @return
     */
    int ld_preloaded_posix_open (const char* path, int flags);

    /**
     * ld_preloaded_posix_creat:
     *  https://linux.die.net/man/2/creat
     * @param path
     * @param mode
     * @return
     */
    int ld_preloaded_posix_creat (const char* path, mode_t mode);

    /**
     * ld_preloaded_posix_creat64:
     *  https://www.unix.com/man-page/hpux/2/creat64/
     * @param path
     * @param mode
     * @return
     */
    int ld_preloaded_posix_creat64 (const char* path, mode_t mode);

    /**
     * ld_preloaded_posix_openat:
     *  https://linux.die.net/man/2/openat
     * @param dirfd
     * @param path
     * @param flags
     * @param mode
     * @return
     */
    int ld_preloaded_posix_openat (int dirfd, const char* path, int flags, mode_t mode);

    /**
     * ld_preloaded_posix_openat:
     *  https://linux.die.net/man/2/openat
     * @param dirfd
     * @param path
     * @param flags
     * @return
     */
    int ld_preloaded_posix_openat (int dirfd, const char* path, int flags);

    /**
     * ld_preloaded_posix_open64:
     *  https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/open64.c.html
     * @param path
     * @param flags
     * @param mode
     * @return
     */
    int ld_preloaded_posix_open64 (const char* path, int flags, mode_t mode);

    /**
     * ld_preloaded_posix_open64:
     *  https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/open64.c.html
     * @param path
     * @param flags
     * @return
     */
    int ld_preloaded_posix_open64 (const char* path, int flags);

    /**
     * ld_preloaded_posix_close:
     *  https://linux.die.net/man/2/close
     * @param fd
     * @return
     */
    int ld_preloaded_posix_close (int fd);

    /**
     * ld_preloaded_posix_sync:
     *  https://linux.die.net/man/2/sync
     */
    void ld_preloaded_posix_sync ();

    /**
     * ld_preloaded_posix_statfs:
     *  https://linux.die.net/man/2/statfs
     * @param path
     * @param buf
     * @return
     */
    int ld_preloaded_posix_statfs (const char* path, struct statfs* buf);

    /**
     * ld_preloaded_posix_fstatfs
     *  https://linux.die.net/man/2/fstatfs
     * @param fd
     * @param buf
     * @return
     */
    int ld_preloaded_posix_fstatfs (int fd, struct statfs* buf);

    /**
     * ld_preloaded_posix_statfs64:
     *  https://linux.die.net/man/2/statfs64
     *  https://refspecs.linuxbase.org/LSB_5.0.0/LSB-Core-generic/LSB-Core-generic/baselib-statfs64.html
     * @param path
     * @param buf
     * @return
     */
    int ld_preloaded_posix_statfs64 (const char* path, struct statfs64* buf);

    /**
     * ld_preloaded_posix_fstatfs64
     *  https://linux.die.net/man/2/fstatfs64
     *  https://refspecs.linuxbase.org/LSB_5.0.0/LSB-Core-generic/LSB-Core-generic/baselib-fstatfs64.html
     * @param fd
     * @param buf
     * @return
     */
    int ld_preloaded_posix_fstatfs64 (int fd, struct statfs64* buf);

    /**
     * ld_preloaded_posix_unlink:
     *  https://linux.die.net/man/2/unlink
     * @param old_path
     * @return
     */
    int ld_preloaded_posix_unlink (const char* old_path);

    /**
     * ld_preloaded_posix_unlinkat:
     *  https://linux.die.net/man/2/unlinkat
     * @param dirfd
     * @param pathname
     * @param flags
     * @return
     */
    int ld_preloaded_posix_unlinkat (int dirfd, const char* pathname, int flags);

    /**
     * ld_preloaded_posix_rename:
     *  https://linux.die.net/man/2/rename
     * @param old_path
     * @param new_path
     * @return
     */
    int ld_preloaded_posix_rename (const char* old_path, const char* new_path);

    /**
     * ld_preloaded_posix_renameat:
     *  https://linux.die.net/man/2/renameat
     * @param olddirfd
     * @param old_path
     * @param newdirfd
     * @param new_path
     * @return
     */
    int ld_preloaded_posix_renameat (int olddirfd,
        const char* old_path,
        int newdirfd,
        const char* new_path);

    /**
     * ld_preloaded_posix_fopen:
     *  https://linux.die.net/man/3/fopen
     * @param pathname
     * @param mode
     * @return
     */
    FILE* ld_preloaded_posix_fopen (const char* pathname, const char* mode);

    /**
     * ld_preloaded_posix_fopen64:
     *  https://sites.ualberta.ca/dept/chemeng/AIX-43/share/man/info/C/a_doc_lib/libs/basetrf1/fopen.htm
     * @param pathname
     * @param mode
     * @return
     */
    FILE* ld_preloaded_posix_fopen64 (const char* pathname, const char* mode);

    /**
     * ld_preloaded_posix_fclose:
     *  https://linux.die.net/man/3/fclose
     * @param stream
     * @return
     */
    int ld_preloaded_posix_fclose (FILE* stream);

    /**
     * ld_preloaded_posix_mkdir:
     *  https://linux.die.net/man/2/mkdir
     * @param path
     * @param mode
     * @return
     */
    int ld_preloaded_posix_mkdir (const char* path, mode_t mode);

    /**
     * ld_preloaded_posix_mkdirat:
     *  https://linux.die.net/man/2/mkdirat
     * @param dirfd
     * @param path
     * @param mode
     * @return
     */
    int ld_preloaded_posix_mkdirat (int dirfd, const char* path, mode_t mode);

    /**
     * ld_preloaded_posix_mknod:
     * @param path
     * @param mode
     * @param dev
     * @return
     */
    int ld_preloaded_posix_mknod (const char* path, mode_t mode, dev_t dev);

    /**
     * ld_preloaded_posix_mknodat:
     * @param dirfd
     * @param path
     * @param mode
     * @param dev
     * @return
     */
    int ld_preloaded_posix_mknodat (int dirfd, const char* path, mode_t mode, dev_t dev);

    /**
     * ld_preloaded_posix_rmdir:
     *  https://linux.die.net/man/3/rmdir
     * @param path
     * @return
     */
    int ld_preloaded_posix_rmdir (const char* path);

    /**
     * ld_preloaded_posix_getxattr:
     *  https://linux.die.net/man/2/getxattr
     * @param path
     * @param name
     * @param value
     * @param size
     * @return
     */
    ssize_t
    ld_preloaded_posix_getxattr (const char* path, const char* name, void* value, size_t size);

    /**
     * ld_preloaded_posix_lgetxattr:
     *  https://linux.die.net/man/2/lgetxattr
     * @param path
     * @param name
     * @param value
     * @param size
     * @return
     */
    ssize_t
    ld_preloaded_posix_lgetxattr (const char* path, const char* name, void* value, size_t size);

    /**
     * ld_preloaded_posix_fgetxattr:
     *  https://linux.die.net/man/2/fgetxattr
     * @param fd
     * @param name
     * @param value
     * @param size
     * @return
     */
    ssize_t ld_preloaded_posix_fgetxattr (int fd, const char* name, void* value, size_t size);

    /**
     * ld_preloaded_posix_setxattr:
     *  https://linux.die.net/man/2/setxattr
     * @param path
     * @param name
     * @param value
     * @param size
     * @param flags
     * @return
     */
    int ld_preloaded_posix_setxattr (const char* path,
        const char* name,
        const void* value,
        size_t size,
        int flags);

    /**
     * ld_preloaded_posix_lsetxattr:
     *  https://linux.die.net/man/2/lsetxattr
     * @param path
     * @param name
     * @param value
     * @param size
     * @param flags
     * @return
     */
    int ld_preloaded_posix_lsetxattr (const char* path,
        const char* name,
        const void* value,
        size_t size,
        int flags);

    /**
     * ld_preloaded_posix_fsetxattr:
     *  https://linux.die.net/man/2/fsetxattr
     * @param fd
     * @param name
     * @param value
     * @param size
     * @param flags
     * @return
     */
    int ld_preloaded_posix_fsetxattr (int fd,
        const char* name,
        const void* value,
        size_t size,
        int flags);

    /**
     * ld_preloaded_posix_listxattr:
     *  https://linux.die.net/man/2/listxattr
     * @param path
     * @param list
     * @param size
     * @return
     */
    ssize_t ld_preloaded_posix_listxattr (const char* path, char* list, size_t size);

    /**
     * ld_preloaded_posix_llistxattr:
     *  https://linux.die.net/man/2/llistxattr
     * @param path
     * @param list
     * @param size
     * @return
     */
    ssize_t ld_preloaded_posix_llistxattr (const char* path, char* list, size_t size);

    /**
     * ld_preloaded_posix_flistxattr:
     *  https://linux.die.net/man/2/flistxattr
     * @param fd
     * @param list
     * @param size
     * @return
     */
    ssize_t ld_preloaded_posix_flistxattr (int fd, char* list, size_t size);

    /**
     * ld_preloaded_posix_socket:
     * @param domain
     * @param type
     * @param protocol
     */
    int ld_preloaded_posix_socket (int domain, int type, int protocol);

    /**
     * ld_preloaded_posix_fcntl:
     * @param fd
     * @param cmd
     * @param arg (void*)
     * @return
     */
    int ld_preloaded_posix_fcntl (int fd, int cmd, void* arg);
};
} // namespace padll::interface::ldpreloaded

#endif // PADLL_LD_PRELOADED_POSIX_HPP
