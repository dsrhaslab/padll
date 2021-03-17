/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <dlfcn.h>

#define BUFFER_SIZE 1024

typedef ssize_t (*my_write_t) (int, const void*, size_t);
bool is_ld_preload_enabled = true;


ssize_t write_wrapper (int fd, const void* buffer, size_t size) {
    return ::write (fd, buffer, size);
}


int main (int argc, char* argv[])
{

void* lib = ::dlopen ("libc.so.6", RTLD_LAZY);
    my_write_t write_function_ptr { nullptr };

    if (is_ld_preload_enabled) {
        write_function_ptr = (my_write_t)dlsym (lib, "write");
    } else {
//        write_function_ptr = write_wrapper;
        write_function_ptr = ::write;
    }


    ssize_t read_bytes = 0;
    ssize_t written_bytes = 0;
    char* buffer = static_cast<char*> (std::malloc (sizeof (char) * BUFFER_SIZE));

    int fd_write = ::open ("tmp.txt", O_CREAT | O_TRUNC | O_RDWR, 0600);

    while ((read_bytes = ::read (STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
        written_bytes += ::write (fd_write, buffer, read_bytes);
//	((my_write_t)dlsym (lib, "write")) (1, buffer, read_bytes);
  	write_function_ptr (1, buffer, read_bytes);
    }

    close (fd_write);
    delete (buffer);

    return 0;
}
