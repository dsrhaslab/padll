/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

int test_mkdir_call (const char* pathname, mode_t mode)
{
    std::cout << "Test mkdir call\n";
    int result = ::mkdir (pathname, mode);

    if (result != 0) {
        std::cerr << "Error while creating directory (" << errno << ")\n";
    }

    return result;
}

int test_readdir_call (const char* pathname)
{
    std::cout << "Test readdir call\n";
    DIR* folder = ::opendir (pathname);

    if (folder == nullptr) {
        std::cerr << "Error while opening directory (" << errno << ")\n";
    }

    dirent* entry;
    int num_files = 0;

    while ((entry = ::readdir (folder))) {
        num_files++;
        std::cout << "file " << num_files << " {";
        std::cout << entry->d_ino << ", ";
        std::cout << entry->d_name << ", ";
        std::cout << entry->d_namlen << ", ";
        std::cout << entry->d_reclen << ", ";
        std::cout << entry->d_type << ", ";
        std::cout << entry->d_seekoff << "}\n";
    }

    int result = ::closedir (folder);
    if (result != 0) {
        std::cerr << "Error while closing directory (" << errno << ")\n";
    }

    return result;
}

/**
 * test_opendir_closedir_call:
 *
 * Validation: the statistic entries 'opendir' and 'closedir' of the Statistics' container reserved
 * for directory-based calls (PosixPassthrough::m_dir_stats), should contain:
 *  - {1, 0, 0} for valid paths (success);
 *  - {1, 0, 1} for invalid paths (error).
 * @param pathname
 * @return
 */
int test_opendir_closedir_call (const char* pathname)
{
    std::cout << "Test opendir and closedir calls\n";
    DIR* folder = ::opendir (pathname);

    if (folder == nullptr) {
        std::cerr << "Error while opening directory (" << errno << ")\n";
    }

    int result = ::closedir (folder);
    if (result != 0) {
        std::cerr << "Error while closing directory (" << errno << ")\n";
    }

    return result;
}

void test_rmdir_call ()
{
    std::cout << "Test rmdir calls\n";

}



int main (int argc, char** argv)
{
    int mkdir_result = test_mkdir_call (argv[1], 0777);
    test_opendir_closedir_call (argv[1]);

    if (mkdir_result == 0) {
        test_readdir_call (argv[1]);
    }

    return 0;
}