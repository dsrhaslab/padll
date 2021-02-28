/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <cstdio>
#include <unistd.h>
#include <cstdlib>

#define BUFFER_SIZE 1024

int main (int argc, char* argv[]) {

    ssize_t read_bytes = 0;
    ssize_t written_bytes = 0;
    char* buffer = static_cast<char*>(malloc(sizeof(char) * BUFFER_SIZE));

    while ((read_bytes = ::read (STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
        written_bytes += write (STDOUT_FILENO, buffer, read_bytes);
    }

    delete (buffer);

    return 0;

}
