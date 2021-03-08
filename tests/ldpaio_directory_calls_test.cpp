/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <iostream>
#include <dirent.h>

void test_mkdir_call ()
{
    std::cout << "Test mkdir call\n";
}

// readdir: https://c-for-dummies.com/blog/?p=3246
void test_readdir_call (const std::string& pathname)
{
    std::cout << "Test readdir call\n";
}

/**
 * test_opendir_call:
 *
 * Validation: the StatisticsEntry 'opendir' of the Statistics' container reserved for
 * directory-based calls (PosixPassthrough::m_m_dir_stats), should contain:
 *  - {1, 0, 0} for valid paths (success);
 *  - {1, 0, 1} for invalid paths (error).
 * @param pathname
 * @return
 */
int test_opendir_call (const char* pathname)
{
    std::cout << "Test opendir call\n";
    DIR* folder = ::opendir (pathname);

    if (folder == nullptr) {
        std::cerr << "Error while reading directory (" << errno << ")\n";
        return 1;
    }

    return 0;
}

void test_closedir_call ()
{
    std::cout << "Test closedir call\n";
}

void test_rmdir_call ()
{
    std::cout << "Test rmdir call\n";
}

int main (int argc, char** argv)
{

    test_opendir_call (argv[1]);
    return 0;
}