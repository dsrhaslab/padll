/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#include <sys/xattr.h>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

/**
 * test_getxattr_call:
 * @param path
 * @param xattr
 * @return
 */
int test_getxattr_call (const char* path, const char* xattr)
{
    std::cout << "Test getxattr call (" << path << ", " << xattr << ")\n";

    ssize_t info_size;
    // get size of extended attribute
    // verify if the test is running on an Apple device and use the respective xattr calls
    #if defined(__APPLE__)
        info_size = ::getxattr (path, xattr, nullptr, 0, 0, 0);
    #else
        info_size = ::getxattr (path, xattr, nullptr, 0);
    #endif

    // validate info_size result after getxattr
    if (info_size == 0) {
        std::cerr << "Error while getting attribute\n";
        return -1;
    }

    // allocate size for
//    char* info = static_cast<char*> (malloc (info_size));
    char* info = new char[info_size];

    ssize_t return_value;
    // get extended attribute
    // verify if the test is running on an Apple device and use the respective xattr calls
    #if defined(__APPLE__)
        return_value = ::getxattr (path, xattr, info, info_size, 0, 0);
    #else
        return_value = ::getxattr (path, xattr, info, info_size);
    #endif

    std::cout << "\tresult {" << info  << ", " << info_size << ", " << return_value << "}\n";

    delete [] info;

    return return_value;
}

/**
 * test_setxattr_call:
 * @param path
 * @param xattr
 * @param value
 * @return
 */
int test_setxattr_call (const char* path, const char* xattr, const char* value)
{
    std::cout << "Test setxattr call (" << path << ", " << xattr << ", " << value << ")\n";

    int return_value;
    // set new extended attribute value to a given file
    // verify if the test is running on an Apple device and use the respective xattr calls
    #if defined(__APPLE__)
        return_value = ::setxattr (path, xattr, value, std::strlen (value), 0, 0);
    #else
        return_value = ::setxattr (path, xattr, value, std::strlen (value), 0);
    #endif

    // validate return_value
    if (return_value != 0) {
        std::cerr << "Error while getting attribute (" << errno << ")\n";
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
int test_fsetxattr_call (const char* path, const char* xattr, const char* value)
{
    std::cout << "Test setxattr call (" << path << ", " << xattr << ", " << value << ")\n";

    int fd = ::open (path, O_RDWR);
    int return_value;
    // set new extended attribute value to a given file
    // verify if the test is running on an Apple device and use the respective xattr calls
    #if defined(__APPLE__)
        return_value = ::fsetxattr (fd, xattr, value, std::strlen (value), 0, 0);
    #else
        return_value = ::fsetxattr (fd, xattr, value, std::strlen (value), 0);
    #endif

    // validate return value
    if (return_value != 0) {
        std::cerr << "Error while getting attribute (" << errno << ")\n";
    }

    ::close (fd);

    return return_value;
}

/**
 * test_listxattr:
 * @param path
 * @return
 */
int test_listxattr (const char* path)
{
    std::cout << "Test listxattr call (" << path << ")\n";

    ssize_t buflen, keylen;
    char *buf, *key;

    // determine the length of the buffer needed
    // verify if the test is running on an Apple device and use the respective xattr calls
    #if defined(__APPLE__)
        buflen = ::listxattr (path, nullptr, 0, 0);
    #else
        buflen = ::listxattr (path, nullptr, 0);
    #endif

    switch (buflen) {
        case -1:
            std::cerr << "Error in listxattr (" << errno << ")\n";
            return EXIT_FAILURE;

        case 0:
            std::cout << path << " has no attributes.\n";
            return EXIT_SUCCESS;

        default:
            break;
    }

    // allocate size for buffer
    // buf = static_cast<char*> (malloc (buflen));
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
        std::cerr << "Error in listxattr (" << errno << ")\n";
        return EXIT_FAILURE;
    }

    // loop over the list of zero terminated strings with the attribute keys
    key = buf;
    while (buflen > 0) {
        std::cout << key << "\n";
        keylen = std::strlen(key) + 1;
        buflen -= keylen;
        key += keylen;
    }

    // free (buf);
    delete [] buf;

    return EXIT_SUCCESS;
}

int main (int argc, char** argv)
{
    if (argc == 1) {
        std::string path = "/tmp/foo";
        std::string xattr = "user.tmp";
        std::string value = "xyz-value";

        int return_value = test_setxattr_call (path.data (), xattr.data (), value.data ());
        std::cout << "setxattr (" << return_value << ")\n";

        return_value = test_fsetxattr_call (path.data(), xattr.data (), value.data());
        std::cout << "fsetxattr (" << return_value << ")\n";

        return_value = test_listxattr (path.data ());
        std::cout << "listxattr (" << return_value << ")\n";

        return_value = test_getxattr_call (path.data (), xattr.data ());
        std::cout << "getxattr (" << return_value << ")\n";
    } else {
        test_listxattr (argv[1]);
    }
}


