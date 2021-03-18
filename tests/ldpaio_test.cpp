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

int main (int argc, char* argv[])
{
    ssize_t read_bytes = 0;
    ssize_t written_bytes = 0;
    char* buffer = static_cast<char*> (std::malloc (sizeof (char) * BUFFER_SIZE));

    int fd_write = ::open ("tmp.txt", O_CREAT | O_TRUNC | O_RDWR, 0600);

    while ((read_bytes = ::read (STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
        written_bytes += ::write (fd_write, buffer, read_bytes);
   }

    close (fd_write);
    delete (buffer);

    return 0;
}
