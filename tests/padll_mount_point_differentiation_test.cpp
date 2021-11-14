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

        /*
         * test_extract_mount_point_from_path:
         */
        void test_extract_mount_point_from_path () {
            std::fprintf (this->m_fd, "");
        }

        /*
         * test_pick_workflow_id:
         */
        void test_pick_workflow_id () {
            std::fprintf (this->m_fd, "");
        }

        /**
         * test_register_mount_point_type:
         */
        void test_register_mount_point_type (MountPointTable* table_ptr) {
            std::fprintf (this->m_fd, "test_register_mount_point_type:\n");
            // register operations is conducted on initialize (which is executed in the constructor)

            std::stringstream stream;
            if (option_mount_point_differentiation) {
                stream << "Local workflows: ";
                for (auto &elem: option_default_local_mount_point_workflows) {
                    stream << elem << " ";
                }
                stream << std::endl;

                stream << "Remote workflows: ";
                for (auto &elem: option_default_remote_mount_point_workflows) {
                    stream << elem << " ";
                }
                stream << std::endl;
            } else {
                stream << "All workflows: ";
                for (auto& elem : option_default_mount_point_workflows) {
                    stream << elem << " ";
                }
                stream << std::endl;
            }

            std::fprintf (this->m_fd, "%s\n", stream.str().c_str());
            std::fprintf (this->m_fd, "------------------\n");
        }

        /**
         * test_sequential_create_mount_point_entry:
         */
        void test_sequential_create_mount_point_entry () {
            std::fprintf (this->m_fd, "");
        }

        /**
         * test_sequential_get_mount_point_entry:
         */
        void test_sequential_get_mount_point_entry () {
            std::fprintf (this->m_fd, "");
        }

        /**
         * test_sequential_remove_mount_point_entry:
         */
        void test_sequential_remove_mount_point_entry () {
            std::fprintf (this->m_fd, "");
        }

        /**
         * test_concurrent_create_mount_point_entry:
         */
        void test_concurrent_create_mount_point_entry () {
            std::fprintf (this->m_fd, "");
        }

        /**
         * test_concurrent_get_mount_point_entry:
         */
        void test_concurrent_get_mount_point_entry () {
            std::fprintf (this->m_fd, "");
        }

        /**
         * test_concurrent_remove_mount_point_entry:
         */
        void test_concurrent_remove_mount_point_entry () {
            std::fprintf (this->m_fd, "");
        }

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
    // MountPointTable mount_point_table {"test" };



    return 0;
}