/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <sys/types.h>
#include <sys/xattr.h>
#include <thread>
#include <unistd.h>
#include <vector>

class ExtendedAttributesCallsTest {

private:
    FILE* m_fd { stdout };
    FILE* m_err { stderr };

    /**
     * create_file_pool:
     * @param dir_path
     * @param num_files
     * @param initial_files
     * @return
     */
    int create_file_pool (const char* dir_path, const int& num_files, const int& initial_files)
    {
        auto created_files = 0;
        // validate path
        std::filesystem::path dir_path_fs { dir_path };
        if (!std::filesystem::exists (dir_path_fs)) {
            std::fprintf (m_err, "Directory %s does not exist.\n", dir_path);
            return -1;
        }

        for (int i = initial_files; i < (num_files + initial_files); i++) {
            // generate file path
            std::string file_path = std::string (dir_path) + "/file-" + std::to_string (i);

            // create file
            int fd = ::open (file_path.c_str (), O_CREAT | O_WRONLY, 0666);

            // validate file descriptor
            if (fd < 0) {
                std::fprintf (m_err, "Error while creating file %s.\n", file_path.c_str ());
                return -1;
            }

            created_files++;
        }
        return created_files;
    }

    /**
     * remove_file_pool:
     * @param dir_path
     * @param num_files
     * @param initial_files
     * @return
     */
    int remove_file_pool (const char* dir_path, const int& num_files, const int& initial_files)
    {
        auto deleted_files = 0;
        // validate path
        std::filesystem::path dir_path_fs { dir_path };
        if (!std::filesystem::exists (dir_path_fs)) {
            std::fprintf (m_err, "Directory %s does not exist.\n", dir_path);
            return -1;
        }

        for (int i = initial_files; i < (num_files + initial_files); i++) {
            // generate file path
            std::string file_path = std::string (dir_path) + "/file-" + std::to_string (i);

            // remove file
            auto return_value = ::unlink (file_path.c_str ());

            // validate return value
            if (return_value < 0) {
                std::fprintf (m_err, "Error while removing file %s.\n", file_path.c_str ());
                return -1;
            }

            deleted_files++;
        }
        return 0;
    }

    /**
     * print_attribute_keys:
     * @param buf
     * @param key
     * @param buflen
     * @param keylen
     */
    void print_attribute_keys (char* buf, char* key, ssize_t& buflen, ssize_t& keylen)
    {
        key = buf;
        std::fprintf (this->m_fd, "Skipping over list elements ...\n");
        while (buflen > 0) {
            std::fprintf (this->m_fd, "\t%s\n", key);
            keylen = static_cast<ssize_t> (std::strlen (key)) + 1;
            buflen -= keylen;
            key += keylen;
        }
    }

    /**
     * test_getxattr_call:
     * @param path
     * @param xattr
     * @param debug
     * @return
     */
    ssize_t test_getxattr_call (const char* path, const char* xattr, const bool& debug)
    {
        if (debug) {
            std::fprintf (this->m_fd, "Test getxattr call (%s, %s)\n", path, xattr);
        }

        // use static buffer size
        ssize_t info_size = 200;

        // allocate buffer size to place the attribute value
        char* info = new char[info_size];
        ssize_t return_value;

        // get extended attribute
        // verify if the test is running on an Apple device and use the respective xattr calls
#if defined(__APPLE__)
        return_value = ::getxattr (path, xattr, info, info_size, 0, 0);
#else
        return_value = ::getxattr (path, xattr, info, info_size);
#endif

        if (debug) {
            // validate return value after getxattr
            std::fprintf (this->m_fd, "getxattr::result {%ld, %ld", info_size, return_value);

            (info_size > -1) ? std::fprintf (this->m_fd, ", %s}\n", info)
                             : std::fprintf (this->m_fd, "}\n");
        }

        delete[] info;
        return return_value;
    }

    /**
     * test_lgetxattr:
     * @param path
     * @param xattr
     * @param debug
     * @return
     */
    ssize_t test_lgetxattr_call (const char* path, const char* xattr, const bool& debug)
    {
        if (debug) {
            std::fprintf (this->m_fd, "Test lgetxattr call (%s, %s)\n", path, xattr);
        }

        // use static buffer size
        ssize_t info_size = 200;

        // allocate buffer size to place the attribute value
        char* info = new char[info_size];
        ssize_t return_value = 0;

        // get extended attribute
#if defined(__unix__) || defined(__linux__)
        return_value = ::lgetxattr (path, xattr, info, info_size);
#endif

        if (debug) {
            // validate return value after lgetxattr
            std::fprintf (this->m_fd, "lgetxattr::result {%ld, %ld", info_size, return_value);

            (info_size > -1) ? std::fprintf (this->m_fd, ", %s}\n", info)
                             : std::fprintf (this->m_fd, "}\n");
        }

        delete[] info;
        return return_value;
    }

    /**
     * test_fgetxattr_call:
     * @param fd
     * @param xattr
     * @param debug
     * @return
     */
    ssize_t test_fgetxattr_call (int fd, const char* xattr, const bool& debug)
    {
        if (debug) {
            std::fprintf (this->m_fd, "Test fgetxattr call (%d, %s)\n", fd, xattr);
        }

        // use static buffer size
        ssize_t info_size = 200;

        // allocate buffer size to place the attribute value
        char* info = new char[info_size];
        ssize_t return_value;

        // get extended attribute
        // verify if the test is running on an Apple device and use the respective xattr calls
#if defined(__APPLE__)
        return_value = ::fgetxattr (fd, xattr, info, info_size, 0, 0);
#else
        return_value = ::fgetxattr (fd, xattr, info, info_size);
#endif

        if (debug) {
            // validate return value after fgetxattr
            std::fprintf (this->m_fd, "fgetxattr::result {%ld, %ld", info_size, return_value);

            (info_size > -1) ? std::fprintf (this->m_fd, ", %s}\n", info)
                             : std::fprintf (this->m_fd, "}\n");
        }

        delete[] info;
        return return_value;
    }

    /**
     * test_setxattr_call:
     * @param path
     * @param xattr
     * @param value
     * @param debug
     * @return
     */
    ssize_t
    test_setxattr_call (const char* path, const char* xattr, const char* value, const bool& debug)
    {
        if (debug) {
            std::fprintf (this->m_fd, "Test setxattr call (%s, %s, %s)\n", path, xattr, value);
        }

        int return_value;
        // set new extended attribute value to a given file
        // verify if the test is running on an Apple device and use the respective xattr calls
#if defined(__APPLE__)
        return_value = ::setxattr (path, xattr, value, std::strlen (value), 0, 0);
#else
        return_value = ::setxattr (path, xattr, value, std::strlen (value), 0);
#endif

        // validate return_value
        if (return_value == -1) {
            std::fprintf (this->m_err,
                "Error while setting attribute (%s)\n",
                std::strerror (errno));
        }

        return return_value;
    }

    /**
     * test_lsetxattr_call:
     * @param path
     * @param xattr
     * @param value
     * @param debug
     * @return
     */
    int
    test_lsetxattr_call (const char* path, const char* xattr, const char* value, const bool& debug)
    {
        if (debug) {
            std::fprintf (this->m_fd, "Test lsetxattr call (%s, %s, %s)\n", path, xattr, value);
        }

        int return_value = -1;
        // set new extended attribute value to a given file
#if defined(__unix__) || defined(__linux__)
        return_value = ::lsetxattr (path, xattr, value, std::strlen (value), 0);
#endif

        // validate return_value
        if (return_value == -1) {
            std::fprintf (this->m_err,
                "Error while setting attribute (%s)\n",
                std::strerror (errno));
        }

        return return_value;
    }

    /**
     * test_fsetxattr_call:
     * @param fd
     * @param xattr
     * @param value
     * @return
     */
    int test_fsetxattr_call (int fd, const char* xattr, const char* value, const bool& debug)
    {
        if (debug) {
            std::fprintf (this->m_fd, "Test fsetxattr call (%d, %s, %s)\n", fd, xattr, value);
        }

        int return_value;
        // set new extended attribute value to a given file
        // verify if the test is running on an Apple device and use the respective xattr calls
#if defined(__APPLE__)
        return_value = ::fsetxattr (fd, xattr, value, std::strlen (value), 0, 0);
#else
        return_value = ::fsetxattr (fd, xattr, value, std::strlen (value), 0);
#endif

        // validate return value
        if (return_value == -1) {
            std::cerr << "Error while setting attribute (" << errno << ")\n";
        }

        return return_value;
    }

    /**
     * test_listxattr:
     * @param path
     * @return
     */
    int test_listxattr (const char* path, const bool& debug)
    {
        if (debug) {
            std::fprintf (this->m_fd, "Test listxattr call (%s)\n", path);
        }

        ssize_t buflen, keylen;
        char *buf, *key = nullptr;

        // determine the length of the buffer needed
        // verify if the test is running on an Apple device and use the respective xattr calls
#if defined(__APPLE__)
        buflen = ::listxattr (path, nullptr, 0, 0);
#else
        buflen = ::listxattr (path, nullptr, 0);
#endif

        switch (buflen) {
            case -1:
                std::fprintf (this->m_err, "Error in listxattr (%s)\n", std::strerror (errno));
                return EXIT_FAILURE;

            case 0:
                std::fprintf (this->m_fd, "%s has no attributes.\n", path);
                return EXIT_SUCCESS;

            default:
                break;
        }

        // allocate size for buffer
        buf = new char[buflen];

        // list extended attribute elements of a given file
        // verify if the test is running on an Apple device and use the respective xattr calls
#if defined(__APPLE__)
        buflen = ::listxattr (path, buf, buflen, 0);
#else
        buflen = ::listxattr (path, buf, buflen);
#endif

        // validate return value
        if (buflen == -1) {
            std::fprintf (this->m_err, "Error in listxattr (%s)\n", std::strerror (errno));
            return EXIT_FAILURE;
        }

        // loop over the list of zero terminated strings with the attribute keys
        if (debug) {
            this->print_attribute_keys (buf, key, buflen, keylen);
        }

        delete[] buf;
        return EXIT_SUCCESS;
    }

    /**
     * test_llistxattr:
     * @param path
     * @param debug
     * @return
     */
    int test_llistxattr (const char* path, const bool& debug)
    {
        if (debug) {
            std::fprintf (this->m_fd, "Test llistxattr call (%s)\n", path);
        }

        ssize_t buflen = 0, keylen;
        char *buf, *key = nullptr;

        // determine the length of the buffer needed
#if defined(__unix__) || defined(__linux__)
        buflen = ::llistxattr (path, nullptr, 0);
#endif

        switch (buflen) {
            case -1:
                std::fprintf (this->m_err, "Error in llistxattr (%s)\n", std::strerror (errno));
                return EXIT_FAILURE;

            case 0:
                std::fprintf (this->m_fd, "%s has no attributes.\n", path);
                return EXIT_SUCCESS;

            default:
                break;
        }

        // allocate size for buffer
        buf = new char[buflen];

        // list extended attribute elements of a given file
#if defined(__unix__) || defined(__linux__)
        buflen = ::llistxattr (path, buf, buflen);
#endif

        // validate return value
        if (buflen == -1) {
            std::fprintf (this->m_err, "Error in llistxattr (%s)\n", std::strerror (errno));
            return EXIT_FAILURE;
        }

        // loop over the list of zero terminated strings with the attribute keys
        if (debug) {
            this->print_attribute_keys (buf, key, buflen, keylen);
        }

        delete[] buf;
        return EXIT_SUCCESS;
    }

    /**
     * test_flistxattr:
     * @param fd
     * @param debug
     * @return
     */
    int test_flistxattr (int fd, const bool& debug)
    {
        if (debug) {
            std::fprintf (this->m_fd, "Test flistxattr call (%d)\n", fd);
        }

        ssize_t buflen, keylen;
        char *buf, *key = nullptr;

        // determine the length of the buffer needed
        // verify if the test is running on an Apple device and use the respective xattr calls
#if defined(__APPLE__)
        buflen = ::flistxattr (fd, nullptr, 0, 0);
#else
        buflen = ::flistxattr (fd, nullptr, 0);
#endif

        switch (buflen) {
            case -1:
                std::fprintf (this->m_err, "Error in flistxattr (%s)\n", std::strerror (errno));
                return EXIT_FAILURE;

            case 0:
                std::fprintf (this->m_fd, "%d has no attributes.\n", fd);
                return EXIT_SUCCESS;

            default:
                break;
        }

        // allocate size for buffer
        buf = new char[buflen];

        // list extended attribute elements of a given file
        // verify if the test is running on an Apple device and use the respective xattr calls
#if defined(__APPLE__)
        buflen = ::flistxattr (fd, buf, buflen, 0);
#else
        buflen = ::flistxattr (fd, buf, buflen);
#endif

        // validate return value
        if (buflen == -1) {
            std::fprintf (this->m_err, "Error in flistxattr (%s)\n", std::strerror (errno));
            return EXIT_FAILURE;
        }

        // loop over the list of zero terminated strings with the attribute keys
        if (debug) {
            this->print_attribute_keys (buf, key, buflen, keylen);
        }

        delete[] buf;
        return EXIT_SUCCESS;
    }

    /**
     * test_removexattr:
     * @param path
     * @param name
     * @return
     */
    int test_removexattr (const char* path, const char* name, const bool& debug)
    {
        if (debug) {
            std::fprintf (this->m_fd, "Test removexattr call (%s, %s)\n", path, name);
        }

        int return_value;
        // remove extended attribute value to a given file
        // verify if the test is running on an Apple device and use the respective xattr calls
#if defined(__APPLE__)
        return_value = ::removexattr (path, name, 0);
#else
        return_value = ::removexattr (path, name);
#endif

        // validate return_value
        if (return_value != 0) {
            std::fprintf (this->m_err,
                "Error while removing attribute (%s)\n",
                std::strerror (errno));
        }

        return return_value;
    }

    /**
     * test_lremovexattr:
     * @param path
     * @param name
     * @return
     */
    int test_lremovexattr (const char* path, const char* name, const bool& debug)
    {
        if (debug) {
            std::fprintf (this->m_fd, "Test lremovexattr call (%s, %s)\n", path, name);
        }

        int return_value = -1;
        // remove extended attribute value to a given file
        // verify if the test is running on an Apple device and use the respective xattr calls
#if defined(__unix__) || defined(__linux__)
        return_value = ::lremovexattr (path, name);
#endif

        // validate return_value
        if (return_value != 0) {
            std::fprintf (this->m_err,
                "Error while removing attribute (%s)\n",
                std::strerror (errno));
        }

        return return_value;
    }

    /**
     * test_fremovexattr:
     * @param path
     * @param name
     * @param debug
     * @return
     */
    int test_fremovexattr (int fd, const char* name, const bool& debug)
    {
        if (debug) {
            std::fprintf (this->m_fd, "Test fremovexattr call (%d, %s)\n", fd, name);
        }

        int return_value;
        // remove extended attribute value to a given file
        // verify if the test is running on an Apple device and use the respective xattr calls
#if defined(__APPLE__)
        return_value = ::fremovexattr (fd, name, 0);
#else
        return_value = ::fremovexattr (fd, name);
#endif

        // validate return_value
        if (return_value != 0) {
            std::fprintf (this->m_err,
                "Error while removing attribute (%s)\n",
                std::strerror (errno));
        }

        return return_value;
    }

public:
    ExtendedAttributesCallsTest () = default;

    /**
     * ExtendedAttributesCallsTest explict constructor.
     * @param fd Pointer to a file descriptor.
     */
    explicit ExtendedAttributesCallsTest (FILE* fd) : m_fd { fd }
    { }

    /**
     * simple_ext_attributes_test:
     * @param path
     * @param xattr
     * @param value
     * @param debug
     */
    void simple_ext_attributes_test (const std::string& path,
        const std::string& xattr,
        const std::string& value,
        const bool& debug)
    {
        ssize_t return_value
            = this->test_setxattr_call (path.data (), xattr.data (), value.data (), debug);
        std::fprintf (this->m_fd, "setxattr (%ld)\n", return_value);

        return_value = this->test_listxattr (path.data (), debug);
        std::fprintf (this->m_fd, "listxattr (%ld)\n", return_value);

        return_value = this->test_getxattr_call (path.data (), xattr.data (), debug);
        std::fprintf (this->m_fd, "getxattr (%ld)\n", return_value);

        return_value = this->test_removexattr (path.data (), xattr.data (), debug);
        std::fprintf (this->m_fd, "removexattr (%ld)\n", return_value);
    }

    /**
     * simple_lext_attributes_test:
     * @param path
     * @param xattr
     * @param value
     */
    void simple_lext_attributes_test (const std::string& path,
        const std::string& xattr,
        const std::string& value,
        const bool& debug)
    {
        ssize_t return_value
            = this->test_lsetxattr_call (path.data (), xattr.data (), value.data (), debug);
        std::fprintf (this->m_fd, "lsetxattr (%ld)\n", return_value);

        return_value = this->test_llistxattr (path.data (), debug);
        std::fprintf (this->m_fd, "llistxattr (%ld)\n", return_value);

        return_value = this->test_lgetxattr_call (path.data (), xattr.data (), debug);
        std::fprintf (this->m_fd, "lgetxattr (%ld)\n", return_value);

        return_value = this->test_lremovexattr (path.data (), xattr.data (), debug);
        std::fprintf (this->m_fd, "lremovexattr (%ld)\n", return_value);
    }

    /**
     * simple_fext_attributes_test:
     * @param path
     * @param xattr
     * @param value
     * @param debug
     */
    void simple_fext_attributes_test (const std::string& path,
        const std::string& xattr,
        const std::string& value,
        const bool& debug)
    {
        int fd = ::open (path.data (), O_RDWR);
        if (fd == -1) {
            std::cerr << "Error while opening file " << path << ".\n";
            return;
        }

        ssize_t return_value = this->test_fsetxattr_call (fd, xattr.data (), value.data (), debug);
        std::fprintf (this->m_fd, "fsetxattr (%ld)\n", return_value);

        return_value = this->test_flistxattr (fd, debug);
        std::fprintf (this->m_fd, "flistxattr (%ld)\n", return_value);

        return_value = this->test_fgetxattr_call (fd, xattr.data (), debug);
        std::fprintf (this->m_fd, "fgetxattr (%ld)\n", return_value);

        return_value = this->test_fremovexattr (fd, xattr.data (), debug);
        std::fprintf (this->m_fd, "fremovexattr (%ld)\n", return_value);

        ::close (fd);
    }

    /**
     * select_extended_attributes_test:
     * @param type
     * @param path
     * @param xattr
     * @param value
     */
    void simple_extended_attributes_test (int type,
        const std::string& path,
        const std::string& xattr,
        const std::string& value,
        const bool& debug)
    {
        switch (type) {
            case 0:
                this->simple_ext_attributes_test (path, xattr, value, debug);
                break;

            case 1:
                this->simple_lext_attributes_test (path, xattr, value, debug);
                break;

            case 2:
                this->simple_fext_attributes_test (path, xattr, value, debug);
                break;

            default:
                break;
        }
    }

    void set_get_list_ext_attributes_test (const int type,
        const int workers,
        const int iterations,
        const char* dir_path,
        const int num_files,
        const int initial_file_index)
    {
        // create list of files
        this->create_file_pool (dir_path, num_files, initial_file_index);

        auto func = [this] (int type,
                        int iterations,
                        const char* dir_path,
                        int num_files,
                        int initial_file_index) {
            // implement loop for setting, getting, and listing extended attributes of a file
            for (int i = 0; i < iterations; i++) {
                // pick random file
                int file_index = static_cast<int> ((random () % num_files)) + initial_file_index;
                std::string file_path
                    = std::string (dir_path) + "/file-" + std::to_string (file_index);

                this->simple_extended_attributes_test (type, file_path, "user.test", "test", false);
            }
        };

        // spawn N threads to perform this task
        std::vector<std::thread> threads;
        threads.reserve (workers);
        for (int i = 0; i < workers; i++) {
            threads.emplace_back (func, type, iterations, dir_path, num_files, initial_file_index);
        }

        // wait for all threads to finish
        for (int i = 0; i < workers; i++) {
            threads[i].join ();
        }

        // remove list of files
        this->remove_file_pool (dir_path, num_files, initial_file_index);
    }
};

/**
 * TODO:
 *  - test independent function tests;
 *  - create setxattr_getxattr_listxattr_test, lsetxattr_lgetxattr_llistxattr_test, and
 *  fsetxattr_fgetxattr_flistxattr_test using a loop to control the amount of each operation, or
 *  even all operations, using PAIO.
 */
int main (int argc, [[maybe_unused]] char** argv)
{
    ExtendedAttributesCallsTest test {};

    bool debug_detailed_messages = true;

    if (argc == 1) {
        int type = 0;
        std::string path = "/tmp/foo";
        std::string xattr = "user.tmp";
        std::string value = "xyz-value";

        test.simple_extended_attributes_test (type, path, xattr, value, debug_detailed_messages);

        int workers = 1;
        int iterations = 10;
        int num_files = 10;
        int files_index = 0;

        test.set_get_list_ext_attributes_test (0,
            workers,
            iterations,
            "/tmp/",
            num_files,
            files_index);
    }

    return 0;
}
