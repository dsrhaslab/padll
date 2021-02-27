/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#include <ldpaio/interface/posix_file_system.hpp>
#include <cstring>
#include <iostream>

ssize_t read (int fd, void* buf, size_t size)
{
    std::cout << "One more read ...\n";
    return 0;
}