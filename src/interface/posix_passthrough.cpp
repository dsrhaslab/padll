/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <padll/interface/posix_passthrough.hpp>

namespace padll {

// PosixPassthrough default constructor.
PosixPassthrough::PosixPassthrough ()
{
    // initialize library handle pointer.
    this->initialize ();
}

// PosixPassthrough explicit parameterized constructor.
PosixPassthrough::PosixPassthrough (const std::string& lib_name) : m_lib_name { lib_name }
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
            Logging::log_error ("PosixPassthrough::Error while closing dynamic link ("
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
        Logging::log_error ("PosixPassthrough::Error while dlopen'ing " + this->m_lib_name + ".");
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

// passthrough_posix_fread call. (...)
size_t
PosixPassthrough::passthrough_posix_fread (void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    return ((libc_fread_t)dlsym (RTLD_NEXT, "fread")) (ptr, size, nmemb, stream);
}

// passthrough_posix_fwrite call. (...)
size_t PosixPassthrough::passthrough_posix_fwrite (const void* ptr,
    size_t size,
    size_t nmemb,
    FILE* stream)
{
    return ((libc_fwrite_t)dlsym (RTLD_NEXT, "fwrite")) (ptr, size, nmemb, stream);
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

// passthrough_posix_readdir call. (...)
struct dirent* PosixPassthrough::passthrough_posix_readdir (DIR* dirp)
{
    return ((libc_readdir_t)dlsym (RTLD_NEXT, "readdir")) (dirp);
}

// passthrough_posix_readdir64 call. (...)
struct dirent64* PosixPassthrough::passthrough_posix_readdir64 (DIR* dirp)
{
    return ((libc_readdir64_t)dlsym (RTLD_NEXT, "readdir64")) (dirp);
}

// passthrough_posix_opendir call. (...)
DIR* PosixPassthrough::passthrough_posix_opendir (const char* path)
{
    return ((libc_opendir_t)dlsym (RTLD_NEXT, "opendir")) (path);
}

// passthrough_posix_fdopendir call. (...)
DIR* PosixPassthrough::passthrough_posix_fdopendir (int fd)
{
    return ((libc_fdopendir_t)dlsym (RTLD_NEXT, "fdopendir")) (fd);
}

// passthrough_posix_closedir call. (...)
int PosixPassthrough::passthrough_posix_closedir (DIR* dirp)
{
    return ((libc_closedir_t)dlsym (RTLD_NEXT, "closedir")) (dirp);
}

// passthrough_posix_rmdir call. (...)
int PosixPassthrough::passthrough_posix_rmdir (const char* path)
{
    return ((libc_rmdir_t)dlsym (RTLD_NEXT, "rmdir")) (path);
}

// passthrough_posix_dirfd call. (...)
int PosixPassthrough::passthrough_posix_dirfd (DIR* dirp)
{
    return ((libc_dirfd_t)dlsym (RTLD_NEXT, "dirfd")) (dirp);
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

// passthrough_posix_removexattr call. (...)
int PosixPassthrough::passthrough_posix_removexattr (const char* path, const char* name)
{
    return ((libc_removexattr_t)dlsym (RTLD_NEXT, "removexattr")) (path, name);
}

// passthrough_posix_lremovexattr call. (...)
int PosixPassthrough::passthrough_posix_lremovexattr (const char* path, const char* name)
{
    return ((libc_lremovexattr_t)dlsym (RTLD_NEXT, "lremovexattr")) (path, name);
}

// passthrough_posix_fremovexattr call. (...)
int PosixPassthrough::passthrough_posix_fremovexattr (int fd, const char* name)
{
    return ((libc_fremovexattr_t)dlsym (RTLD_NEXT, "fremovexattr")) (fd, name);
}

} // namespace padll
