/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#include <sys/xattr.h>
#include <cstring>
#include <iostream>

/**
 * test_getxattr_call:
 * @param path
 * @param xattr
 * @return
 */
int test_getxattr_call (const char* path, const char* xattr)
{
    std::cout << "Test getxattr call (" << path << ", " << xattr << ")\n";

    // get size of extended attribute
    ssize_t info_size = ::getxattr (path, xattr, nullptr, 0
        #if defined(__APPLE__)
            , 0, 0
        #endif
        );

    if (info_size == 0) {
        std::cerr << "Error while getting attribute\n";
        return -1;
    }

    char* info = static_cast<char*> (malloc (info_size));

    // get extended attribute
    int return_value = ::getxattr (path, xattr, info, info_size
        #if defined(__APPLE__)
                    , 0, 0
        #endif
            );

    std::cout << "-> " << info  << " - " << info_size << " - " << return_value << "\n";

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

    // set extended attribute value
    int return_value = ::setxattr (path, xattr, value, std::strlen (value), 0
        #if defined(__APPLE__)
                    , 0
        #endif
        );

    if (return_value != 0) {
        std::cerr << "Error while getting attribute (" << errno << ")\n";
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

    // determine the length of the buffer needed.
    buflen = ::listxattr (path, nullptr, 0
        #if defined(__APPLE__)
                    , 0
        #endif
        );

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
    buf = static_cast<char*> (malloc (buflen));
    // list extended attribute elements of a given file
    buflen = ::listxattr (path, buf, buflen
        #if defined(__APPLE__)
            , 0
        #endif
        );

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

    free (buf);

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
        test_listxattr (path.data ());
        test_getxattr_call (path.data (), xattr.data ());
    } else {
        test_listxattr (argv[1]);
    }
}


