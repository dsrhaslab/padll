/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <string>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main (int argc, char* argv[])
{
    ssize_t read_bytes = 0;
    ssize_t total_read_bytes = 0;
    ssize_t written_bytes = 0;
    int read_ops = 0;
    int write_ops = 0;
    char* buffer = static_cast<char*> (std::malloc (sizeof (char) * BUFFER_SIZE));

    std::string write_file { "/home/tmp.txt" };
    std::string read_file { "/home/sample.txt" };

    int fd_write = ::open (write_file.c_str (), O_CREAT | O_TRUNC | O_RDWR, 0600);
    int fd_read = ::open (read_file.c_str (), O_RDONLY, 0600);

    while ((read_bytes = ::read (fd_read, buffer, BUFFER_SIZE)) > 0) {
        written_bytes += ::write (fd_write, buffer, read_bytes);
        total_read_bytes += read_bytes;
        read_ops++;
        write_ops++;
    }

    ::close (fd_write);
    delete (buffer);

    std::printf ("Read: %d ops; %ld bytes\n", read_ops, total_read_bytes);
    std::printf ("Write: %d ops; %ld bytes\n", write_ops, written_bytes);

    return 0;
}
