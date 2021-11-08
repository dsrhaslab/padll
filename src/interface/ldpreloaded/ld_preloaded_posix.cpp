/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <padll/interface/ldpreloaded/ld_preloaded_posix.hpp>

namespace padll {

// LdPreloadedPosix default constructor.
LdPreloadedPosix::LdPreloadedPosix () :
    m_logger_ptr { std::make_shared<Logging> () },
    m_stage { paio::make_unique<DataPlaneStage> (this->m_logger_ptr) }
{
    std::printf ("LdPreloadedPosix default constructor.\n");
}

// TODO: check move operation within LdPreloadPosix, but not on PosixFileSystem
// LdPreloadedPosix explicit constructor.
LdPreloadedPosix::LdPreloadedPosix (std::shared_ptr<Logging> logging_ptr) :
    m_logger_ptr { logging_ptr },
    m_stage { paio::make_unique<DataPlaneStage> (logging_ptr) }
{
    std::printf ("LdPreloadedPosix explicit constructor.\n");
}

// LdPreloadedPosix parameterized constructor.
LdPreloadedPosix::LdPreloadedPosix (const std::string& lib,
    const bool& stat_collection,
    std::shared_ptr<Logging> logging_ptr) :
    m_collect { stat_collection },
    m_logger_ptr { logging_ptr },
    m_stage { paio::make_unique<DataPlaneStage> (logging_ptr) }
{
    std::printf ("LdPreloadedPosix parameterized constructor.\n");
}

// LdPreloadedPosix default destructor.
LdPreloadedPosix::~LdPreloadedPosix ()
{
    std::printf ("LdPreloadedPosix default destructor.\n");
    this->m_logger_ptr->log_info ("LdPreloadedPosix default destructor.");

    if (option_default_table_format) {
        // print to stdout metadata-based statistics in tabular format
        this->m_metadata_stats.tabulate ();
        // print to stdout data-based statistics in tabular format
        this->m_data_stats.tabulate ();
        // print to stdout directory-based statistics in tabular format
        this->m_dir_stats.tabulate ();
        // print to stdout extended attributes based statistics in tabular format
        this->m_ext_attr_stats.tabulate ();
        // print to stdout file modes based statistics in tabular format
        this->m_file_mode_stats.tabulate ();
    } else {
        this->m_logger_ptr->log_debug (this->to_string ());
    }
}

// set_statistic_collection call. (...)
void LdPreloadedPosix::set_statistic_collection (bool value)
{
    this->m_collect.store (value);
}

// get_statistic_entry call.
StatisticEntry LdPreloadedPosix::get_statistic_entry (const OperationType& operation_type,
    const int& operation_entry)
{
    switch (operation_type) {
        case OperationType::metadata_calls:
            return this->m_metadata_stats.get_statistic_entry (operation_entry);

        case OperationType::data_calls:
            return this->m_data_stats.get_statistic_entry (operation_entry);

        case OperationType::directory_calls:
            return this->m_dir_stats.get_statistic_entry (operation_entry);

        case OperationType::ext_attr_calls:
            return this->m_ext_attr_stats.get_statistic_entry (operation_entry);

        case OperationType::file_mode_calls:
            return this->m_file_mode_stats.get_statistic_entry (operation_entry);

        default:
            return StatisticEntry {};
    }
}

// to_string call. (...)
std::string LdPreloadedPosix::to_string ()
{
    auto pid = ::getpid ();
    auto ppid = ::getppid ();
    std::stringstream stream;

    stream << "\nLdPreloadedPosix::Metadata statistics (" << pid << ", " << ppid << ")\n";
    stream << "-----------------------------------------------------------\n";
    stream << this->m_metadata_stats.to_string () << "\n";

    stream << "LdPreloadedPosix::Data statistics (" << pid << ", " << ppid << ")\n";
    stream << "-----------------------------------------------------------\n";
    stream << this->m_data_stats.to_string () << "\n";

    stream << "LdPreloadedPosix::Directory statistics (" << pid << ", " << ppid << ")\n";
    stream << "-----------------------------------------------------------\n";
    stream << this->m_dir_stats.to_string () << "\n";

    stream << "LdPreloadedPosix::Extended attributes statistics (" << pid << ", " << ppid << ")\n";
    stream << "-------------------------------------------------------------------\n";
    stream << this->m_ext_attr_stats.to_string () << "\n";

    stream << "LdPreloadedPosix::File mode statistics (" << pid << ", " << ppid << ")\n";
    stream << "-----------------------------------------------------------\n";
    stream << this->m_file_mode_stats.to_string () << "\n";

    return stream.str ();
}

// ld_preloaded_posix_read call.
ssize_t LdPreloadedPosix::ld_preloaded_posix_read (int fd, void* buf, size_t counter)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-read (" + std::to_string (fd) + ")");
    }

    // hook POSIX read operation to m_data_operations.m_read
    this->m_dlsym_hook.hook_posix_read (m_data_operations.m_read);

    // enforce read request to PAIO data plane stage
    this->m_stage->enforce_request (static_cast<int>(paio::POSIX::read),
        static_cast<int>(paio::POSIX_META::data_op),
        counter);

    // perform original POSIX read operation
    ssize_t result = m_data_operations.m_read (fd, buf, counter);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::read), 1, result);
        } else {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::read), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_write call.
ssize_t LdPreloadedPosix::ld_preloaded_posix_write (int fd, const void* buf, size_t counter)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-write (" + std::to_string (fd) + ")");
    }

    // hook POSIX write operation to m_data_operations.m_write
    this->m_dlsym_hook.hook_posix_write (m_data_operations.m_write);

    // enforce write request to PAIO data plane stage
    this->m_stage->enforce_request (static_cast<int>(paio::POSIX::write),
        static_cast<int>(paio::POSIX_META::data_op),
        counter);

    // perform original POSIX write operation
    ssize_t result = m_data_operations.m_write (fd, buf, counter);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::write), 1, result);
            this->m_logger_ptr->log_debug ("ld_preloaded_posix-counter-write ("
                + std::to_string (
                    this->m_data_stats.get_statistic_entry (static_cast<int> (Data::write))
                        .get_operation_counter ())
                + ")");
        } else {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::write), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_pread call.
ssize_t LdPreloadedPosix::ld_preloaded_posix_pread (int fd, void* buf, size_t counter, off_t offset)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-pread (" + std::to_string (fd) + ")");
    }

    // hook POSIX pread operation to m_data_operations.m_pread
    this->m_dlsym_hook.hook_posix_pread (m_data_operations.m_pread);

    // enforce pread request to PAIO data plane stage
    this->m_stage->enforce_request (static_cast<int>(paio::POSIX::pread),
                                    static_cast<int>(paio::POSIX_META::data_op),
                                    counter);

    // perform original POSIX pread operation
    ssize_t result = m_data_operations.m_pread (fd, buf, counter, offset);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::pread), 1, result);
        } else {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::pread), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_pwrite call.
ssize_t
LdPreloadedPosix::ld_preloaded_posix_pwrite (int fd, const void* buf, size_t counter, off_t offset)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-pwrite (" + std::to_string (fd) + ")");
    }

    // hook POSIX pwrite operation to m_data_operations.m_pwrite
    this->m_dlsym_hook.hook_posix_pwrite (m_data_operations.m_pwrite);

    // enforce pwrite request to PAIO data plane stage
    this->m_stage->enforce_request (static_cast<int>(paio::POSIX::pwrite),
                                    static_cast<int>(paio::POSIX_META::data_op),
                                    counter);

    // perform original POSIX pwrite operation
    ssize_t result = m_data_operations.m_pwrite (fd, buf, counter, offset);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::pwrite), 1, result);
        } else {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::pwrite), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_pread64 call.
#if defined(__USE_LARGEFILE64)
ssize_t
LdPreloadedPosix::ld_preloaded_posix_pread64 (int fd, void* buf, size_t counter, off64_t offset)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-pread64 (" + std::to_string (fd) + ")");
    }

    // hook POSIX pread64 operation to m_data_operations.m_pread64
    this->m_dlsym_hook.hook_posix_pread64 (m_data_operations.m_pread64);

    // enforce pread64 request to PAIO data plane stage
    this->m_stage->enforce_request (static_cast<int>(paio::POSIX::pread64),
        static_cast<int>(paio::POSIX_META::data_op),
        counter);

    // perform original POSIX pread64 operation
    ssize_t result = m_data_operations.m_pread64 (fd, buf, counter, offset);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::pread64), 1, result);
        } else {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::pread64), 1, 0, 1);
        }
    }

    return result;
}
#endif

// ld_preloaded_posix_pwrite64 call.
#if defined(__USE_LARGEFILE64)
ssize_t LdPreloadedPosix::ld_preloaded_posix_pwrite64 (int fd,
    const void* buf,
    size_t counter,
    off64_t offset)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-pwrite64 (" + std::to_string (fd) + ")");
    }

    // hook POSIX pwrite64 operation to m_data_operations.m_pwrite64
    this->m_dlsym_hook.hook_posix_pwrite64 (m_data_operations.m_pwrite64);

    // enforce pwrite64 request to PAIO data plane stage
    this->m_stage->enforce_request (static_cast<int>(paio::POSIX::pwrite64),
        static_cast<int>(paio::POSIX_META::data_op),
        counter);


    // perform original POSIX pwrite64 operation
    ssize_t result = m_data_operations.m_pwrite64 (fd, buf, counter, offset);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::pwrite64),
                1,
                result);
        } else {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::pwrite64), 1, 0, 1);
        }
    }

    return result;
}
#endif

// ld_preloaded_posix_fread call. (...)
size_t
LdPreloadedPosix::ld_preloaded_posix_fread (void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fpread");
    }

    // hook POSIX fread operation to m_data_operations.m_fread
    this->m_dlsym_hook.hook_posix_fread (m_data_operations.m_fread);

    // enforce fread request to PAIO data plane stage
    this->m_stage->enforce_request (static_cast<int>(paio::POSIX::fread),
                                    static_cast<int>(paio::POSIX_META::data_op),
                                    (size > 0 && nmemb > 0) ? size * nmemb : 1);

    // perform original POSIX fread operation
    size_t result = m_data_operations.m_fread (ptr, size, nmemb, stream);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::fread), 1, result);
        } else {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::fread), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_fwrite call. (...)
size_t LdPreloadedPosix::ld_preloaded_posix_fwrite (const void* ptr,
    size_t size,
    size_t nmemb,
    FILE* stream)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fwrite");
    }

    // hook POSIX fwrite operation to m_data_operations.m_fwrite
    this->m_dlsym_hook.hook_posix_fwrite (m_data_operations.m_fwrite);

    this->m_stage->enforce_request (static_cast<int>(paio::POSIX::fwrite),
                                    static_cast<int>(paio::POSIX_META::data_op),
                                    (size > 0 && nmemb > 0) ? size * nmemb : 1);

    // perform original POSIX fwrite operation
    size_t result = m_data_operations.m_fwrite (ptr, size, nmemb, stream);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::fwrite), 1, result);
        } else {
            this->m_data_stats.update_statistic_entry (static_cast<int> (Data::fwrite), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_open call.
int LdPreloadedPosix::ld_preloaded_posix_open (const char* path, int flags, mode_t mode)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug (
            "ld_preloaded_posix-open-variadic (" + std::string (path) + ")");
    }

    // hook POSIX open operation to m_metadata_operations.m_open_var
    this->m_dlsym_hook.hook_posix_open_var (m_metadata_operations.m_open_var);

    // enforce open request to PAIO data plane stage
    this->m_stage->enforce_request (static_cast<int>(paio::POSIX::open),
                                    static_cast<int>(paio::POSIX_META::meta_op),
                                    1);

    // perform original POSIX open operation
    int result = m_metadata_operations.m_open_var (path, flags, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (
                static_cast<int> (Metadata::open_variadic),
                1,
                0);
        } else {
            this->m_metadata_stats
                .update_statistic_entry (static_cast<int> (Metadata::open_variadic), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_open call.
int LdPreloadedPosix::ld_preloaded_posix_open (const char* path, int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-open (" + std::string (path) + ")");
    }

    // hook POSIX open operation to m_metadata_operations.m_open
    this->m_dlsym_hook.hook_posix_open (m_metadata_operations.m_open);

    // enforce open request to PAIO data plane stage
    this->m_stage->enforce_request (static_cast<int>(paio::POSIX::open),
                                    static_cast<int>(paio::POSIX_META::meta_op),
                                    1);

    // perform original POSIX open operation
    int result = m_metadata_operations.m_open (path, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::open), 1, 0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::open),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_creat call.
int LdPreloadedPosix::ld_preloaded_posix_creat (const char* path, mode_t mode)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-creat (" + std::string (path) + ")");
    }

    // hook POSIX creat operation to m_metadata_operations.m_creat
    this->m_dlsym_hook.hook_posix_creat (m_metadata_operations.m_creat);

    // enforce creat request to PAIO data plane stage
    this->m_stage->enforce_request (static_cast<int>(paio::POSIX::creat),
                                    static_cast<int>(paio::POSIX_META::meta_op),
                                    1);

    // perform original POSIX creat operation
    int result = m_metadata_operations.m_creat (path, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::creat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::creat),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_creat64 call.
int LdPreloadedPosix::ld_preloaded_posix_creat64 (const char* path, mode_t mode)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-creat64 (" + std::string (path) + ")");
    }

    // hook POSIX creat64 operation to m_metadata_operations.m_creat64
    this->m_dlsym_hook.hook_posix_creat64 (m_metadata_operations.m_creat64);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX creat64 operation
    int result = m_metadata_operations.m_creat64 (path, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::creat64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::creat64),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_openat call.
int LdPreloadedPosix::ld_preloaded_posix_openat (int dirfd,
    const char* path,
    int flags,
    mode_t mode)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-openat-variadic ("
            + std::to_string (dirfd) + ", " + std::string (path) + ")");
    }

    // hook POSIX openat variadic operation to m_metadata_operations.m_openat_var
    this->m_dlsym_hook.hook_posix_openat_var (m_metadata_operations.m_openat_var);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX openat operation
    int result = m_metadata_operations.m_openat_var (dirfd, path, flags, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (
                static_cast<int> (Metadata::openat_variadic),
                1,
                0);
        } else {
            this->m_metadata_stats
                .update_statistic_entry (static_cast<int> (Metadata::openat_variadic), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_openat call.
int LdPreloadedPosix::ld_preloaded_posix_openat (int dirfd, const char* path, int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-openat (" + std::to_string (dirfd) + ", "
            + std::string (path) + ")");
    }

    // hook POSIX openat operation to m_metadata_operations.m_openat
    this->m_dlsym_hook.hook_posix_openat (m_metadata_operations.m_openat);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX openat operation
    int result = m_metadata_operations.m_openat (dirfd, path, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::openat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::openat),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_open64 call. (...)
int LdPreloadedPosix::ld_preloaded_posix_open64 (const char* path, int flags, mode_t mode)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug (
            "ld_preloaded_posix-open64-variadic (" + std::string (path) + ")");
    }

    // hook POSIX open64_var operation to m_metadata_operations.m_open64_var
    this->m_dlsym_hook.hook_posix_open64_variadic (m_metadata_operations.m_open64_var);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX open64 operation
    int result = m_metadata_operations.m_open64_var (path, flags, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (
                static_cast<int> (Metadata::open64_variadic),
                1,
                0);
        } else {
            this->m_metadata_stats
                .update_statistic_entry (static_cast<int> (Metadata::open64_variadic), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_open64 call. (...)
int LdPreloadedPosix::ld_preloaded_posix_open64 (const char* path, int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-open64 (" + std::string (path) + ")");
    }

    // hook POSIX open64 operation to m_metadata_operations.m_open64
    this->m_dlsym_hook.hook_posix_open64 (m_metadata_operations.m_open64);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX open64 operation
    int result = m_metadata_operations.m_open64 (path, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::open64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::open64),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_close call.
int LdPreloadedPosix::ld_preloaded_posix_close (int fd)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-close (" + std::to_string (fd) + ")");
    }

    // hook POSIX close operation to m_metadata_operations.m_close
    this->m_dlsym_hook.hook_posix_close (m_metadata_operations.m_close);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX close operation
    int result = m_metadata_operations.m_close (fd);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::close),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::close),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fsync call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fsync (int fd)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fsync (" + std::to_string (fd) + ")");
    }

    // hook POSIX fsync operation to m_metadata_operations.m_fsync
    this->m_dlsym_hook.hook_posix_fsync (m_metadata_operations.m_fsync);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fsync operation
    int result = m_metadata_operations.m_fsync (fd);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fsync),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fsync),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fdatasync call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fdatasync (int fd)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug (
            "ld_preloaded_posix-fdatasync (" + std::to_string (fd) + ")");
    }

    // hook POSIX fdatasync operation to m_metadata_operations.m_fdatasync
    this->m_dlsym_hook.hook_posix_fdatasync (m_metadata_operations.m_fdatasync);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fdatasync operation
    int result = m_metadata_operations.m_fdatasync (fd);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fdatasync),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fdatasync),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_sync call. (...)
void LdPreloadedPosix::ld_preloaded_posix_sync ()
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-sync");
    }

    // hook POSIX sync operation to m_metadata_operations.m_sync
    this->m_dlsym_hook.hook_posix_sync (m_metadata_operations.m_sync);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX sync operation
    m_metadata_operations.m_sync ();

    // update statistic entry
    if (this->m_collect) {
        this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::sync), 1, 0);
    }
}

// ld_preloaded_posix_syncfs call. (...)
int LdPreloadedPosix::ld_preloaded_posix_syncfs (int fd)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-syncfs (" + std::to_string (fd) + ")");
    }

    // hook POSIX syncfs operation to m_metadata_operations.m_syncfs
    this->m_dlsym_hook.hook_posix_syncfs (m_metadata_operations.m_syncfs);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX syncfs operation
    int result = m_metadata_operations.m_syncfs (fd);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::syncfs),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::syncfs),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_truncate call. (...)
int LdPreloadedPosix::ld_preloaded_posix_truncate (const char* path, off_t length)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-truncate (" + std::string (path) + ")");
    }

    // hook POSIX truncate operation to m_metadata_operations.m_truncate
    this->m_dlsym_hook.hook_posix_truncate (m_metadata_operations.m_truncate);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX truncate operation
    int result = m_metadata_operations.m_truncate (path, length);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::truncate),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::truncate),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_ftruncate call. (...)
int LdPreloadedPosix::ld_preloaded_posix_ftruncate (int fd, off_t length)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug (
            "ld_preloaded_posix-ftruncate (" + std::to_string (fd) + ")");
    }

    // hook POSIX ftruncate operation to m_metadata_operations.m_ftruncate
    this->m_dlsym_hook.hook_posix_ftruncate (m_metadata_operations.m_ftruncate);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX ftruncate operation
    int result = m_metadata_operations.m_ftruncate (fd, length);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::ftruncate),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::ftruncate),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_truncate64 call. (...)
int LdPreloadedPosix::ld_preloaded_posix_truncate64 (const char* path, off_t length)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug (
            "ld_preloaded_posix-truncate64 (" + std::string (path) + ")");
    }

    // hook POSIX truncate64 operation to m_metadata_operations.m_truncate64
    this->m_dlsym_hook.hook_posix_truncate64 (m_metadata_operations.m_truncate64);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX truncate64 operation
    int result = m_metadata_operations.m_truncate64 (path, length);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::truncate64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::truncate64),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_ftruncate64 call. (...)
int LdPreloadedPosix::ld_preloaded_posix_ftruncate64 (int fd, off_t length)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug (
            "ld_preloaded_posix-ftruncate64 (" + std::to_string (fd) + ")");
    }

    // hook POSIX ftruncate64 operation to m_metadata_operations.m_ftruncate64
    this->m_dlsym_hook.hook_posix_ftruncate64 (m_metadata_operations.m_ftruncate64);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX ftruncate64 operation
    int result = m_metadata_operations.m_ftruncate64 (fd, length);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::ftruncate64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::ftruncate64),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_xstat call. (...)
int LdPreloadedPosix::ld_preloaded_posix_xstat (int version, const char* path, struct stat* statbuf)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-xstat (" + std::string (path) + ")");
    }

    // hook POSIX __xstat operation to m_metadata_operations.m_xstat
    this->m_dlsym_hook.hook_posix_xstat (m_metadata_operations.m_xstat);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX __xstat (stat) operation
    int result = m_metadata_operations.m_xstat (version, path, statbuf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::stat), 1, 0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::stat),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_lxstat call. (...)
int LdPreloadedPosix::ld_preloaded_posix_lxstat (int version,
    const char* path,
    struct stat* statbuf)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-lxstat (" + std::string (path) + ")");
    }

    // hook POSIX __lxstat operation to m_metadata_operations.m_lxstat
    this->m_dlsym_hook.hook_posix_lxstat (m_metadata_operations.m_lxstat);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX __lxstat (lstat) operation
    int result = m_metadata_operations.m_lxstat (version, path, statbuf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::lstat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::lstat),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fxstat call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fxstat (int version, int fd, struct stat* statbuf)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fxstat (" + std::to_string (fd) + ")");
    }

    // hook POSIX __fxstat operation to m_metadata_operations.m_fxstat
    this->m_dlsym_hook.hook_posix_fxstat (m_metadata_operations.m_fxstat);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX __fxstat (fstat) operation
    int result = m_metadata_operations.m_fxstat (version, fd, statbuf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstat),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fxstatat call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fxstatat (int version,
    int dirfd,
    const char* path,
    struct stat* statbuf,
    int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fxstatat (" + std::to_string (dirfd)
            + ", " + std::string (path) + ")");
    }

    // hook POSIX __fxstatat operation to m_metadata_operations.m_fxstatat
    this->m_dlsym_hook.hook_posix_fxstatat (m_metadata_operations.m_fxstatat);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX __fxstatat (fstatat) operation
    int result = m_metadata_operations.m_fxstatat (version, dirfd, path, statbuf, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstatat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstatat),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_xstat64 call. (...)
int LdPreloadedPosix::ld_preloaded_posix_xstat64 (int version,
    const char* path,
    struct stat64* statbuf)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-xstat64 (" + std::string (path) + ")");
    }

    // hook POSIX __xstat64 operation to m_metadata_operations.m_xstat64
    this->m_dlsym_hook.hook_posix_xstat64 (m_metadata_operations.m_xstat64);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX __xstat64 (stat) operation
    int result = m_metadata_operations.m_xstat64 (version, path, statbuf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::stat64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::stat64),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_lxstat64 call. (...)
int LdPreloadedPosix::ld_preloaded_posix_lxstat64 (int version,
    const char* path,
    struct stat64* statbuf)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-lxstat64 (" + std::string (path) + ")");
    }

    // hook POSIX __lxstat64 operation to m_metadata_operations.m_lxstat64
    this->m_dlsym_hook.hook_posix_lxstat64 (m_metadata_operations.m_lxstat64);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX __lxstat64 (lstat64) operation
    int result = m_metadata_operations.m_lxstat64 (version, path, statbuf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::lstat64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::lstat64),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fxstat64 call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fxstat64 (int version, int fd, struct stat64* statbuf)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fxstat64 (" + std::to_string (fd) + ")");
    }

    // hook POSIX __fxstat64 operation to m_metadata_operations.m_fxstat64
    this->m_dlsym_hook.hook_posix_fxstat64 (m_metadata_operations.m_fxstat64);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX __fxstat64 (fxstat64) operation
    int result = m_metadata_operations.m_fxstat64 (version, fd, statbuf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstat64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstat64),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fxstatat64 call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fxstatat64 (int version,
    int dirfd,
    const char* path,
    struct stat64* statbuf,
    int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fxstatat64 (" + std::to_string (dirfd)
            + ", " + std::string (path) + ")");
    }

    // hook POSIX __fxstatat64 operation to m_metadata_operations.m_fxstatat64
    this->m_dlsym_hook.hook_posix_fxstatat64 (m_metadata_operations.m_fxstatat64);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX __fxstatat64 (fxstatat64) operation
    int result = m_metadata_operations.m_fxstatat64 (version, dirfd, path, statbuf, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstatat64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstatat64),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_statfs call. (...)
int LdPreloadedPosix::ld_preloaded_posix_statfs (const char* path, struct statfs* buf)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-statfs (" + std::string (path) + ")");
    }

    // hook POSIX statfs operation to m_metadata_operations.m_statfs
    this->m_dlsym_hook.hook_posix_statfs (m_metadata_operations.m_statfs);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX statfs operation
    int result = m_metadata_operations.m_statfs (path, buf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::statfs),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::statfs),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fstatfs call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fstatfs (int fd, struct statfs* buf)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fstatfs (" + std::to_string (fd) + ")");
    }

    // hook POSIX fstatfs operation to m_metadata_operations.m_fstatfs
    this->m_dlsym_hook.hook_posix_fstatfs (m_metadata_operations.m_fstatfs);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fstatfs operation
    int result = m_metadata_operations.m_fstatfs (fd, buf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstatfs),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstatfs),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_statfs64 call. (...)
int LdPreloadedPosix::ld_preloaded_posix_statfs64 (const char* path, struct statfs64* buf)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-statfs64 (" + std::string (path) + ")");
    }

    // hook POSIX statfs64 operation to m_metadata_operations.m_statfs64
    this->m_dlsym_hook.hook_posix_statfs64 (m_metadata_operations.m_statfs64);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX statfs64 operation
    int result = m_metadata_operations.m_statfs64 (path, buf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::statfs64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::statfs64),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fstatfs64 call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fstatfs64 (int fd, struct statfs64* buf)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug (
            "ld_preloaded_posix-fstatfs64 (" + std::to_string (fd) + ")");
    }

    // hook POSIX fstatfs64 operation to m_metadata_operations.m_fstatfs64
    this->m_dlsym_hook.hook_posix_fstatfs64 (m_metadata_operations.m_fstatfs64);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fstatfs64 operation
    int result = m_metadata_operations.m_fstatfs64 (fd, buf);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstatfs64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fstatfs64),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_link call. (...)
int LdPreloadedPosix::ld_preloaded_posix_link (const char* old_path, const char* new_path)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-link (" + std::string (old_path) + ", "
            + std::string (new_path) + ")");
    }

    // hook POSIX link operation to m_metadata_operations.m_link
    this->m_dlsym_hook.hook_posix_link (m_metadata_operations.m_link);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX link operation
    int result = m_metadata_operations.m_link (old_path, new_path);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::link), 1, 0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::link),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_unlink call. (...)
int LdPreloadedPosix::ld_preloaded_posix_unlink (const char* path)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-unlink (" + std::string (path) + ")");
    }

    // hook POSIX unlink operation to m_metadata_operations.m_unlink
    this->m_dlsym_hook.hook_posix_unlink (m_metadata_operations.m_unlink);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX unlink operation
    int result = m_metadata_operations.m_unlink (path);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::unlink),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::unlink),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_linkat call. (...)
int LdPreloadedPosix::ld_preloaded_posix_linkat (int olddirfd,
    const char* old_path,
    int newdirfd,
    const char* new_path,
    int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-linkat (" + std::to_string (olddirfd)
            + ", " + std::string (old_path) + ", " + std::to_string (newdirfd) + ", "
            + std::string (new_path) + ")");
    }

    // hook POSIX linkat operation to m_metadata_operations.m_linkat
    this->m_dlsym_hook.hook_posix_linkat (m_metadata_operations.m_linkat);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX linkat operation
    int result = m_metadata_operations.m_linkat (olddirfd, old_path, newdirfd, new_path, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::linkat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::linkat),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_unlinkat call. (...)
int LdPreloadedPosix::ld_preloaded_posix_unlinkat (int dirfd, const char* pathname, int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-unlinkat (" + std::to_string (dirfd)
            + ", " + std::string (pathname) + ", " + std::to_string (flags) + ")");
    }

    // hook POSIX unlinkat operation to m_metadata_operations.m_unlinkat
    this->m_dlsym_hook.hook_posix_unlinkat (m_metadata_operations.m_unlinkat);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX unlinkat operation
    int result = m_metadata_operations.m_unlinkat (dirfd, pathname, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::unlinkat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::unlinkat),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_rename call. (...)
int LdPreloadedPosix::ld_preloaded_posix_rename (const char* old_path, const char* new_path)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-rename (" + std::string (old_path) + ", "
            + std::string (new_path) + ")");
    }

    // hook POSIX rename operation to m_metadata_operations.m_rename
    this->m_dlsym_hook.hook_posix_rename (m_metadata_operations.m_rename);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX rename operation
    int result = m_metadata_operations.m_rename (old_path, new_path);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::rename),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::rename),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_renameat call. (...)
int LdPreloadedPosix::ld_preloaded_posix_renameat (int olddirfd,
    const char* old_path,
    int newdirfd,
    const char* new_path)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-renameat (" + std::to_string (olddirfd)
            + ", " + std::string (old_path) + ", " + std::to_string (newdirfd) + ", "
            + std::string (new_path) + ")");
    }

    // hook POSIX renameat operation to m_metadata_operations.m_renameat
    this->m_dlsym_hook.hook_posix_renameat (m_metadata_operations.m_renameat);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX renameat operation
    int result = m_metadata_operations.m_renameat (olddirfd, old_path, newdirfd, new_path);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::renameat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::renameat),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_symlink call. (...)
int LdPreloadedPosix::ld_preloaded_posix_symlink (const char* target, const char* linkpath)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-symlink (" + std::string (target) + ", "
            + std::string (linkpath) + ")");
    }

    // hook POSIX symlink operation to m_metadata_operations.m_symlink
    this->m_dlsym_hook.hook_posix_symlink (m_metadata_operations.m_symlink);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX symlink operation
    int result = m_metadata_operations.m_symlink (target, linkpath);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::symlink),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::symlink),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_symlinkat call. (...)
int LdPreloadedPosix::ld_preloaded_posix_symlinkat (const char* target,
    int newdirfd,
    const char* linkpath)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-symlinkat (" + std::string (target)
            + ", " + std::to_string (newdirfd) + ", " + std::string (linkpath) + ")");
    }

    // hook POSIX symlinkat operation to m_metadata_operations.m_symlinkat
    this->m_dlsym_hook.hook_posix_symlinkat (m_metadata_operations.m_symlinkat);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX symlinkat operation
    int result = m_metadata_operations.m_symlinkat (target, newdirfd, linkpath);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::symlinkat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::symlinkat),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_readlink call. (...)
ssize_t LdPreloadedPosix::ld_preloaded_posix_readlink (const char* path, char* buf, size_t bufsize)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-readlink (" + std::string (path) + ")");
    }

    // hook POSIX readlink operation to m_metadata_operations.m_readlink
    this->m_dlsym_hook.hook_posix_readlink (m_metadata_operations.m_readlink);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX readlink operation
    ssize_t result = m_metadata_operations.m_readlink (path, buf, bufsize);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::readlink),
                1,
                result);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::readlink),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_readlinkat call. (...)
ssize_t LdPreloadedPosix::ld_preloaded_posix_readlinkat (int dirfd,
    const char* path,
    char* buf,
    size_t bufsize)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-readlinkat (" + std::to_string (dirfd)
            + ", " + std::string (path) + ")");
    }

    // hook POSIX readlinkat operation to m_metadata_operations.m_readlinkat
    this->m_dlsym_hook.hook_posix_readlinkat (m_metadata_operations.m_readlinkat);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX readlinkat operation
    ssize_t result = m_metadata_operations.m_readlinkat (dirfd, path, buf, bufsize);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::readlinkat),
                1,
                result);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::readlinkat),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fopen call. (...)
FILE* LdPreloadedPosix::ld_preloaded_posix_fopen (const char* pathname, const char* mode)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fopen (" + std::string (pathname) + ")");
    }

    // hook POSIX fopen operation to m_metadata_operations.m_fopen
    this->m_dlsym_hook.hook_posix_fopen (m_metadata_operations.m_fopen);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fopen operation
    FILE* result = m_metadata_operations.m_fopen (pathname, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result != nullptr) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fopen),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fopen),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fopen64 call. (...)
FILE* LdPreloadedPosix::ld_preloaded_posix_fopen64 (const char* pathname, const char* mode)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug (
            "ld_preloaded_posix-fopen64 (" + std::string (pathname) + ")");
    }

    // hook POSIX fopen64 operation to m_metadata_operations.m_fopen64
    this->m_dlsym_hook.hook_posix_fopen64 (m_metadata_operations.m_fopen64);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fopen64 operation
    FILE* result = m_metadata_operations.m_fopen64 (pathname, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result != nullptr) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fopen64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fopen64),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fdopen call. (...)
FILE* LdPreloadedPosix::ld_preloaded_posix_fdopen (int fd, const char* mode)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fdopen (" + std::to_string (fd) + ")");
    }

    // hook POSIX fdopen operation to m_metadata_operations.m_fdopen
    this->m_dlsym_hook.hook_posix_fdopen (m_metadata_operations.m_fdopen);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fdopen operation
    FILE* result = m_metadata_operations.m_fdopen (fd, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result != nullptr) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fdopen),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fdopen),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_freopen call. (...)
FILE* LdPreloadedPosix::ld_preloaded_posix_freopen (const char* pathname,
    const char* mode,
    FILE* stream)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug (
            "ld_preloaded_posix-freopen (" + std::string (pathname) + ")");
    }

    // hook POSIX freopen operation to m_metadata_operations.m_freopen
    this->m_dlsym_hook.hook_posix_freopen (m_metadata_operations.m_freopen);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX freopen operation
    FILE* result = m_metadata_operations.m_freopen (pathname, mode, stream);

    // update statistic entry
    if (this->m_collect) {
        if (result != nullptr) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::freopen),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::freopen),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_freopen64 call. (...)
FILE* LdPreloadedPosix::ld_preloaded_posix_freopen64 (const char* pathname,
    const char* mode,
    FILE* stream)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug (
            "ld_preloaded_posix-freopen64 (" + std::string (pathname) + ")");
    }

    // hook POSIX freopen64 operation to m_metadata_operations.m_freopen64
    this->m_dlsym_hook.hook_posix_freopen64 (m_metadata_operations.m_freopen64);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX freopen64 operation
    FILE* result = m_metadata_operations.m_freopen64 (pathname, mode, stream);

    // update statistic entry
    if (this->m_collect) {
        if (result != nullptr) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::freopen64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::freopen64),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fclose call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fclose (FILE* stream)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fclose");
    }

    // hook POSIX fclose operation to m_metadata_operations.m_fclose
    this->m_dlsym_hook.hook_posix_fclose (m_metadata_operations.m_fclose);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fclose operation
    int result = m_metadata_operations.m_fclose (stream);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fclose),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fclose),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fflush call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fflush (FILE* stream)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fflush");
    }

    // hook POSIX fflush operation to m_metadata_operations.m_fflush
    this->m_dlsym_hook.hook_posix_fflush (m_metadata_operations.m_fflush);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fflush operation
    int result = m_metadata_operations.m_fflush (stream);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fflush),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fflush),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_access call. (...)
int LdPreloadedPosix::ld_preloaded_posix_access (const char* path, int mode)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-access");
    }

    // hook POSIX access operation to m_metadata_operations.m_access
    this->m_dlsym_hook.hook_posix_access (m_metadata_operations.m_access);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX access operation
    int result = m_metadata_operations.m_access (path, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::access),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::access),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_faccessat call. (...)
int LdPreloadedPosix::ld_preloaded_posix_faccessat (int dirfd,
    const char* path,
    int mode,
    int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-faccessat");
    }

    // hook POSIX faccessat operation to m_metadata_operations.m_faccessat
    this->m_dlsym_hook.hook_posix_faccessat (m_metadata_operations.m_faccessat);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX faccessat operation
    int result = m_metadata_operations.m_faccessat (dirfd, path, mode, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::faccessat),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::faccessat),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_lseek call. (...)
off_t LdPreloadedPosix::ld_preloaded_posix_lseek (int fd, off_t offset, int whence)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-lseek");
    }

    // hook POSIX lseek operation to m_metadata_operations.m_lseek
    this->m_dlsym_hook.hook_posix_lseek (m_metadata_operations.m_lseek);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX lseek operation
    off_t result = m_metadata_operations.m_lseek (fd, offset, whence);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::lseek),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::lseek),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fseek call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fseek (FILE* stream, long offset, int whence)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fseek");
    }

    // hook POSIX fseek operation to m_metadata_operations.m_fseek
    this->m_dlsym_hook.hook_posix_fseek (m_metadata_operations.m_fseek);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fseek operation
    int result = m_metadata_operations.m_fseek (stream, offset, whence);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fseek),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fseek),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_ftell call. (...)
long LdPreloadedPosix::ld_preloaded_posix_ftell (FILE* stream)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-ftell");
    }

    // hook POSIX ftell operation to m_metadata_operations.m_ftell
    this->m_dlsym_hook.hook_posix_ftell (m_metadata_operations.m_ftell);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX ftell operation
    long result = m_metadata_operations.m_ftell (stream);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::ftell),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::ftell),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_lseek64 call. (...)
off_t LdPreloadedPosix::ld_preloaded_posix_lseek64 (int fd, off_t offset, int whence)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-lseek64");
    }

    // hook POSIX lseek64 operation to m_metadata_operations.m_lseek64
    this->m_dlsym_hook.hook_posix_lseek64 (m_metadata_operations.m_lseek64);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX lseek64 operation
    off_t result = m_metadata_operations.m_lseek64 (fd, offset, whence);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::lseek64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::lseek64),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fseeko64 call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fseeko64 (FILE* stream, off_t offset, int whence)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fseeko64");
    }

    // hook POSIX fseeko64 operation to m_metadata_operations.m_fseeko64
    this->m_dlsym_hook.hook_posix_fseeko64 (m_metadata_operations.m_fseeko64);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fseeko64 operation
    int result = m_metadata_operations.m_fseeko64 (stream, offset, whence);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fseeko64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::fseeko64),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_ftello64 call. (...)
off_t LdPreloadedPosix::ld_preloaded_posix_ftello64 (FILE* stream)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-ftello64");
    }

    // hook POSIX ftello64 operation to m_metadata_operations.m_ftello64
    this->m_dlsym_hook.hook_posix_ftello64 (m_metadata_operations.m_ftello64);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX ftello64 operation
    long result = m_metadata_operations.m_ftello64 (stream);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::ftello64),
                1,
                0);
        } else {
            this->m_metadata_stats.update_statistic_entry (static_cast<int> (Metadata::ftello64),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_mkdir call. (...)
int LdPreloadedPosix::ld_preloaded_posix_mkdir (const char* path, mode_t mode)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-mkdir (" + std::string (path) + ")");
    }

    // hook POSIX mkdir operation to m_directory_operations.m_mkdir
    this->m_dlsym_hook.hook_posix_mkdir (m_directory_operations.m_mkdir);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX mkdir operation
    int result = m_directory_operations.m_mkdir (path, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::mkdir), 1, 0);
        } else {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::mkdir), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_mkdirat call. (...)
int LdPreloadedPosix::ld_preloaded_posix_mkdirat (int dirfd, const char* path, mode_t mode)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-mkdirat (" + std::to_string (dirfd)
            + ", " + std::string (path) + ")");
    }

    // hook POSIX mkdirat operation to m_directory_operations.m_mkdirat
    this->m_dlsym_hook.hook_posix_mkdirat (m_directory_operations.m_mkdirat);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX mkdirat operation
    int result = m_directory_operations.m_mkdirat (dirfd, path, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::mkdirat), 1, 0);
        } else {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::mkdirat),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_readdir call. (...)
struct dirent* LdPreloadedPosix::ld_preloaded_posix_readdir (DIR* dirp)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-readdir");
    }

    // hook POSIX readdir operation to m_directory_operations.m_readdir
    this->m_dlsym_hook.hook_posix_readdir (m_directory_operations.m_readdir);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX readdir operation
    struct dirent* entry = m_directory_operations.m_readdir (dirp);

    // update statistic entry
    if (this->m_collect) {
        if (entry != nullptr) {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::readdir), 1, 0);
        } else {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::readdir),
                1,
                0,
                1);
        }
    }

    return entry;
}

// ld_preloaded_posix_readdir64 call. (...)
struct dirent64* LdPreloadedPosix::ld_preloaded_posix_readdir64 (DIR* dirp)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-readdir64");
    }

    // hook POSIX readdir64 operation to m_directory_operations.m_readdir64
    this->m_dlsym_hook.hook_posix_readdir64 (m_directory_operations.m_readdir64);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX readdir64 operation
    struct dirent64* entry = m_directory_operations.m_readdir64 (dirp);

    // update statistic entry
    if (this->m_collect) {
        if (entry != nullptr) {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::readdir64),
                1,
                0);
        } else {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::readdir64),
                1,
                0,
                1);
        }
    }

    return entry;
}

// ld_preloaded_posix_opendir call. (...)
DIR* LdPreloadedPosix::ld_preloaded_posix_opendir (const char* path)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-opendir (" + std::string (path) + ")");
    }

    // hook POSIX opendir operation to m_directory_operations.m_opendir
    this->m_dlsym_hook.hook_posix_opendir (m_directory_operations.m_opendir);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX opendir operation
    DIR* folder = m_directory_operations.m_opendir (path);

    // update statistic entry
    if (this->m_collect) {
        if (folder != nullptr) {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::opendir), 1, 0);
        } else {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::opendir),
                1,
                0,
                1);
        }
    }

    return folder;
}

// ld_preloaded_posix_fdopendir call. (...)
DIR* LdPreloadedPosix::ld_preloaded_posix_fdopendir (int fd)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug (
            "ld_preloaded_posix-fdopendir (" + std::to_string (fd) + ")");
    }

    // hook POSIX fdopendir operation to m_directory_operations.m_fdopendir
    this->m_dlsym_hook.hook_posix_fdopendir (m_directory_operations.m_fdopendir);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fopendir operation
    DIR* folder = m_directory_operations.m_fdopendir (fd);

    // update statistic entry
    if (this->m_collect) {
        if (folder != nullptr) {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::fdopendir),
                1,
                0);
        } else {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::fdopendir),
                1,
                0,
                1);
        }
    }

    return folder;
}

// ld_preloaded_posix_closedir call. (...)
int LdPreloadedPosix::ld_preloaded_posix_closedir (DIR* dirp)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-closedir");
    }

    // hook POSIX closedir operation to m_directory_operations.m_closedir
    this->m_dlsym_hook.hook_posix_closedir (m_directory_operations.m_closedir);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX closedir operation
    int result = m_directory_operations.m_closedir (dirp);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::closedir), 1, 0);
        } else {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::closedir),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_rmdir call. (...)
int LdPreloadedPosix::ld_preloaded_posix_rmdir (const char* path)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-rmdir");
    }

    // hook POSIX rmdir operation to m_directory_operations.m_rmdir
    this->m_dlsym_hook.hook_posix_rmdir (m_directory_operations.m_rmdir);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX rmdir operation
    int result = m_directory_operations.m_rmdir (path);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::rmdir), 1, 0);
        } else {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::rmdir), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_dirfd call. (...)
int LdPreloadedPosix::ld_preloaded_posix_dirfd (DIR* dirp)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-dirfd");
    }

    // hook POSIX dirfd operation to m_directory_operations.m_dirfd
    this->m_dlsym_hook.hook_posix_dirfd (m_directory_operations.m_dirfd);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX dirfd operation
    int result = m_directory_operations.m_dirfd (dirp);

    // update statistic entry
    if (this->m_collect) {
        if (result >= 0) {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::dirfd), 1, 0);
        } else {
            this->m_dir_stats.update_statistic_entry (static_cast<int> (Directory::dirfd), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_getxattr call. (...)
ssize_t LdPreloadedPosix::ld_preloaded_posix_getxattr (const char* path,
    const char* name,
    void* value,
    size_t size)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug (
            "ld_preloaded_posix-getxattr (" + std::string (path) + ", " + std::string (name) + ")");
    }

    // hook POSIX getxattr operation to m_extattr_operations.m_getxattr
    this->m_dlsym_hook.hook_posix_getxattr (m_extattr_operations.m_getxattr);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX getxattr operation
    ssize_t result = m_extattr_operations.m_getxattr (path, name, value, size);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::getxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats
                .update_statistic_entry (static_cast<int> (ExtendedAttributes::getxattr), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_lgetxattr call. (...)
ssize_t LdPreloadedPosix::ld_preloaded_posix_lgetxattr (const char* path,
    const char* name,
    void* value,
    size_t size)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-lgetxattr (" + std::string (path) + ", "
            + std::string (name) + ")");
    }

    // hook POSIX lgetxattr operation to m_extattr_operations.m_lgetxattr
    this->m_dlsym_hook.hook_posix_lgetxattr (m_extattr_operations.m_lgetxattr);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX lgetxattr operation
    ssize_t result = m_extattr_operations.m_lgetxattr (path, name, value, size);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::lgetxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats
                .update_statistic_entry (static_cast<int> (ExtendedAttributes::lgetxattr), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_fgetxattr call. (...)
ssize_t
LdPreloadedPosix::ld_preloaded_posix_fgetxattr (int fd, const char* name, void* value, size_t size)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fgetxattr (" + std::to_string (fd) + ", "
            + std::string (name) + ")");
    }

    // hook POSIX fgetxattr operation to m_extattr_operations.m_fgetxattr
    this->m_dlsym_hook.hook_posix_fgetxattr (m_extattr_operations.m_fgetxattr);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fgetxattr operation
    ssize_t result = m_extattr_operations.m_fgetxattr (fd, name, value, size);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::fgetxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats
                .update_statistic_entry (static_cast<int> (ExtendedAttributes::fgetxattr), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_setxattr call. (...)
int LdPreloadedPosix::ld_preloaded_posix_setxattr (const char* path,
    const char* name,
    const void* value,
    size_t size,
    int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug (
            "ld_preloaded_posix-setxattr (" + std::string (path) + ", " + std::string (name) + ")");
    }

    // hook POSIX setxattr operation to m_extattr_operations.m_setxattr
    this->m_dlsym_hook.hook_posix_setxattr (m_extattr_operations.m_setxattr);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX setxattr operation
    int result = m_extattr_operations.m_setxattr (path, name, value, size, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::setxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats
                .update_statistic_entry (static_cast<int> (ExtendedAttributes::setxattr), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_lsetxattr call. (...)
int LdPreloadedPosix::ld_preloaded_posix_lsetxattr (const char* path,
    const char* name,
    const void* value,
    size_t size,
    int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-lsetxattr (" + std::string (path) + ", "
            + std::string (name) + ")");
    }

    // hook POSIX lsetxattr operation to m_extattr_operations.m_lsetxattr
    this->m_dlsym_hook.hook_posix_lsetxattr (m_extattr_operations.m_lsetxattr);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX lsetxattr operation
    int result = m_extattr_operations.m_lsetxattr (path, name, value, size, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::lsetxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats
                .update_statistic_entry (static_cast<int> (ExtendedAttributes::lsetxattr), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_fsetxattr call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fsetxattr (int fd,
    const char* name,
    const void* value,
    size_t size,
    int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fsetxattr (" + std::to_string (fd) + ", "
            + std::string (name) + ")");
    }

    // hook POSIX fsetxattr operation to m_extattr_operations.m_fsetxattr
    this->m_dlsym_hook.hook_posix_fsetxattr (m_extattr_operations.m_fsetxattr);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fsetxattr operation
    int result = m_extattr_operations.m_fsetxattr (fd, name, value, size, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::fsetxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats
                .update_statistic_entry (static_cast<int> (ExtendedAttributes::fsetxattr), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_listxattr call. (...)
ssize_t LdPreloadedPosix::ld_preloaded_posix_listxattr (const char* path, char* list, size_t size)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-listxattr (" + std::string (path) + ")");
    }

    // hook POSIX listxattr operation to m_extattr_operations.m_listxattr
    this->m_dlsym_hook.hook_posix_listxattr (m_extattr_operations.m_listxattr);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX listxattr operation
    ssize_t result = m_extattr_operations.m_listxattr (path, list, size);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::listxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats
                .update_statistic_entry (static_cast<int> (ExtendedAttributes::listxattr), 1, 0, 1);
        }
    }

    return result;
}

// ld_preloaded_posix_llistxattr call. (...)
ssize_t LdPreloadedPosix::ld_preloaded_posix_llistxattr (const char* path, char* list, size_t size)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug (
            "ld_preloaded_posix-llistxattr (" + std::string (path) + ")");
    }

    // hook POSIX llistxattr operation to m_extattr_operations.m_llistxattr
    this->m_dlsym_hook.hook_posix_llistxattr (m_extattr_operations.m_llistxattr);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX llistxattr operation
    ssize_t result = m_extattr_operations.m_llistxattr (path, list, size);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::llistxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::llistxattr),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_flistxattr call. (...)
ssize_t LdPreloadedPosix::ld_preloaded_posix_flistxattr (int fd, char* list, size_t size)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug (
            "ld_preloaded_posix-flistxattr (" + std::to_string (fd) + ")");
    }

    // hook POSIX flistxattr operation to m_extattr_operations.m_flistxattr
    this->m_dlsym_hook.hook_posix_flistxattr (m_extattr_operations.m_flistxattr);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX flistxattr operation
    ssize_t result = m_extattr_operations.m_flistxattr (fd, list, size);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::flistxattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::flistxattr),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_removexattr call. (...)
int LdPreloadedPosix::ld_preloaded_posix_removexattr (const char* path, const char* name)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-removexattr (" + std::string (path)
            + ", " + std::string (name) + ")");
    }

    // hook POSIX removexattr operation to m_extattr_operations.m_removexattr
    this->m_dlsym_hook.hook_posix_removexattr (m_extattr_operations.m_removexattr);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX removexattr operation
    int result = m_extattr_operations.m_removexattr (path, name);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::removexattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::removexattr),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_lremovexattr call. (...)
int LdPreloadedPosix::ld_preloaded_posix_lremovexattr (const char* path, const char* name)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-lremovexattr (" + std::string (path)
            + ", " + std::string (name) + ")");
    }

    // hook POSIX lremovexattr operation to m_extattr_operations.m_lremovexattr
    this->m_dlsym_hook.hook_posix_lremovexattr (m_extattr_operations.m_lremovexattr);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX lremovexattr operation
    int result = m_extattr_operations.m_lremovexattr (path, name);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::lremovexattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::lremovexattr),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fremovexattr call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fremovexattr (int fd, const char* name)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fremovexattr (" + std::to_string (fd)
            + ", " + std::string (name) + ")");
    }

    // hook POSIX fremovexattr operation to m_extattr_operations.m_fremovexattr
    this->m_dlsym_hook.hook_posix_fremovexattr (m_extattr_operations.m_fremovexattr);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fremovexattr operation
    int result = m_extattr_operations.m_fremovexattr (fd, name);

    // update statistic entry
    if (this->m_collect) {
        if (result != -1) {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::fremovexattr),
                1,
                0);
        } else {
            this->m_ext_attr_stats.update_statistic_entry (
                static_cast<int> (ExtendedAttributes::fremovexattr),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_chmod call. (...)
int LdPreloadedPosix::ld_preloaded_posix_chmod (const char* path, mode_t mode)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-chmod (" + std::string (path) + ")");
    }

    // hook POSIX chmod operation to m_filemodes_operations.m_chmod
    this->m_dlsym_hook.hook_posix_chmod (m_filemodes_operations.m_chmod);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX chmod operation
    int result = m_filemodes_operations.m_chmod (path, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::chmod),
                1,
                0);
        } else {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::chmod),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fchmod call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fchmod (int fd, mode_t mode)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fchmod (" + std::to_string (fd) + ")");
    }

    // hook POSIX fchmod operation to m_filemodes_operations.m_fchmod
    this->m_dlsym_hook.hook_posix_fchmod (m_filemodes_operations.m_fchmod);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fchmod operation
    int result = m_filemodes_operations.m_fchmod (fd, mode);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::fchmod),
                1,
                0);
        } else {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::fchmod),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fchmodat call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fchmodat (int dirfd,
    const char* path,
    mode_t mode,
    int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fchmodat (" + std::to_string (dirfd)
            + ", " + std::string (path) + ")");
    }

    // hook POSIX fchmodat operation to m_filemodes_operations.m_fchmodat
    this->m_dlsym_hook.hook_posix_fchmodat (m_filemodes_operations.m_fchmodat);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fchmodat operation
    int result = m_filemodes_operations.m_fchmodat (dirfd, path, mode, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::fchmodat),
                1,
                0);
        } else {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::fchmodat),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_chown call. (...)
int LdPreloadedPosix::ld_preloaded_posix_chown (const char* pathname, uid_t owner, gid_t group)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-chown (" + std::string (pathname) + ")");
    }

    // hook POSIX chown operation to m_filemodes_operations.m_chown
    this->m_dlsym_hook.hook_posix_chown (m_filemodes_operations.m_chown);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX chown operation
    int result = m_filemodes_operations.m_chown (pathname, owner, group);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::chown),
                1,
                0);
        } else {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::chown),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_lchown call. (...)
int LdPreloadedPosix::ld_preloaded_posix_lchown (const char* pathname, uid_t owner, gid_t group)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug (
            "ld_preloaded_posix-lchown (" + std::string (pathname) + ")");
    }

    // hook POSIX lchown operation to m_filemodes_operations.m_lchown
    this->m_dlsym_hook.hook_posix_lchown (m_filemodes_operations.m_lchown);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX lchown operation
    int result = m_filemodes_operations.m_lchown (pathname, owner, group);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::lchown),
                1,
                0);
        } else {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::lchown),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fchown call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fchown (int fd, uid_t owner, gid_t group)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fchown (" + std::to_string (fd) + ")");
    }

    // hook POSIX fchown operation to m_filemodes_operations.m_fchown
    this->m_dlsym_hook.hook_posix_fchown (m_filemodes_operations.m_fchown);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fchown operation
    int result = m_filemodes_operations.m_fchown (fd, owner, group);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::fchown),
                1,
                0);
        } else {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::fchown),
                1,
                0,
                1);
        }
    }

    return result;
}

// ld_preloaded_posix_fchownat call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fchownat (int dirfd,
    const char* pathname,
    uid_t owner,
    gid_t group,
    int flags)
{
    // logging message
    if (option_default_detailed_logging) {
        this->m_logger_ptr->log_debug ("ld_preloaded_posix-fchownat (" + std::to_string (dirfd)
            + ", " + std::string (pathname) + ")");
    }

    // hook POSIX fchownat operation to m_filemodes_operations.m_fchownat
    this->m_dlsym_hook.hook_posix_fchownat (m_filemodes_operations.m_fchownat);

    // TODO: add here call to the paio-stage
    // this->enforce_request (...);

    // perform original POSIX fchownat operation
    int result = m_filemodes_operations.m_fchownat (dirfd, pathname, owner, group, flags);

    // update statistic entry
    if (this->m_collect) {
        if (result == 0) {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::fchownat),
                1,
                0);
        } else {
            this->m_file_mode_stats.update_statistic_entry (static_cast<int> (FileModes::fchownat),
                1,
                0,
                1);
        }
    }

    return result;
}

} // namespace padll
