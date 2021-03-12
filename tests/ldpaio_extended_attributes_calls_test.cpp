/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/types.h>
#include <sys/xattr.h>
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
    char* info = new char[info_size];

    ssize_t return_value;
    // get extended attribute
    // verify if the test is running on an Apple device and use the respective xattr calls
#if defined(__APPLE__)
    return_value = ::getxattr (path, xattr, info, info_size, 0, 0);
#else
    return_value = ::getxattr (path, xattr, info, info_size);
#endif

    std::cout << "\tresult {" << info << ", " << info_size << ", " << return_value << "}\n";

    delete[] info;
    return return_value;
}

/**
 * test_lgetxattr:
 * @param path
 * @param xattr
 * @return
 */
int test_lgetxattr_call (const char* path, const char* xattr)
{
    std::cout << "Test lgetxattr call (" << path << ", " << xattr << ")\n";

    ssize_t info_size = 0;
    // get size of extended attribute
#if defined(__unix__) || defined(__linux__)
    info_size = ::lgetxattr (path, xattr, nullptr, 0);
#endif

    // validate info_size result after lgetxattr
    if (info_size == 0) {
        std::cerr << "Error while getting attribute\n";
        return -1;
    }

    // allocate size for
    char* info = new char[info_size];

    ssize_t return_value = 0;
    // get extended attribute
#if defined(__unix__) || defined(__linux__)
    return_value = ::lgetxattr (path, xattr, info, info_size);
#endif

    std::cout << "\tresult {" << info << ", " << info_size << ", " << return_value << "}\n";

    delete[] info;
    return return_value;
}

/**
 * test_fgetxattr_call:
 * @param fd
 * @param xattr
 * @return
 */
int test_fgetxattr_call (int fd, const char* xattr)
{
    std::cout << "Test fgetxattr call (" << fd << ", " << xattr << ")\n";

    ssize_t info_size;
    // get size of extended attribute
    // verify if the test is running on an Apple device and use the respective xattr calls
#if defined(__APPLE__)
    info_size = ::fgetxattr (fd, xattr, nullptr, 0, 0, 0);
#else
    info_size = ::fgetxattr (fd, xattr, nullptr, 0);
#endif

    // validate info_size result after fgetxattr
    if (info_size == 0) {
        std::cerr << "Error while getting attribute\n";
        return -1;
    }

    // allocate size for
    char* info = new char[info_size];

    ssize_t return_value;
    // get extended attribute
    // verify if the test is running on an Apple device and use the respective xattr calls
#if defined(__APPLE__)
    return_value = ::fgetxattr (fd, xattr, info, info_size, 0, 0);
#else
    return_value = ::fgetxattr (fd, xattr, info, info_size);
#endif

    std::cout << "\tresult {" << info << ", " << info_size << ", " << return_value << "}\n";

    delete[] info;
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
        std::cerr << "Error while setting attribute (" << errno << ")\n";
    }

    return return_value;
}

/**
 * test_lsetxattr_call:
 * @param path
 * @param xattr
 * @param value
 * @return
 */
int test_lsetxattr_call (const char* path, const char* xattr, const char* value)
{
    std::cout << "Test lsetxattr call (" << path << ", " << xattr << ", " << value << ")\n";

    int return_value = -1;
    // set new extended attribute value to a given file
#if defined(__unix__) || defined(__linux__)
    return_value = ::lsetxattr (path, xattr, value, std::strlen (value), 0);
#endif

    // validate return_value
    if (return_value != 0) {
        std::cerr << "Error while setting attribute (" << errno << ")\n";
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
int test_fsetxattr_call (int fd, const char* xattr, const char* value)
{
    std::cout << "Test setxattr call (" << fd << ", " << xattr << ", " << value << ")\n";

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
        std::cerr << "Error while setting attribute (" << errno << ")\n";
    }

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
        keylen = std::strlen (key) + 1;
        buflen -= keylen;
        key += keylen;
    }

    delete[] buf;
    return EXIT_SUCCESS;
}

/**
 * test_llistxattr:
 * @param path
 * @return
 */
int test_llistxattr (const char* path)
{
    std::cout << "Test llistxattr call (" << path << ")\n";

    ssize_t buflen = 0, keylen;
    char *buf, *key;

    // determine the length of the buffer needed
#if defined(__unix__) || defined(__linux__)
    buflen = ::llistxattr (path, nullptr, 0);
#endif

    switch (buflen) {
        case -1:
            std::cerr << "Error in llistxattr (" << errno << ")\n";
            return EXIT_FAILURE;

        case 0:
            std::cout << path << " has no attributes.\n";
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
        std::cerr << "Error in llistxattr (" << errno << ")\n";
        return EXIT_FAILURE;
    }

    // loop over the list of zero terminated strings with the attribute keys
    key = buf;
    while (buflen > 0) {
        std::cout << key << "\n";
        keylen = std::strlen (key) + 1;
        buflen -= keylen;
        key += keylen;
    }

    delete[] buf;
    return EXIT_SUCCESS;
}

/**
 * test_flistxattr:
 * @param path
 * @return
 */
int test_flistxattr (int fd)
{
    std::cout << "Test listxattr call (" << fd << ")\n";

    ssize_t buflen, keylen;
    char *buf, *key;

    // determine the length of the buffer needed
    // verify if the test is running on an Apple device and use the respective xattr calls
#if defined(__APPLE__)
    buflen = ::flistxattr (fd, nullptr, 0, 0);
#else
    buflen = ::flistxattr (fd, nullptr, 0);
#endif

    switch (buflen) {
        case -1:
            std::cerr << "Error in flistxattr (" << errno << ")\n";
            return EXIT_FAILURE;

        case 0:
            std::cout << fd << " has no attributes.\n";
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
        std::cerr << "Error in listxattr (" << errno << ")\n";
        return EXIT_FAILURE;
    }

    // loop over the list of zero terminated strings with the attribute keys
    key = buf;
    while (buflen > 0) {
        std::cout << key << "\n";
        keylen = std::strlen (key) + 1;
        buflen -= keylen;
        key += keylen;
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
int test_removexattr (const char* path, const char* name)
{
    std::cout << "Test removexattr call (" << path << ", " << name << ")\n";

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
        std::cerr << "Error while removing attribute (" << errno << ")\n";
    }

    return return_value;
}

/**
 * test_lremovexattr:
 * @param path
 * @param name
 * @return
 */
int test_lremovexattr (const char* path, const char* name)
{
    std::cout << "Test lremovexattr call (" << path << ", " << name << ")\n";

    int return_value = -1;
    // remove extended attribute value to a given file
    // verify if the test is running on an Apple device and use the respective xattr calls
#if defined(__unix__) || defined(__linux__)
    return_value = ::lremovexattr (path, name);
#endif

    // validate return_value
    if (return_value != 0) {
        std::cerr << "Error while removing attribute (" << errno << ")\n";
    }

    return return_value;
}

/**
 * test_fremovexattr:
 * @param path
 * @param name
 * @return
 */
int test_fremovexattr (int fd, const char* name)
{
    std::cout << "Test fremovexattr call (" << fd << ", " << name << ")\n";

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
        std::cerr << "Error while removing attribute (" << errno << ")\n";
    }

    return return_value;
}

/**
 * test_ext_attributes:
 * @param path
 * @param xattr
 * @param value
 */
void test_ext_attributes (const std::string& path,
    const std::string& xattr,
    const std::string& value)
{
    int return_value = test_setxattr_call (path.data (), xattr.data (), value.data ());
    std::cout << "setxattr (" << return_value << ")\n";

    return_value = test_listxattr (path.data ());
    std::cout << "listxattr (" << return_value << ")\n";

    return_value = test_getxattr_call (path.data (), xattr.data ());
    std::cout << "getxattr (" << return_value << ")\n";

    return_value = test_removexattr (path.data (), xattr.data ());
    std::cout << "removexattr (" << return_value << ")\n";
}

/**
 * test_lext_attributes:
 * @param path
 * @param xattr
 * @param value
 */
void test_lext_attributes (const std::string& path,
    const std::string& xattr,
    const std::string& value)
{
    int return_value = test_lsetxattr_call (path.data (), xattr.data (), value.data ());
    std::cout << "lsetxattr (" << return_value << ")\n";

    return_value = test_llistxattr (path.data ());
    std::cout << "llistxattr (" << return_value << ")\n";

    return_value = test_lgetxattr_call (path.data (), xattr.data ());
    std::cout << "lgetxattr (" << return_value << ")\n";

    return_value = test_lremovexattr (path.data (), xattr.data ());
    std::cout << "lremovexattr (" << return_value << ")\n";
}

/**
 * test_fext_attributes:
 * @param path
 * @param xattr
 * @param value
 */
void test_fext_attributes (const std::string& path,
    const std::string& xattr,
    const std::string& value)
{
    int fd = ::open (path.data (), O_RDWR);
    if (fd == -1) {
        std::cerr << "Error while opening file " << path << ".\n";
        return;
    }

    int return_value = test_fsetxattr_call (fd, xattr.data (), value.data ());
    std::cout << "fsetxattr (" << return_value << ")\n";

    return_value = test_flistxattr (fd);
    std::cout << "flistxattr (" << return_value << ")\n";

    return_value = test_fgetxattr_call (fd, xattr.data ());
    std::cout << "fgetxattr (" << return_value << ")\n";

    return_value = test_fremovexattr (fd, xattr.data ());
    std::cout << "fremovexattr (" << return_value << ")\n";

    ::close (fd);
}

/**
 * select_extended_attributes_test:
 * @param type
 * @param path
 * @param xattr
 * @param value
 */
void select_extended_attributes_test (int type,
    const std::string& path,
    const std::string& xattr,
    const std::string& value)
{
    switch (type) {
        case 0:
            test_ext_attributes (path, xattr, value);
            break;

        case 1:
            test_lext_attributes (path, xattr, value);
            break;

        case 2:
            test_fext_attributes (path, xattr, value);
            break;

        default:
            break;
    }
}

int main (int argc, char** argv)
{
    if (argc == 1) {
        int type = 0;
        std::string path = "/tmp/foo";
        std::string xattr = "user.tmp";
        std::string value = "xyz-value";

        select_extended_attributes_test (type, path, xattr, value);
    } else {
        test_listxattr (argv[1]);
    }
}
