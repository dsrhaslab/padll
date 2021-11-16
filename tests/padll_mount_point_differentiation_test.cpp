/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <cstdio>
#include <iostream>
#include <padll/stage/mount_point_table.hpp>

using namespace padll::stage;

namespace padll::tests {

class MountPointDifferentiationTest {

private:
    FILE* m_fd { stdout };

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
    void test_extract_mount_point (MountPointTable* table_ptr)
    {
        std::fprintf (this->m_fd, "------------------\n");
        std::fprintf (this->m_fd, "test_extract_mount_point:\n");

        std::vector<std::string_view> file_paths { "/home/user/file1",
            "/local/file2",
            "/remote/path/to/file3",
            "/local/path/to/file4" };

        for (auto& elem : file_paths) {
            auto mount_point = table_ptr->extract_mount_point (elem);
            std::fprintf (this->m_fd,
                "\tfile_path: %s -- %s\n",
                elem.data (),
                (mount_point == MountPoint::kLocal)
                    ? "local"
                    : ((mount_point == MountPoint::kRemote) ? "remote" : "none"));
        }

        std::fprintf (this->m_fd, "------------------\n");
    }

    /*
     * test_pick_workflow_id:
     */
    void test_pick_workflow_id ()
    {
        std::fprintf (this->m_fd, "");
    }

    /**
     * test_register_mount_point_type:
     */
    void test_register_mount_point_type (MountPointTable* table_ptr)
    {
        std::fprintf (this->m_fd, "------------------\n");
        std::fprintf (this->m_fd, "test_register_mount_point_type:\n");
        // register operations is conducted on initialize (which is executed in the constructor)

        std::stringstream stream;
        if (option_mount_point_differentiation) {
            stream << "\tLocal workflows: ";
            for (auto& elem :
                table_ptr->get_default_workflows ().default_local_mount_point_workflows) {
                stream << elem << " ";
            }
            stream << std::endl;

            stream << "\tRemote workflows: ";
            for (auto& elem :
                table_ptr->get_default_workflows ().default_remote_mount_point_workflows) {
                stream << elem << " ";
            }
            stream << std::endl;
        } else {
            stream << "\tAll workflows: ";
            for (auto& elem : table_ptr->get_default_workflows ().default_mount_point_workflows) {
                stream << elem << " ";
            }
            stream << std::endl;
        }

        std::fprintf (this->m_fd, "%s", stream.str ().c_str ());
        std::fprintf (this->m_fd, "------------------\n");
    }

    /**
     * test_sequential_create_mount_point_entry:
     */
    void test_sequential_create_mount_point_entry (MountPointTable* table_ptr,
        bool create_fd,
        const std::string& path,
        int num_files)
    {
        std::fprintf (this->m_fd, "------------------\n");
        std::fprintf (this->m_fd, "test_pick_workflow_id:\n");

        if (create_fd) {
            for (int i = 0; i < num_files; i++) {

                // todo: finish me ...
                auto return_value = ::open ((path + std::to_string (i)).c_str (), O_CREAT, 0666);
                if (return_value == -1) {
                    std::fprintf (this->m_fd, "Error: %s\n", strerror (errno));
                    return;
                }

                // table_ptr->create_mount_point_entry (return_value, path + std::to_string (i));
            }
        }

        std::fprintf (this->m_fd, "------------------\n");
    }

    /**
     * test_sequential_get_mount_point_entry:
     */
    void test_sequential_get_mount_point_entry ()
    {
        std::fprintf (this->m_fd, "");
    }

    /**
     * test_sequential_remove_mount_point_entry:
     */
    void test_sequential_remove_mount_point_entry ()
    {
        std::fprintf (this->m_fd, "");
    }

    /**
     * test_concurrent_create_mount_point_entry:
     */
    void test_concurrent_create_mount_point_entry ()
    {
        std::fprintf (this->m_fd, "");
    }

    /**
     * test_concurrent_get_mount_point_entry:
     */
    void test_concurrent_get_mount_point_entry ()
    {
        std::fprintf (this->m_fd, "");
    }

    /**
     * test_concurrent_remove_mount_point_entry:
     */
    void test_concurrent_remove_mount_point_entry ()
    {
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
    MountPointTable mount_point_table { std::make_shared<Logging> (), "test" };

    test.test_register_mount_point_type (&mount_point_table);
    test.test_extract_mount_point (&mount_point_table);

    return 0;
}