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
PosixPassthrough::PosixPassthrough (const std::string& lib_name) :
    m_lib_name { lib_name }
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
            Logging::log_error ("PosixPassthrough::Error while closing dynamic link (" +
                std::to_string (dlclose_result) + ").");
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
ssize_t PosixPassthrough::passthrough_posix_read (int fd, void *buf, ssize_t counter) {
    return ((libc_read_t) dlsym (RTLD_NEXT, "read")) (fd, buf, counter);
}

// passthrough_posix_write call. (...)
ssize_t PosixPassthrough::passthrough_posix_write (int fd, const void *buf, ssize_t counter) {
    return ((libc_write_t) dlsym (RTLD_NEXT, "write")) (fd, buf, counter);
}

// passthrough_posix_pread call. (...)
ssize_t PosixPassthrough::passthrough_posix_pread (int fd, void *buf, ssize_t counter, off_t offset)
{
    return ((libc_pread_t) dlsym (RTLD_NEXT, "pread")) (fd, buf, counter, offset);
}

// passthrough_posix_pwrite call. (...)
ssize_t PosixPassthrough::passthrough_posix_pwrite (int fd,
    const void* buf,
    ssize_t counter,
    off_t offset)
{
    return ((libc_pwrite_t) dlsym (RTLD_NEXT, "pwrite")) (fd, buf, counter, offset);
}

// passthrough_posix_pread64 call. (...)
#if defined(__USE_LARGEFILE64)
    ssize_t PosixPassthrough::passthrough_posix_pread64 (int fd,
        void* buf,
        ssize_t counter,
        off64_t offset)
    {
        return ((libc_pread64_t) dlsym (RTLD_NEXT, "pread64")) (fd, buf, counter, offset);
    }
#endif

// passthrough_posix_pwrite64 call. (...)
#if defined(__USE_LARGEFILE64)
    ssize_t PosixPassthrough::passthrough_posix_pwrite64 (int fd,
        const void* buf,
        ssize_t counter,
        off64_t offset)
    {
        return ((libc_pwrite64_t) dlsym (RTLD_NEXT, "pwrite64")) (fd, buf, counter, offset);
    }
#endif

// passthrough_posix_fread call. (...)
size_t PosixPassthrough::passthrough_posix_fread (void* ptr,
    size_t size,
    size_t nmemb,
    FILE* stream)
{
    return ((libc_fread_t) dlsym (RTLD_NEXT, "fread")) (ptr, size, nmemb, stream);
}

// passthrough_posix_fwrite call. (...)
size_t PosixPassthrough::passthrough_posix_fwrite (const void* ptr,
    size_t size,
    size_t nmemb,
    FILE* stream)
{
    return ((libc_fwrite_t) dlsym (RTLD_NEXT, "fwrite")) (ptr, size, nmemb, stream);
}


}
