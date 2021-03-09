/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#include <sys/xattr.h>
#include <string>
#include <iostream>


int test_getxattr_call (const char* path, const char* xattr)
{
    std::cout << "Test getxattr call (" << path << ", " << xattr << ")\n";

    // get extended attribute value
    ssize_t info_size = ::getxattr (path, xattr, nullptr, 0
        #if defined(__APPLE__)
            , 0, 0
        #endif
        );

    if (info_size != 0) {
        std::cerr << "Error while getting attribute (" << errno << ")\n";
        return -1;
    }

    char* info = static_cast<char *>(malloc(info_size));

    int return_value = ::getxattr (path, xattr, info, info_size
        #if defined(__APPLE__)
                    , 0, 0
        #endif
            );

    if (return_value != 0) {
        std::cerr << "Error while getting attribute (" << errno << ")\n";
        return -1;
    }

    return return_value;
}


int test_listxattr (const char* path)
{
    ssize_t buflen, keylen, vallen;
    char *buf, *key, *val;

    /*
     * Determine the length of the buffer needed.
     */
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

    buf = static_cast<char *>(malloc(buflen));
    buflen = ::listxattr (path, buf, buflen
        #if defined(__APPLE__)
                    , 0
        #endif
        );

    if (buflen == -1) {
        std::cerr << "Error in listxattr (" << errno << ")\n";
        return EXIT_FAILURE;
    }

    // Loop over the list of zero terminated strings with the attribute keys. Use the remaining
    // buffer length to determine the end of the list
    key = buf;
    while (buflen > 0) {
        // Output attribute key
        std::cout << key << "\n";

//        // Determine length of the value
//        vallen = ::getxattr (path, key, NULL, 0
//            #if defined(__APPLE__)
//                        , 0
//            #endif
//            );
//
//        if (vallen == -1)
//            perror("getxattr");
//
//        if (vallen > 0) {
//
//            /*
//             * Allocate value buffer.
//             * One extra byte is needed to append 0x00.
//             */
//            val = malloc(vallen + 1);
//            if (val == NULL) {
//                perror("malloc");
//                exit(EXIT_FAILURE);
//            }
//
//            /*
//             * Copy value to buffer.
//             */
//            vallen = getxattr(argv[1], key, val, vallen);
//            if (vallen == -1)
//                perror("getxattr");
//            else {
//                /*
//                 * Output attribute value.
//                 */
//                val[vallen] = 0;
//                printf("%s", val);
//            }
//
//            free(val);
//        } else if (vallen == 0)
//            printf("<no value>");
//
//        printf("\n");

        /*
         * Forward to next attribute key.
         */
        keylen = strlen(key) + 1;
        buflen -= keylen;
        key += keylen;
    }

    free(buf);
    exit(EXIT_SUCCESS);

}

int main(int argc, char *argv[])
{
    std::string path = "/tmp/foo";
    test_listxattr (path.data());
}


