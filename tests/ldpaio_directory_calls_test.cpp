/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <dirent.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

/**
 * test_mkdir_call:
 * @param pathname
 * @param mode
 * @return
 */
int test_mkdir_call (const char* pathname, mode_t mode)
{
    std::cout << "Test mkdir call (" << pathname << ")\n";
    // create directory
    int result = ::mkdir (pathname, mode);

    // validate if creation was successful or not
    if (result != 0) {
        std::cerr << "Error while creating directory (" << errno << ")\n";
    }

    return result;
}

/**
 * test_mkdirat_call:
 * @param folder
 * @param path
 * @param mode
 * @return
 */
int test_mkdirat_call (DIR* folder, const char* path, mode_t mode)
{
    std::cout << "Test mkdirat call (" << path << ")\n";
    // get file descriptor from directory pointer
    int dirfd = ::dirfd (folder);

    // validate if directory file descriptor is valid
    if (dirfd == -1) {
        std::cerr << "Error while getting file descriptor from directory (" << errno << ")\n";
    }

    // create directory
    int result = ::mkdirat (dirfd, path, mode);

    // validate if creation was successful or not
    if (result != 0) {
        std::cerr << "Error while creating directory (" << errno << ")\n";
    }

    return result;
}

/**
 * test_readdir_call:
 * @param pathname
 */
int test_readdir_call (const char* pathname)
{
    std::cout << "Test readdir call (" << pathname << ")\n";
    // open directory
    DIR* folder = ::opendir (pathname);

    if (folder == nullptr) {
        std::cerr << "Error while opening directory (" << errno << ")\n";
    }

    dirent* entry;
    int num_files = 0;

    // read directory elements
    while ((entry = ::readdir (folder))) {
        num_files++;
        std::cout << "file " << num_files << " {";
        std::cout << entry->d_ino << ", ";
        std::cout << entry->d_name << ", ";
        std::cout << entry->d_reclen << ", ";
        std::cout << entry->d_type << "}\n";
    }

    // close directory
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
 * for directory-based calls (PosixPassthrough::m_dir_stats) should be updated.
 * @param pathname
 * @return
 */
int test_opendir_closedir_call (const char* pathname)
{
    std::cout << "Test opendir and closedir calls (" << pathname << ")\n";
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

/**
 * test_fdopendir_closedir_call:
 * @param pathname
 * @return
 */
int test_fdopendir_closedir_call (const char* pathname)
{
    std::cout << "Test opendir and closedir calls (" << pathname << ")\n";
    DIR* folder = ::opendir (pathname);

    // validate if directory was successfully open
    if (folder == nullptr) {
        std::cerr << "Error while opening directory (" << errno << ")\n";
    }

    // get file descriptor from directory pointer
    int dirfd = ::dirfd (folder);

    // validate if directory file descriptor is valid
    if (dirfd == -1) {
        std::cerr << "Error while getting file descriptor from directory (" << errno << ")\n";
    }

    // open directory through file descriptor
    DIR* new_folder = ::fdopendir (dirfd);

    // validate if directory was successfully open
    if (folder == nullptr) {
        std::cerr << "Error while opening directory (" << errno << ")\n";
    }

    int result = ::closedir (folder);
    if (result != 0) {
        std::cerr << "Error while closing directory (" << errno << ")\n";
    }

    result = ::closedir (new_folder);
    if (result != 0) {
        std::cerr << "Error while closing directory (" << errno << ")\n";
    }

    result = ::close (dirfd);
    if (result != 0) {
        std::cerr << "Error while closing directory (" << errno << ")\n";
    }

    return result;
}

/**
 * test_rmdir_call:
 * @param pathname
 */
void test_rmdir_call (const char* pathname)
{
    std::cout << "Test rmdir call (" << pathname << ")\n";
    int result = ::rmdir (pathname);

    if (result != 0) {
        std::cerr << "Error while removing directory (" << errno << ")\n";
    }
}

int main (int argc, char** argv)
{
    if (argc > 1) {
        test_mkdir_call (argv[1], 0777);
        test_opendir_closedir_call (argv[1]);
        test_readdir_call (argv[1]);
    } else {
        std::string path = "/tmp/newdir";

        test_mkdir_call (path.data (), 0777);
        test_opendir_closedir_call (path.data ());
        test_readdir_call (path.data ());
        test_rmdir_call (path.data ());
    }

    return 0;
}