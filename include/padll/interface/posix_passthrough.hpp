/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef PADLL_POSIX_PASSTHROUGH_H
#define PADLL_POSIX_PASSTHROUGH_H

#include <mutex>
#include <padll/libraries/libc_operation_headers.hpp>
#include <padll/utils/logging.hpp>
#include <string>

namespace padll {

class PosixPassthrough {

private:
    std::mutex m_lock;
    std::string m_lib_name { "libc.so.6" };
    void* m_lib_handle { nullptr };

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
     * PosixPassthrough (explicit) parameterized constructor.
     * @param lib
     */
    explicit PosixPassthrough (const std::string& lib);

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
     * passthrough_posix_fread:
     *  https://linux.die.net/man/3/fread
     * @param ptr
     * @param size
     * @param nmemb
     * @param stream
     * @return
     */
    size_t passthrough_posix_fread (void* ptr, size_t size, size_t nmemb, FILE* stream);

    /**
     * passthrough_posix_fwrite:
     *  https://linux.die.net/man/3/fwrite
     * @param ptr
     * @param size
     * @param nmemb
     * @param stream
     * @return
     */
    size_t passthrough_posix_fwrite (const void* ptr, size_t size, size_t nmemb, FILE* stream);

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
     * passthrough_posix_readdir:
     *  https://linux.die.net/man/3/readdir
     * @param dirp
     * @return
     */
    struct dirent* passthrough_posix_readdir (DIR* dirp);

    /**
     * passthrough_posix_readdir64:
     *  https://www.mkssoftware.com/docs/man3/readdir.3.asp
     * @param dirp
     * @return
     */
    struct dirent64* passthrough_posix_readdir64 (DIR* dirp);

    /**
     * passthrough_posix_opendir:
     *  https://linux.die.net/man/3/opendir
     * @param path
     * @return
     */
    DIR* passthrough_posix_opendir (const char* path);

    /**
     * passthrough_posix_fdopendir:
     *  https://linux.die.net/man/3/fdopendir
     * @param fd
     * @return
     */
    DIR* passthrough_posix_fdopendir (int fd);

    /**
     * passthrough_posix_closedir:
     *  https://linux.die.net/man/3/closedir
     * @param dirp
     * @return
     */
    int passthrough_posix_closedir (DIR* dirp);

    /**
     * passthrough_posix_rmdir:
     *  https://linux.die.net/man/3/rmdir
     * @param path
     * @return
     */
    int passthrough_posix_rmdir (const char* path);

    /**
     * passthrough_posix_dirfd:
     *  https://linux.die.net/man/3/dirfd
     * @param dirp
     * @return
     */
    int passthrough_posix_dirfd (DIR* dirp);
};
} // namespace padll
#endif // PADLL_POSIX_PASSTHROUGH_H
