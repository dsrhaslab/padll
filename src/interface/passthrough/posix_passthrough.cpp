/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <padll/interface/passthrough/posix_passthrough.hpp>
#include <utility>

/**
 * Missing: passthrough_posix_open
 */

namespace padll::interface::passthrough {

// PosixPassthrough default constructor.
PosixPassthrough::PosixPassthrough () : m_logger_ptr { std::make_shared<Log> () }
{
    // initialize library handle pointer.
    this->initialize ();
}

// PosixPassthrough explicit parameterized constructor.
PosixPassthrough::PosixPassthrough (std::shared_ptr<Log> log_ptr) :
    m_logger_ptr { std::move (log_ptr) }
{
    // initialize library handle pointer.
    this->initialize ();
}

// PosixPassthrough explicit parameterized constructor.
PosixPassthrough::PosixPassthrough (std::string lib_name, std::shared_ptr<Log> log_ptr) :
    m_lib_name { std::move (lib_name) },
    m_logger_ptr { std::move (log_ptr) }
{
    // initialize library handle pointer.
    this->initialize ();
}

// PosixPassthrough default destructor.
PosixPassthrough::~PosixPassthrough ()
{
    // validate if library handle is valid and close dynamic linking
    if (this->m_lib_handle != nullptr) {
        // close dynamic linking to intercepted library.
        // It decrements the reference count on the dynamically loaded shared object, referred to
        // by handle m_lib_handle. If the reference count drops to zero, then the object is
        // unloaded. All shared objects that were automatically loaded when dlopen () was invoked
        // on the object referred to by handle are recursively closed in the same manner.
        int dlclose_result = ::dlclose (this->m_lib_handle);

        // validate result from dlclose
        if (dlclose_result != 0) {
            this->m_logger_ptr->log_error ("PosixPassthrough::Error while closing dynamic link ("
                + std::to_string (dlclose_result) + ").");
        }
    }
}

// dlopen_library_handle call. (...)
bool PosixPassthrough::dlopen_library_handle ()
{
    std::unique_lock<std::mutex> unique_lock (this->m_lock);
    // Dynamic loading of the libc library (referred to as 'libc.so.6').
    // loads the dynamic shared object (shared library) file named by the null-terminated string
    // filename and returns an opaque "handle" for the loaded object.
    this->m_lib_handle = ::dlopen (this->m_lib_name.data (), RTLD_LAZY);

    // return true if the m_lib_handle is valid, and false otherwise.
    return (this->m_lib_handle != nullptr);
}

// initialize call. (...)
void PosixPassthrough::initialize ()
{

    // open library and assign pointer to m_lib_handle
    bool open_lib_handle = this->dlopen_library_handle ();

    // validate library pointer
    if (!open_lib_handle) {
        this->m_logger_ptr->log_error (
            "PosixPassthrough::Error while dlopen'ing " + this->m_lib_name + ".");
        return;
    }
}

// passthrough_posix_read call. (...)
ssize_t PosixPassthrough::passthrough_posix_read (int fd, void* buf, size_t counter)
{
    return ((libc_read_t)dlsym (RTLD_NEXT, "read")) (fd, buf, counter);
}

// passthrough_posix_write call. (...)
ssize_t PosixPassthrough::passthrough_posix_write (int fd, const void* buf, size_t counter)
{
    return ((libc_write_t)dlsym (RTLD_NEXT, "write")) (fd, buf, counter);
}

// passthrough_posix_pread call. (...)
ssize_t PosixPassthrough::passthrough_posix_pread (int fd, void* buf, size_t counter, off_t offset)
{
    return ((libc_pread_t)dlsym (RTLD_NEXT, "pread")) (fd, buf, counter, offset);
}

// passthrough_posix_pwrite call. (...)
ssize_t
PosixPassthrough::passthrough_posix_pwrite (int fd, const void* buf, size_t counter, off_t offset)
{
    return ((libc_pwrite_t)dlsym (RTLD_NEXT, "pwrite")) (fd, buf, counter, offset);
}

// passthrough_posix_pread64 call. (...)
#if defined(__USE_LARGEFILE64)
ssize_t
PosixPassthrough::passthrough_posix_pread64 (int fd, void* buf, size_t counter, off64_t offset)
{
    return ((libc_pread64_t)dlsym (RTLD_NEXT, "pread64")) (fd, buf, counter, offset);
}
#endif

// passthrough_posix_pwrite64 call. (...)
#if defined(__USE_LARGEFILE64)
ssize_t PosixPassthrough::passthrough_posix_pwrite64 (int fd,
    const void* buf,
    size_t counter,
    off64_t offset)
{
    return ((libc_pwrite64_t)dlsym (RTLD_NEXT, "pwrite64")) (fd, buf, counter, offset);
}
#endif

// passthrough_posix_open call. (...)
int PosixPassthrough::passthrough_posix_open (const char* path, int flags, mode_t mode)
{
    return ((libc_open_variadic_t)dlsym (RTLD_NEXT, "open")) (path, flags, mode);
}

// passthrough_posix_open call. (...)
int PosixPassthrough::passthrough_posix_open (const char* path, int flags)
{
    return ((libc_open_t)dlsym (RTLD_NEXT, "open")) (path, flags);
}

// passthrough_posix_creat call. (...)
int PosixPassthrough::passthrough_posix_creat (const char* path, mode_t mode)
{
    return ((libc_creat_t)dlsym (RTLD_NEXT, "creat")) (path, mode);
}

// passthrough_posix_creat64 call. (...)
int PosixPassthrough::passthrough_posix_creat64 (const char* path, mode_t mode)
{
    return ((libc_creat64_t)dlsym (RTLD_NEXT, "creat64")) (path, mode);
}

// passthrough_posix_openat call. (...)
int PosixPassthrough::passthrough_posix_openat (int dirfd, const char* path, int flags, mode_t mode)
{
    return ((libc_openat_variadic_t)dlsym (RTLD_NEXT, "openat")) (dirfd, path, flags, mode);
}

// passthrough_posix_openat call. (...)
int PosixPassthrough::passthrough_posix_openat (int dirfd, const char* path, int flags)
{
    return ((libc_openat_t)dlsym (RTLD_NEXT, "openat")) (dirfd, path, flags);
}

// passthrough_posix_open64 call. (...)
int PosixPassthrough::passthrough_posix_open64 (const char* path, int flags, mode_t mode)
{
    return ((libc_open64_variadic_t)dlsym (RTLD_NEXT, "open64")) (path, flags, mode);
}

// passthrough_posix_open64 call. (...)
int PosixPassthrough::passthrough_posix_open64 (const char* path, int flags)
{
    return ((libc_open64_t)dlsym (RTLD_NEXT, "open64")) (path, flags);
}

// passthrough_posix_close call. (...)
int PosixPassthrough::passthrough_posix_close (int fd)
{
    return ((libc_close_t)dlsym (RTLD_NEXT, "close")) (fd);
}

// passthrough_posix_sync call. (...)
void PosixPassthrough::passthrough_posix_sync ()
{
    return ((libc_sync_t)dlsym (RTLD_NEXT, "sync")) ();
}

// passthrough_posix_xstat call. (...)
int PosixPassthrough::passthrough_posix_xstat (int version, const char* path, struct stat* statbuf)
{
    return ((libc_xstat_t)dlsym (RTLD_NEXT, "xstat")) (version, path, statbuf);
}

// passthrough_posix_lxstat call. (...)
int PosixPassthrough::passthrough_posix_lxstat (int version, const char* path, struct stat* statbuf)
{
    return ((libc_lxstat_t)dlsym (RTLD_NEXT, "lxstat")) (version, path, statbuf);
}

// passthrough_posix_fxstat call. (...)
int PosixPassthrough::passthrough_posix_fxstat (int version, int fd, struct stat* statbuf)
{
    return ((libc_fxstat_t)dlsym (RTLD_NEXT, "fxstat")) (version, fd, statbuf);
}

// passthrough_posix_fxstatat call. (...)
int PosixPassthrough::passthrough_posix_fxstatat (int version,
    int dirfd,
    const char* path,
    struct stat* statbuf,
    int flags)
{
    return ((libc_fxstatat_t)dlsym (RTLD_NEXT, "fxstatat")) (version, dirfd, path, statbuf, flags);
}

// passthrough_posix_xstat64 call. (...)
int PosixPassthrough::passthrough_posix_xstat64 (int version,
    const char* path,
    struct stat64* statbuf)
{
    return ((libc_xstat64_t)dlsym (RTLD_NEXT, "xstat64")) (version, path, statbuf);
}

// passthrough_posix_lxstat64 call. (...)
int PosixPassthrough::passthrough_posix_lxstat64 (int version,
    const char* path,
    struct stat64* statbuf)
{
    return ((libc_lxstat64_t)dlsym (RTLD_NEXT, "lxstat64")) (version, path, statbuf);
}

// passthrough_posix_fxstat64 call. (...)
int PosixPassthrough::passthrough_posix_fxstat64 (int version, int fd, struct stat64* statbuf)
{
    return ((libc_fxstat64_t)dlsym (RTLD_NEXT, "fxstat64")) (version, fd, statbuf);
}

// passthrough_posix_fxstatat64 call. (...)
int PosixPassthrough::passthrough_posix_fxstatat64 (int version,
    int dirfd,
    const char* path,
    struct stat64* statbuf,
    int flags)
{
    return (
        (libc_fxstatat64_t)dlsym (RTLD_NEXT, "fxstatat64")) (version, dirfd, path, statbuf, flags);
}

// passthrough_posix_statfs call. (...)
int PosixPassthrough::passthrough_posix_statfs (const char* path, struct statfs* buf)
{
    return ((libc_statfs_t)dlsym (RTLD_NEXT, "statfs")) (path, buf);
}

// passthrough_posix_fstatfs call. (...)
int PosixPassthrough::passthrough_posix_fstatfs (int fd, struct statfs* buf)
{
    return ((libc_fstatfs_t)dlsym (RTLD_NEXT, "fstatfs")) (fd, buf);
}

// passthrough_posix_statfs64 call. (...)
int PosixPassthrough::passthrough_posix_statfs64 (const char* path, struct statfs64* buf)
{
    return ((libc_statfs64_t)dlsym (RTLD_NEXT, "statfs64")) (path, buf);
}

// passthrough_posix_fstatfs64 call. (...)
int PosixPassthrough::passthrough_posix_fstatfs64 (int fd, struct statfs64* buf)
{
    return ((libc_fstatfs64_t)dlsym (RTLD_NEXT, "fstatfs64")) (fd, buf);
}

// passthrough_posix_link call. (...)
int PosixPassthrough::passthrough_posix_link (const char* old_path, const char* new_path)
{
    return ((libc_link_t)dlsym (RTLD_NEXT, "link")) (old_path, new_path);
}

// passthrough_posix_unlink call. (...)
int PosixPassthrough::passthrough_posix_unlink (const char* old_path)
{
    return ((libc_unlink_t)dlsym (RTLD_NEXT, "unlink")) (old_path);
}

// passthrough_posix_unlinkat call. (...)
int PosixPassthrough::passthrough_posix_unlinkat (int dirfd, const char* pathname, int flags)
{
    return ((libc_unlinkat_t)dlsym (RTLD_NEXT, "unlinkat")) (dirfd, pathname, flags);
}

// passthrough_posix_rename call. (...)
int PosixPassthrough::passthrough_posix_rename (const char* old_path, const char* new_path)
{
    return ((libc_rename_t)dlsym (RTLD_NEXT, "rename")) (old_path, new_path);
}

// passthrough_posix_renameat call. (...)
int PosixPassthrough::passthrough_posix_renameat (int olddirfd,
    const char* old_path,
    int newdirfd,
    const char* new_path)
{
    return (
        (libc_renameat_t)dlsym (RTLD_NEXT, "renameat")) (olddirfd, old_path, newdirfd, new_path);
}

// passthrough_posix_fopen call. (...)
FILE* PosixPassthrough::passthrough_posix_fopen (const char* pathname, const char* mode)
{
    return ((libc_fopen_t)dlsym (RTLD_NEXT, "fopen")) (pathname, mode);
}

// passthrough_posix_fopen64 call. (...)
FILE* PosixPassthrough::passthrough_posix_fopen64 (const char* pathname, const char* mode)
{
    return ((libc_fopen64_t)dlsym (RTLD_NEXT, "fopen64")) (pathname, mode);
}

// passthrough_posix_fclose call. (...)
int PosixPassthrough::passthrough_posix_fclose (FILE* stream)
{
    return ((libc_fclose_t)dlsym (RTLD_NEXT, "fclose")) (stream);
}

// passthrough_posix_mkdir call. (...)
int PosixPassthrough::passthrough_posix_mkdir (const char* path, mode_t mode)
{
    return ((libc_mkdir_t)dlsym (RTLD_NEXT, "mkdir")) (path, mode);
}

// passthrough_posix_mkdirat call. (...)
int PosixPassthrough::passthrough_posix_mkdirat (int dirfd, const char* path, mode_t mode)
{
    return ((libc_mkdirat_t)dlsym (RTLD_NEXT, "mkdirat")) (dirfd, path, mode);
}

// passthrough_posix_rmdir call. (...)
int PosixPassthrough::passthrough_posix_rmdir (const char* path)
{
    return ((libc_rmdir_t)dlsym (RTLD_NEXT, "rmdir")) (path);
}

// passthrough_posix_getxattr call. (...)
ssize_t PosixPassthrough::passthrough_posix_getxattr (const char* path,
    const char* name,
    void* value,
    size_t size)
{
    return ((libc_getxattr_t)dlsym (RTLD_NEXT, "getxattr")) (path, name, value, size);
}

// passthrough_posix_lgetxattr call. (...)
ssize_t PosixPassthrough::passthrough_posix_lgetxattr (const char* path,
    const char* name,
    void* value,
    size_t size)
{
    return ((libc_lgetxattr_t)dlsym (RTLD_NEXT, "lgetxattr")) (path, name, value, size);
}

// passthrough_posix_fgetxattr call. (...)
ssize_t
PosixPassthrough::passthrough_posix_fgetxattr (int fd, const char* name, void* value, size_t size)
{
    return ((libc_fgetxattr_t)dlsym (RTLD_NEXT, "fgetxattr")) (fd, name, value, size);
}

// passthrough_posix_setxattr call. (...)
int PosixPassthrough::passthrough_posix_setxattr (const char* path,
    const char* name,
    const void* value,
    size_t size,
    int flags)
{
    return ((libc_setxattr_t)dlsym (RTLD_NEXT, "setxattr")) (path, name, value, size, flags);
}

// passthrough_posix_lsetxattr call. (...)
int PosixPassthrough::passthrough_posix_lsetxattr (const char* path,
    const char* name,
    const void* value,
    size_t size,
    int flags)
{
    return ((libc_lsetxattr_t)dlsym (RTLD_NEXT, "lsetxattr")) (path, name, value, size, flags);
}

// passthrough_posix_fsetxattr call. (...)
int PosixPassthrough::passthrough_posix_fsetxattr (int fd,
    const char* name,
    const void* value,
    size_t size,
    int flags)
{
    return ((libc_fsetxattr_t)dlsym (RTLD_NEXT, "fsetxattr")) (fd, name, value, size, flags);
}

// passthrough_posix_listxattr call. (...)
ssize_t PosixPassthrough::passthrough_posix_listxattr (const char* path, char* list, size_t size)
{
    return ((libc_listxattr_t)dlsym (RTLD_NEXT, "listxattr")) (path, list, size);
}

// passthrough_posix_llistxattr call. (...)
ssize_t PosixPassthrough::passthrough_posix_llistxattr (const char* path, char* list, size_t size)
{
    return ((libc_llistxattr_t)dlsym (RTLD_NEXT, "llistxattr")) (path, list, size);
}

// passthrough_posix_flistxattr call. (...)
ssize_t PosixPassthrough::passthrough_posix_flistxattr (int fd, char* list, size_t size)
{
    return ((libc_flistxattr_t)dlsym (RTLD_NEXT, "flistxattr")) (fd, list, size);
}

} // namespace padll::interface::passthrough
