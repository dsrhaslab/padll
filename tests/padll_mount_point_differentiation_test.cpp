/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <cstdio>
#include <iostream>
#include <padll/stage/mount_point_table.hpp>
#include <padll/utils/options.hpp>

using namespace padll::stage;

namespace padll::tests {

class MountPointDifferentiationTest {

private:
    FILE* m_fd;

public:
    /**
     * MountPointDifferentiationTest default constructor.
     */
    MountPointDifferentiationTest () = default;

    /**
     * MountPointDifferentiationTest (explicit) parameterized constructor.
     * @param fd
     */
    explicit MountPointDifferentiationTest (FILE* fd) : m_fd { fd }
    { }

    /**
     * MountPointDifferentiationTest default destructor.
     */
    ~MountPointDifferentiationTest () = default;
};
} // namespace padll::tests

namespace tests = padll::tests;

int main (int argc, char** argv)
{
    // check argv for the file to be placed the result
    FILE* fd = stdout;

    // open file to write the logging results
    if (argc > 1) {
        fd = ::fopen (argv[1], "w");

        if (fd == nullptr) {
            fd = stdout;
        }
    }

    tests::MountPointDifferentiationTest test { fd };
    MountPointTable mount_point_table {};

    std::cout << "Workflows: ";
    for (auto& elem : option_default_local_mount_point_workflows) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    return 0;
}