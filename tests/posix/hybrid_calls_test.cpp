/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
 **/

#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>

#define BUFFER_SIZE 1024

typedef ssize_t (*my_write_t) (int, const void*, size_t);

int main (int argc, char* argv[])
{
    bool differentiate_calls = true;
    void* lib { nullptr };
    my_write_t write_function_ptr { nullptr };

    std::cout << "--> " << differentiate_calls << "\n";

    // enable/disable ld_preloaded functions
    if (argc > 1) {
        differentiate_calls = std::stol (std::string { argv[1] });
        std::cout << "--> " << differentiate_calls << "\n";
    }

    if (differentiate_calls) {
        lib = ::dlopen ("libc.so.6", RTLD_LAZY);
        write_function_ptr = (my_write_t)dlsym (lib, "write");
        std::cout << "dlopen and dlsym made ...\n";
    } else {
        write_function_ptr = ::write;
        std::cout << "attributed plain ::write pointer\n";
    }

    ssize_t read_bytes;
    ssize_t total_read_bytes = 0;
    ssize_t written_bytes = 0;
    ssize_t written_bytes_stdout = 0;
    char* buffer = static_cast<char*> (std::malloc (sizeof (char) * BUFFER_SIZE));

    int fd_write = ::open ("tmp.txt", O_CREAT | O_TRUNC | O_RDWR, 0600);

    while ((read_bytes = ::read (STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
        written_bytes += ::write (fd_write, buffer, read_bytes);
        written_bytes_stdout += write_function_ptr (1, buffer, read_bytes);
        total_read_bytes += read_bytes;
    }

    close (fd_write);
    delete (buffer);

    std::cout << "Read bytes: " << total_read_bytes << "\n";
    std::cout << "Written bytes (file): " << written_bytes << "\n";
    std::cout << "Written bytes (stdout): " << written_bytes_stdout << "\n";

    // close pointer to lib
    if (differentiate_calls)
        ::dlclose (lib);

    return 0;
}
