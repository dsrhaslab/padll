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

std::shared_ptr<padll::Logging> m_logger_ptr { std::make_shared<padll::Logging> (
    padll::option_default_enable_debug_level,
    padll::option_default_enable_debug_with_ld_preload,
    padll::option_default_log_path) };
padll::LdPreloadedPosix m_ld_preloaded_posix { m_logger_ptr };
padll::PosixPassthrough m_posix_passthrough { m_logger_ptr };

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
extern "C" ssize_t read (int fd, void* buf, size_t size);

/**
 * write:
 * @param fd
 * @param buf
 * @param size
 * @return
 */
extern "C" ssize_t write (int fd, const void* buf, size_t size);

/**
 * pread:
 * @param fd
 * @param buf
 * @param size
 * @param offset
 * @return
 */
extern "C" ssize_t pread (int fd, void* buf, size_t size, off_t offset);

/**
 * pwrite:
 * @param fd
 * @param buf
 * @param size
 * @param offset
 * @return
 */
extern "C" ssize_t pwrite (int fd, const void* buf, size_t size, off_t offset);

/**
 * pread64:
 * @param fd
 * @param buf
 * @param size
 * @param offset
 * @return
 */
#if defined(__USE_LARGEFILE64)
extern "C" ssize_t pread64 (int fd, void* buf, size_t size, off64_t offset);
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
extern "C" ssize_t pwrite64 (int fd, const void* buf, size_t size, off64_t offset);
#endif

/**
 * fread:
 * @param ptr
 * @param size
 * @param nmemb
 * @param stream
 * @return
 */
extern "C" size_t fread (void* ptr, size_t size, size_t nmemb, FILE* stream);

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
extern "C" size_t fwrite (const void* ptr, size_t size, size_t nmemb, FILE* stream);
//
///**
// * open:
// * @param path
// * @param flags
// * @param ...
// * @return
// */
// extern "C" int open (const char* path, int flags, ...);
//
///**
// * creat:
// * @param path
// * @param mode
// * @return
// */
// extern "C" int creat (const char* path, mode_t mode);
//
///**
// * creat64:
// * @param path
// * @param mode
// * @return
// */
// extern "C" int creat64 (const char* path, mode_t mode);
//
///**
// * openat:
// * @param dirfd
// * @param path
// * @param flags
// * @param ...
// * @return
// */
// extern "C" int openat (int dirfd, const char* path, int flags, ...);
//
///**
// * open64:
// * @param path
// * @param flags
// * @param ...
// * @return
// */
// extern "C" int open64 (const char* path, int flags, ...);
//
///**
// * close:
// * @param fd
// * @return
// */
// extern "C" int close (int fd);
//
///**
// * fsync:
// * @param fd
// * @return
// */
// extern "C" int fsync (int fd);
//
///**
// * fdatasync:
// * @param fd
// * @return
// */
// extern "C" int fdatasync (int fd);
//
///**
// * sync:
// */
// extern "C" void sync ();
//
///**
// * syncfs:
// * @param fd
// */
// extern "C" int syncfs (int fd);
//
///**
// * truncate:
// * @param path
// * @param length
// * @return
// */
// extern "C" int truncate (const char* path, off_t length);
//
///**
// * ftruncate:
// * @param fd
// * @param length
// * @return
// */
// extern "C" int ftruncate (int fd, off_t length);
//
///**
// * truncate64:
// * @param path
// * @param length
// * @return
// */
// extern "C" int truncate64 (const char* path, off_t length);
//
///**
// * ftruncate64:
// * @param fd
// * @param length
// * @return
// */
// extern "C" int ftruncate64 (int fd, off_t length);
//
///**
// * __xstat:
// *  This method respects to the stat call.
// * @param version
// * @param path
// * @param statbuf
// * @return
// */
// extern "C" int __xstat (int version, const char* path, struct stat* statbuf);
//
///**
// * __lxstat:
// *  This method respects to the lstat call.
// * @param version
// * @param path
// * @param statbuf
// * @return
// */
// extern "C" int __lxstat (int version, const char* path, struct stat* statbuf);
//
///**
// * __fxstat:
// *  This method respects to the fstat call.
// * @param version
// * @param fd
// * @param statbuf
// * @return
// */
// extern "C" int __fxstat (int version, int fd, struct stat* statbuf);
//
///**
// * fxstatat:
// *  This method respects to the fstatat call.
// * @param version
// * @param dirfd
// * @param path
// * @param statbuf
// * @param flags
// * @return
// */
// extern "C" int
//__fxstatat (int version, int dirfd, const char* path, struct stat* statbuf, int flags);
//
///**
// * __xstat64:
// *  This method respects to the stat64 call.
// * @param version
// * @param path
// * @param statbuf
// * @return
// */
// extern "C" int __xstat64 (int version, const char* path, struct stat64* statbuf);
//
///**
// * __lxstat64:
// *  This method respects to the lstat64 call.
// * @param version
// * @param path
// * @param statbuf
// * @return
// */
// extern "C" int __lxstat64 (int version, const char* path, struct stat64* statbuf);
//
///**
// * __fxstat64:
// *  This method respects to the fstat64 call.
// * @param version
// * @param fd
// * @param statbuf
// * @return
// */
// extern "C" int __fxstat64 (int version, int fd, struct stat64* statbuf);
//
///**
// * fxstatat64:
// *  This method respects to the fstatat64 call.
// * @param version
// * @param dirfd
// * @param path
// * @param statbuf
// * @param flags
// * @return
// */
// extern "C" int
//__fxstatat64 (int version, int dirfd, const char* path, struct stat64* statbuf, int flags);
//
///**
// * statfs:
// * @param path
// * @param buf
// * @return
// */
// extern "C" int statfs (const char* path, struct statfs* buf);
//
///**
// * fstatfs:
// * @param fd
// * @param buf
// * @return
// */
// extern "C" int fstatfs (int fd, struct statfs* buf);
//
///**
// * statfs64:
// * @param path
// * @param buf
// * @return
// */
// extern "C" int statfs64 (const char* path, struct statfs64* buf);
//
///**
// * fstatfs64:
// * @param fd
// * @param buf
// * @return
// */
// extern "C" int fstatfs64 (int fd, struct statfs64* buf);
//
///**
// * link:
// * @param old_path
// * @param new_path
// * @return
// */
// extern "C" int link (const char* old_path, const char* new_path);
//
///**
// * unlink:
// * @param path
// * @return
// */
// extern "C" int unlink (const char* path);
//
///**
// * linkat:
// * @param olddirfd
// * @param old_path
// * @param newdirfd
// * @param new_path
// * @param flags
// * @return
// */
// extern "C" int
// linkat (int olddirfd, const char* old_path, int newdirfd, const char* new_path, int flags);
//
///**
// * unlinkat:
// * @param dirfd
// * @param pathname
// * @param flags
// * @return
// */
// extern "C" int unlinkat (int dirfd, const char* pathname, int flags);
//
///**
// * rename:
// *  https://man7.org/linux/man-pages/man2/rename.2.html
// * @param old_path
// * @param new_path
// * @return
// */
// extern "C" int rename (const char* old_path, const char* new_path);
//
///**
// * renameat:
// *  https://man7.org/linux/man-pages/man2/renameat.2.html
// * @param olddirfd
// * @param old_path
// * @param newdirfd
// * @param new_path
// * @return
// */
// extern "C" int renameat (int olddirfd, const char* old_path, int newdirfd, const char* new_path);
//
///**
// * symlink:
// * @param target
// * @param linkpath
// * @return
// */
// extern "C" int symlink (const char* target, const char* linkpath);
//
///**
// * symlinkat:
// * @param target
// * @param newdirfd
// * @param linkpath
// * @return
// */
// extern "C" int symlinkat (const char* target, int newdirfd, const char* linkpath);
//
///**
// * readlink:
// * @param path
// * @param buf
// * @param bufsize
// * @return
// */
// extern "C" ssize_t readlink (const char* path, char* buf, size_t bufsize);
//
///**
// * readlinkat:
// * @param dirfd
// * @param path
// * @param buf
// * @param bufsize
// * @return
// */
// extern "C" ssize_t readlinkat (int dirfd, const char* path, char* buf, size_t bufsize);
//
///**
// * fopen:
// * @param pathname
// * @param mode
// * @return
// */
// extern "C" FILE* fopen (const char* pathname, const char* mode);
//
///**
// * fopen64:
// * @param pathname
// * @param mode
// * @return
// */
// extern "C" FILE* fopen64 (const char* pathname, const char* mode);
//
///**
// * fdopen:
// * @param fd
// * @param mode
// * @return
// */
// extern "C" FILE* fdopen (int fd, const char* mode);
//
///**
// * freopen:
// * @param pathname
// * @param mode
// * @param stream
// * @return
// */
// extern "C" FILE* freopen (const char* pathname, const char* mode, FILE* stream);
//
///**
// * freopen64:
// * @param pathname
// * @param mode
// * @param stream
// * @return
// */
// extern "C" FILE* freopen64 (const char* pathname, const char* mode, FILE* stream);
//
///**
// * fclose:
// * @param stream
// * @return
// */
// extern "C" int fclose (FILE* stream);
//
///**
// * fflush:
// * Note: Attention: this method will (as well as fwrite) will through a segmentation fault when
// *  using the Logging library (even in the initialization, constructors, ...).
// * @param stream
// * @return
// */
// extern "C" int fflush (FILE* stream);
//
///**
// * access:
// * @param path
// * @param mode
// * @return
// */
// extern "C" int access (const char* path, int mode);
//
///**
// * faccessat:
// * @param dirfd
// * @param path
// * @param mode
// * @param flags
// * @return
// */
// extern "C" int faccessat (int dirfd, const char* path, int mode, int flags);
//
///**
// * lseek:
// * @param fd
// * @param offset
// * @param whence
// * @return
// */
// extern "C" off_t lseek (int fd, off_t offset, int whence);
//
///**
// * fseek:
// * @param stream
// * @param offset
// * @param whence
// * @return
// */
// extern "C" int fseek (FILE* stream, long offset, int whence);
//
///**
// * ftell:
// * @param stream
// * @return
// */
// extern "C" long ftell (FILE* stream);
//
///**
// * lseek64:
// * @param fd
// * @param offset
// * @param whence
// * @return
// */
// extern "C" off_t lseek64 (int fd, off_t offset, int whence);
//
///**
// * fseeko64:
// * @param stream
// * @param offset
// * @param whence
// * @return
// */
// extern "C" int fseeko64 (FILE* stream, off_t offset, int whence);
//
///**
// * ftello64:
// * @param stream
// * @return
// */
// extern "C" off_t ftello64 (FILE* stream);
//
///**
// * mkdir:
// * @param path
// * @param mode
// * @return
// */
// extern "C" int mkdir (const char* path, mode_t mode);
//
///**
// * mkdirat:
// * @param dirfd
// * @param path
// * @param mode
// * @return
// */
// extern "C" int mkdirat (int dirfd, const char* path, mode_t mode);
//
///**
// * readdir:
// * @param dirp
// * @return
// */
// extern "C" struct dirent* readdir (DIR* dirp);
//
///**
// * readdir64:
// * @param dirp
// * @return
// */
// extern "C" struct dirent64* readdir64 (DIR* dirp);
//
///**
// * opendir:
// * @param path
// * @return
// */
// extern "C" DIR* opendir (const char* path);
//
///**
// * fdopendir:
// * @param fd
// * @return
// */
// extern "C" DIR* fdopendir (int fd);
//
///**
// * closedir:
// * @param dirp
// * @return
// */
// extern "C" int closedir (DIR* dirp);
//
///**
// * rmdir:
// * @param path
// * @return
// */
// extern "C" int rmdir (const char* path);
//
///**
// * dirfd:
// * @param dirp
// * @return
// */
// extern "C" int dirfd (DIR* dirp);
//
///**
// * getxattr:
// * @param path
// * @param name
// * @param value
// * @param size
// * @return
// */
// extern "C" ssize_t getxattr (const char* path, const char* name, void* value, size_t size);
//
///**
// * lgetxattr:
// * @param path
// * @param name
// * @param value
// * @param size
// * @return
// */
// extern "C" ssize_t lgetxattr (const char* path, const char* name, void* value, size_t size);
//
///**
// * fgetxattr:
// * @param fd
// * @param name
// * @param value
// * @param size
// * @return
// */
// extern "C" ssize_t fgetxattr (int fd, const char* name, void* value, size_t size);
//
///**
// * setxattr:
// * @param path
// * @param name
// * @param value
// * @param size
// * @param flags
// * @return
// */
// extern "C" int
// setxattr (const char* path, const char* name, const void* value, size_t size, int flags);
//
///**
// * lsetxattr:
// * @param path
// * @param name
// * @param value
// * @param size
// * @param flags
// * @return
// */
// extern "C" int
// lsetxattr (const char* path, const char* name, const void* value, size_t size, int flags);
//
///**
// * fsetxattr:
// * @param fd
// * @param name
// * @param value
// * @param size
// * @param flags
// * @return
// */
// extern "C" int fsetxattr (int fd, const char* name, const void* value, size_t size, int flags);
//
///**
// * listxattr:
// * @param path
// * @param list
// * @param size
// * @return
// */
// extern "C" ssize_t listxattr (const char* path, char* list, size_t size);
//
///**
// * llistxattr:
// * @param path
// * @param list
// * @param size
// * @return
// */
// extern "C" ssize_t llistxattr (const char* path, char* list, size_t size);
//
///**
// * flistxattr:
// * @param fd
// * @param list
// * @param size
// * @return
// */
// extern "C" ssize_t flistxattr (int fd, char* list, size_t size);
//
///**
// * removexattr:
// * @param path
// * @param name
// * @return
// */
// extern "C" int removexattr (const char* path, const char* name);
//
///**
// * lremovexattr:
// * @param path
// * @param name
// * @return
// */
// extern "C" int lremovexattr (const char* path, const char* name);
//
///**
// * fremovexattr:
// * @param fd
// * @param name
// * @return
// */
// extern "C" int fremovexattr (int fd, const char* name);
//
///**
// * chmod:
// * @param pathname
// * @param mode
// * @return
// */
// extern "C" int chmod (const char* pathname, mode_t mode);
//
///**
// * fchmod:
// * @param fd
// * @param mode
// * @return
// */
// extern "C" int fchmod (int fd, mode_t mode);
//
///**
// * fchmodat:
// * @param dirfd
// * @param pathname
// * @param mode
// * @param flags
// * @return
// */
// extern "C" int fchmodat (int dirfd, const char* pathname, mode_t mode, int flags);
//
///**
// * chown:
// * @param pathname
// * @param owner
// * @param group
// * @return
// */
// extern "C" int chown (const char* pathname, uid_t owner, gid_t group);
//
///**
// * lchown:
// * @param pathname
// * @param owner
// * @param group
// * @return
// */
// extern "C" int lchown (const char* pathname, uid_t owner, gid_t group);
//
///**
// * fchown:
// * @param fd
// * @param owner
// * @param group
// * @return
// */
// extern "C" int fchown (int fd, uid_t owner, gid_t group);
//
///**
// * fchownat:
// * @param dirfd
// * @param pathname
// * @param owner
// * @param group
// * @param flags
// * @return
// */
// extern "C" int fchownat (int dirfd, const char* pathname, uid_t owner, gid_t group, int flags);

#endif // PADLL_POSIX_FILE_SYSTEM_H