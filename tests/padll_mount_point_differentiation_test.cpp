/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#include <cstdio>
#include <cstring>
#include <dlfcn.h>
#include <fcntl.h>
#include <iostream>
#include <padll/library_headers/libc_headers.hpp>
#include <padll/stage/mount_point_table.hpp>
#include <thread>
#include <variant>

using namespace padll::stage;
using namespace padll::headers;
using namespace std::this_thread;

namespace padll::tests {

class MountPointDifferentiationTest {

private:
    FILE* m_fd { stdout };
    void* m_lib_link { ::dlopen ("libc.so.6", RTLD_LAZY) };
    uint32_t m_default_metadata_unit { static_cast<uint32_t> (-1) };

    void
    performance_report (const std::string& header, const int& operations, const long& elapsed_time)
    {
        std::fprintf (this->m_fd,
            "\n------------------------------------------------------------------\n");
        std::fprintf (this->m_fd, "%s\n", header.c_str ());
        std::fprintf (this->m_fd, "-----------------------------\n");

        std::fprintf (this->m_fd,
            "Ops:\t%d\t\tDuration:%ld ms\n",
            operations,
            (elapsed_time / 1000 / 1000));
        std::fprintf (this->m_fd,
            "------------------------------------------------------------------\n\n");
    }
    /**
     * create_mount_point_entry:
     * @param table_ptr
     * @param path
     * @param num_files
     * @param file_descriptors
     */
    void create_mount_point_entry (MountPointTable* table_ptr,
        const bool& create_fd,
        const std::string& path,
        const int& num_files,
        std::vector<std::variant<int, FILE*>>* file_identifiers,
        const bool& retry_after_create)
    {
        for (int i = 0; i < num_files; i++) {
            auto rand_file = static_cast<int> (random () % num_files);
            std::string path_to_file = path + std::to_string (rand_file);

            // extract mount point from path
            auto mount_point = table_ptr->extract_mount_point (path_to_file);
            bool result;

            if (create_fd) {
// open file and get file descriptor
#ifdef __linux__
                auto fd = ((libc_open_variadic_t)::dlsym (this->m_lib_link,
                    "open")) (path_to_file.c_str (), O_CREAT, 0666);
#else
                // FIXME: this could be further improved (dlsym over libc for non-unix settings)
                auto fd = ::open (path_to_file.c_str (), O_CREAT, 0666);
#endif

                // check if file was created
                if (fd == -1) {
                    std::fprintf (this->m_fd,
                        "Error (%s): %s - %s\n",
                        __func__,
                        strerror (errno),
                        path_to_file.c_str ());
                    return;
                }

                // create mount point entry for file descriptor
                result = table_ptr->create_mount_point_entry (fd,
                    path_to_file,
                    mount_point,
                    this->m_default_metadata_unit);

                // add file descriptor to vector
                file_identifiers->emplace_back (fd);

                // retry createing the same mount point entry
                if (retry_after_create) {
                    result = table_ptr->create_mount_point_entry (fd,
                        path_to_file,
                        mount_point,
                        this->m_default_metadata_unit);
                }
            } else {
// open file and get file pointer
#ifdef __linux__
                auto f_ptr = ((
                    libc_fopen_t)::dlsym (this->m_lib_link, "fopen")) (path_to_file.c_str (), "w");
#else
                // FIXME: this could be further improved (dlsym over libc for non-unix settings)
                auto f_ptr = ::fopen (path_to_file.c_str (), "w");
#endif

                // check if file was created
                if (f_ptr == nullptr) {
                    std::fprintf (this->m_fd, "Error (%s): %s\n", __func__, strerror (errno));
                    return;
                }

                // create mount point entry for file pointer
                result = table_ptr->create_mount_point_entry (f_ptr,
                    path_to_file,
                    mount_point,
                    this->m_default_metadata_unit);

                // add file pointer to vector
                file_identifiers->emplace_back (f_ptr);

                // retry createing the same mount point entry
                if (retry_after_create) {
                    result = table_ptr->create_mount_point_entry (f_ptr,
                        path_to_file,
                        mount_point,
                        this->m_default_metadata_unit);
                }
            }

            // check if entry was created
            if (!result) {
                std::fprintf (this->m_fd, "Error (%s): %s\n", __func__, strerror (errno));
            }
        }
    }

    /**
     * get_mount_point_entry:
     * @param table_ptr
     * @param use_file_descriptor
     * @param file_identifiers
     */
    void get_mount_point_entry (MountPointTable* table_ptr,
        const bool& use_file_descriptor,
        const std::vector<std::variant<int, FILE*>>& file_identifiers,
        const bool& print_debug_info)
    {
        std::stringstream stream;
        long successful_ops = 0;

        for (int i = 0; i < static_cast<int> (file_identifiers.size ()); i++) {
            auto index = static_cast<int> (random () % file_identifiers.size ());

            const MountPointEntry* entry;
            if (use_file_descriptor) {
                auto fd = std::get<int> (file_identifiers[index]);
                auto [return_value, entry_ptr] = table_ptr->get_mount_point_entry (fd);
                entry = std::move (entry_ptr);
            } else {
                auto f_ptr = std::get<FILE*> (file_identifiers[index]);
                auto [return_value, entry_ptr] = table_ptr->get_mount_point_entry (f_ptr);
                entry = std::move (entry_ptr);
            }

            if (entry != nullptr) {
                if (print_debug_info) {
                    std::fprintf (this->m_fd,
                        "%d (%d): %s\n",
                        i,
                        index,
                        entry->to_string ().c_str ());
                }
                successful_ops++;
            } else {
                std::fprintf (this->m_fd, "Error (%s): %s\n", __func__, strerror (errno));
            }
        }

        stream << get_id () << ": successful ops: " << successful_ops << std::endl;
        std::fprintf (this->m_fd, "%s", stream.str ().c_str ());
    }

    /**
     * delete_mount_point_entry:
     * @param table_ptr
     * @param create_fd
     * @param path
     * @param num_files
     * @param file_identifiers
     */
    void delete_mount_point_entry (MountPointTable* table_ptr,
        const bool& use_file_descriptor,
        const std::vector<std::variant<int, FILE*>>& file_identifiers,
        const bool& print_debug_info)
    {
        std::stringstream stream;
        long successful_ops = 0;

        for (int i = 0; i < static_cast<int> (file_identifiers.size ()); i++) {
            auto index = static_cast<int> (random () % file_identifiers.size ());

            bool return_value;
            if (use_file_descriptor) {
                auto fd = std::get<int> (file_identifiers[index]);
                return_value = table_ptr->remove_mount_point_entry (fd);

                // print error message if remote went wrong
                if (!return_value && print_debug_info) {
                    std::fprintf (this->m_fd, "Error (%s): %d\n", __func__, fd);
                }
            } else {
                auto f_ptr = std::get<FILE*> (file_identifiers[index]);
                return_value = table_ptr->remove_mount_point_entry (f_ptr);

                // print error message if remote went wrong
                if (!return_value && print_debug_info) {
                    std::fprintf (this->m_fd, "Error (%s): %p\n", __func__, (void*)f_ptr);
                }
            }

            if (return_value) {
                successful_ops++;
                if (print_debug_info) {
                    std::fprintf (this->m_fd, "Success (%s)\n", __func__);
                }
            }
        }
    }

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
    ~MountPointDifferentiationTest ()
    {
        if (this->m_lib_link != nullptr) {
            ::dlclose (this->m_lib_link);
        }
    }

    /*
     * test_extract_mount_point_from_path:
     */
    void test_extract_mount_point (MountPointTable* table_ptr)
    {
        std::fprintf (this->m_fd, "------------------\n");
        std::fprintf (this->m_fd, "%s:\n", __func__);

        std::vector<std::string_view> file_paths { "/home/user/file1",
            "/local/file2",
            "/remote/path/to/file3",
            "/tmp/path/to/file4" };

        for (auto const& elem : file_paths) {
            auto mount_point = table_ptr->extract_mount_point (elem);
            std::fprintf (this->m_fd,
                "\tfile_path: %s -- %s\n",
                elem.data (),
                padll::options::mount_point_to_string (mount_point).data ());
        }
    }

    /**
     * test_register_mount_point_type:
     */
    void test_register_mount_point_type (MountPointTable* table_ptr)
    {
        std::fprintf (this->m_fd, "------------------\n");
        std::fprintf (this->m_fd, "%s:\n", __func__);
        // register operations is conducted on initialize (which is executed in the constructor)

        std::stringstream stream;
        if (option_mount_point_differentiation_enabled) {
            // FIXME: Needing refactor or cleanup -@gsd at 4/13/2022, 2:22:01 PM
            // Do not consider right now differentiation between local and remote mountpoints.
            // stream << "\tLocal workflows: ";
            // for (auto& elem :
            //     table_ptr->get_default_workflows ().default_local_mount_point_workflows) {
            //     stream << elem << " ";
            // }
            // stream << std::endl;

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
    }

    /**
     * test_create_mount_point_entry:
     * @param table_ptr
     * @param num_threads
     * @param path
     * @param num_files
     * @param file_ptrs
     */
    void test_create_mount_point_entry (MountPointTable* table_ptr,
        const bool& create_fd,
        const int& num_threads,
        const std::string& path,
        const int& num_files,
        std::vector<std::variant<int, FILE*>>* file_ptrs,
        const bool& retry_after_create,
        const bool& print_debug_info)
    {
        // create lambda function for each thread to execute
        auto func = [this] (MountPointTable* f_table_ptr,
                        const bool& f_create_fd,
                        const std::string& f_path,
                        const int& f_num_files,
                        std::vector<std::variant<int, FILE*>>* f_file_ptrs,
                        const bool& f_retry_after_create) {
            // log message
            std::stringstream stream;
            stream << __func__ << "(" << get_id () << ")" << std::endl;
            std::fprintf (this->m_fd, "%s", stream.str ().c_str ());

            // execute create_mount_point_entry
            this->create_mount_point_entry (f_table_ptr,
                f_create_fd,
                f_path,
                f_num_files,
                f_file_ptrs,
                f_retry_after_create);
        };

        // create vector of threads and reserve space for num_threads
        std::vector<std::thread> threads {};
        threads.reserve (num_threads);

        auto start = std::chrono::high_resolution_clock::now ();
        // create threads
        for (int i = 0; i < num_threads; i++) {
            threads.emplace_back (std::thread (func,
                table_ptr,
                create_fd,
                path,
                num_files,
                file_ptrs,
                retry_after_create));
        }

        // join threads
        for (int i = 0; i < num_threads; i++) {
            std::stringstream stream;
            stream << __func__ << "(" << get_id () << "): joining thread-" << i << " ("
                   << threads[i].get_id () << ")" << std::endl;
            // join thread
            threads[i].join ();
            std::fprintf (this->m_fd, "%s\n", stream.str ().c_str ());
        }

        // calculate elapsed time
        auto end = std::chrono::high_resolution_clock::now ();
        std::chrono::duration<double> elapsed_time = end - start;

        // print table info
        if (print_debug_info) {
            create_fd
                ? std::fprintf (this->m_fd, "%s\n", table_ptr->fd_table_to_string ().c_str ())
                : std::fprintf (this->m_fd, "%s\n", table_ptr->fp_table_to_string ().c_str ());
        }

        // print performance report (number of operations and elapsed time)
        this->performance_report ("test_create_mount_point_entry",
            num_threads * num_files,
            std::chrono::duration_cast<std::chrono::nanoseconds> (elapsed_time).count ());
    }

    /**
     * test_get_mount_point_entry:
     */
    void test_get_mount_point_entry (MountPointTable* table_ptr,
        const bool& use_fd,
        const int& num_threads,
        const std::vector<std::variant<int, FILE*>>& file_ptrs,
        const bool& print_debug_info)
    {
        // create lambda function for each thread to execute
        auto func = [this] (MountPointTable* f_table_ptr,
                        const bool& f_use_fd,
                        const std::vector<std::variant<int, FILE*>>& f_file_ptrs,
                        const bool& f_print_debug_info) {
            // log message
            std::stringstream stream;
            stream << __func__ << "(" << get_id () << ")" << std::endl;
            std::fprintf (this->m_fd, "%s", stream.str ().c_str ());

            // execute get_mount_point_entry routine
            this->get_mount_point_entry (f_table_ptr, f_use_fd, f_file_ptrs, f_print_debug_info);
        };

        // create vector of threads and reserve space for num_threads
        std::vector<std::thread> threads;
        threads.reserve (num_threads);

        auto start = std::chrono::high_resolution_clock::now ();
        // create threads
        for (int i = 0; i < num_threads; i++) {
            threads.emplace_back (
                std::thread (func, table_ptr, use_fd, file_ptrs, print_debug_info));
        }

        // join threads
        for (int i = 0; i < num_threads; i++) {
            std::stringstream stream;
            stream << __func__ << "(" << get_id () << "): joining thread-" << i << " ("
                   << threads[i].get_id () << ")" << std::endl;
            // join thread
            threads[i].join ();
            std::fprintf (this->m_fd, "%s\n", stream.str ().c_str ());
        }

        // calculate elapsed time
        auto end = std::chrono::high_resolution_clock::now ();
        std::chrono::duration<double> elapsed_time = end - start;

        // print performance report (number of operations and elapsed time)
        this->performance_report ("test_get_mount_point_entry",
            static_cast<int> (num_threads * file_ptrs.size ()),
            std::chrono::duration_cast<std::chrono::nanoseconds> (elapsed_time).count ());
    }

    /**
     * test_remove_mount_point_entry:
     */
    void test_remove_mount_point_entry (MountPointTable* table_ptr,
        const bool& create_fd,
        const int& num_threads,
        const std::vector<std::variant<int, FILE*>>& file_ptrs,
        const bool& print_debug_info)
    {
        // create lambda function for each thread to execute
        auto func = [this] (MountPointTable* f_table_ptr,
                        const bool& f_create_fd,
                        const std::vector<std::variant<int, FILE*>>& f_file_ptrs,
                        const bool& f_print_debug_info) {
            // log message
            std::stringstream stream;
            stream << __func__ << "(" << get_id () << ")" << std::endl;
            std::fprintf (this->m_fd, "%s", stream.str ().c_str ());

            // execute delete_mount_point_entry routine
            this->delete_mount_point_entry (f_table_ptr,
                f_create_fd,
                f_file_ptrs,
                f_print_debug_info);
        };

        // create vector of threads and reserve space for num_threads
        std::vector<std::thread> threads;
        threads.reserve (num_threads);

        auto start = std::chrono::high_resolution_clock::now ();
        // create threads
        for (int i = 0; i < num_threads; i++) {
            threads.emplace_back (
                std::thread (func, table_ptr, create_fd, file_ptrs, print_debug_info));
        }

        // join threads
        for (int i = 0; i < num_threads; i++) {
            std::stringstream stream;
            stream << __func__ << "(" << get_id () << "): joining thread-" << i << " ("
                   << threads[i].get_id () << ")" << std::endl;
            // join thread
            threads[i].join ();
            std::fprintf (this->m_fd, "%s\n", stream.str ().c_str ());
        }

        // calculate elapsed time
        auto end = std::chrono::high_resolution_clock::now ();
        std::chrono::duration<double> elapsed_time = end - start;

        // print performance report (number of operations and elapsed time)
        this->performance_report ("test_remove_mount_point_entry",
            static_cast<int> ((num_threads * file_ptrs.size ())),
            std::chrono::duration_cast<std::chrono::nanoseconds> (elapsed_time).count ());
    }

    void test_pick_workflow_id ()
    {
        std::fprintf (this->m_fd, "test_pick_workflow_id: incomplete ...\n");
    }
};
} // namespace padll::tests

namespace tests = padll::tests;

/**
 * print_file_identifiers_list:
 * @param file_identifiers_list
 */
void print_file_identifiers_list (
    const std::vector<std::variant<int, FILE*>>& file_identifiers_list)
{
    std::stringstream stream;
    stream << "File identifiers: " << file_identifiers_list.size () << std::endl;
    for (auto& elem : file_identifiers_list) {
        if (std::holds_alternative<int> (elem)) {
            stream << std::get<int> (elem) << " ";
        } else {
            stream << std::get<FILE*> (elem) << " ";
        }
    }
    stream << std::endl;
    std::fprintf (stdout, "%s", stream.str ().c_str ());
}

// NOTE: To execute these tests, ensure that all syscalls (e.g., ::open) will be submitted to the
// PosixPassthrough backend; otherwise, they will be first submitted to PAIO data plane stage.
int main (int argc, char** argv)
{
    // check argv for the file to be placed the result
    FILE* fd = stdout;

    // open file to write the logging results
    if (argc > 1) {
        // FIXME: use pointer to libc function, rather than RTLD_NEXT
        fd = ((padll::headers::libc_fopen_t)dlsym (RTLD_NEXT, "fopen")) (argv[1], "w");

        if (fd == nullptr) {
            fd = stdout;
        }
    }

    tests::MountPointDifferentiationTest test { fd };
    MountPointTable mount_point_table {};
    int num_threads = 1;
    int num_files = 100;
    bool use_fd = true;
    bool print_debug_info = true;
    bool retry_after_create = true;

    // test.test_register_mount_point_type (&mount_point_table);
    // test.test_extract_mount_point (&mount_point_table);

    std::vector<std::variant<int, FILE*>> file_identifiers_list {};
    file_identifiers_list.reserve (num_threads * num_files);

    if (use_fd) {
        test.test_create_mount_point_entry (&mount_point_table,
            use_fd,
            num_threads,
            "/tmp/file-fd-",
            num_files,
            &file_identifiers_list,
            print_debug_info,
            retry_after_create);

        test.test_get_mount_point_entry (&mount_point_table,
            use_fd,
            num_threads,
            file_identifiers_list,
            print_debug_info);

        test.test_remove_mount_point_entry (&mount_point_table,
            use_fd,
            num_threads,
            file_identifiers_list,
            print_debug_info);
    } else {
        test.test_create_mount_point_entry (&mount_point_table,
            use_fd,
            num_threads,
            "/tmp/file-ptr-",
            num_files,
            &file_identifiers_list,
            print_debug_info,
            retry_after_create);
        test.test_get_mount_point_entry (&mount_point_table,
            use_fd,
            num_threads,
            file_identifiers_list,
            print_debug_info);
        test.test_remove_mount_point_entry (&mount_point_table,
            use_fd,
            num_threads,
            file_identifiers_list,
            print_debug_info);
    }

    if (print_debug_info) {
        print_file_identifiers_list (file_identifiers_list);
    }

    return 0;
}
