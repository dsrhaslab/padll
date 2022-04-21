/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
 **/

#include <padll/interface/ldpreloaded/ld_preloaded_posix.hpp>

namespace padll::interface::ldpreloaded {

// LdPreloadedPosix default constructor.
LdPreloadedPosix::LdPreloadedPosix () :
    m_log { std::make_shared<Log> (option_default_enable_debug_level,
        option_default_enable_debug_with_ld_preload,
        std::string { option_default_log_path }) },
    m_dlsym_hook { option_library_name, this->m_log },
    m_loaded { std::make_shared<std::atomic<bool>>(false)}
{
    // create logging message
    std::stringstream stream;
    stream << "LdPreloadedPosix default constructor ";
    stream << "(" << static_cast<void*> (this->m_log.get ()) << ")";

    // write debug logging message
    this->m_log->log_info (stream.str ());
    
    // set loaded
    this->set_loaded (true);
}

// LdPreloadedPosix parameterized constructor.
LdPreloadedPosix::LdPreloadedPosix (const std::string& lib,
    const bool& stat_collection,
    std::shared_ptr<Log> log_ptr,
    std::shared_ptr<std::atomic<bool>> loaded_ptr) :
    m_log { log_ptr },
    m_dlsym_hook { lib, this->m_log },
    m_collect { stat_collection },
    m_loaded { loaded_ptr }
{
    // create logging message
    std::stringstream stream;
    stream << "LdPreloadedPosix parameterized constructor ";
    stream << "(" << static_cast<void*> (this->m_log.get ()) << ")";

    // write debug logging message
    this->m_log->log_info (stream.str ());

    // set loaded
    this->set_loaded (true);
}

// LdPreloadedPosix default destructor.
LdPreloadedPosix::~LdPreloadedPosix ()
{
    // create logging message
    this->m_log->log_info ("LdPreloadedPosix default destructor.");

    // log LdPreloadedPosix statistic counters
    if (option_default_table_format) {
        // print to stdout metadata-based statistics in tabular format
        this->m_metadata_stats.tabulate ();
        // print to stdout data-based statistics in tabular format
        this->m_data_stats.tabulate ();
        // print to stdout directory-based statistics in tabular format
        this->m_dir_stats.tabulate ();
        // print to stdout extended attributes based statistics in tabular format
        this->m_ext_attr_stats.tabulate ();
        // print to stdout special calls based statistics in tabular format
        this->m_special_stats.tabulate ();
    } else {
        this->generate_statistics_report (option_default_statistics_report_path);
    }
}

// set_loaded call. (...)
void LdPreloadedPosix::set_loaded (const bool& value) 
{
    std::cout << "LdPreloadedPosix: " << __func__ << std::endl;
    this->m_loaded->store (value);
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

        case OperationType::special_calls:
            return this->m_special_stats.get_statistic_entry (operation_entry);

        default:
            return StatisticEntry {};
    }
}

// to_string call. (...)
std::string LdPreloadedPosix::to_string ()
{
    std::stringstream stream;
    stream << "----------------------------------------------------------------------\n";
    stream << "LdPreloadedPosix Statistics (" << ::getpid () << ", " << ::getppid () << ")\n";
    stream << "----------------------------------------------------------------------\n";
    stream << this->m_metadata_stats.to_string (true);
    stream << this->m_data_stats.to_string (false);
    stream << this->m_dir_stats.to_string (false);
    stream << this->m_ext_attr_stats.to_string (false);
    stream << this->m_special_stats.to_string (false);

    return stream.str ();
}

// generate_statistics_report call. (...)
void LdPreloadedPosix::generate_statistics_report (const std::string_view& path)
{
    if (option_default_save_statistics_report) {
        std::string filename;
        filename.append (path);
        filename.append ("/");
        filename.append ("padll-ldpreloaded-stats-").append (std::to_string (::getpid ()));
        filename.append (".stat");

        // hook m_fopen to the fopen function if null
        if (m_metadata_operations.m_fopen == nullptr) {
            this->m_dlsym_hook.hook_posix_fopen (m_metadata_operations.m_fopen);
        }

        // hook m_fclose to the close function if null
        if (m_metadata_operations.m_fclose == nullptr) {
            this->m_dlsym_hook.hook_posix_fclose (m_metadata_operations.m_fclose);
        }

        FILE* fptr = m_metadata_operations.m_fopen (filename.c_str (), "w");

        if (fptr != nullptr) {
            std::string report = this->to_string ();
            std::fprintf (fptr, "%s", report.c_str ());
            m_metadata_operations.m_fclose (fptr);
        } else {
            this->m_log->log_error ("Error while opening statistics report file.");
        }
    } else {
        this->m_log->log_info (this->to_string ());
    }
}

// get_metadata_unit call. TODO: finish the implementation ...
[[nodiscard]] uint32_t LdPreloadedPosix::get_metadata_unit ([[maybe_unused]] const char* path) const 
{
    if (option_select_workflow_by_metadata_unit) {
        std::cerr << __func__ << ": missing implementation." << std::endl;
    }

    return static_cast<uint32_t> (-1);
}

// enforce_request call. (...)
[[nodiscard]] bool LdPreloadedPosix::enforce_request (const std::string_view& function_name,
    const uint32_t& workflow_id,
    const int& operation_type,
    const int& operation_context,
    const int& payload)
{
    // validate if workflow-id is valid
    auto is_valid = (workflow_id != static_cast<uint32_t> (-1));

    if (is_valid) {
        // enforce request to PAIO data plane stage
        this->m_stage->enforce_request (workflow_id, operation_type, operation_context, payload);
    } else {
// create logging message
#if OPTION_DETAILED_LOGGING
        std::stringstream stream;
        stream << function_name << ": operation bypassed (" << static_cast<int> (workflow_id) << ","
               << payload << ")";
        this->m_log->log_debug (stream.str ());
#endif
    }

    return is_valid;
}

// update_statistic_entry_data call. (...)
void LdPreloadedPosix::update_statistic_entry_data (const int& operation,
    const ssize_t& bytes,
    const bool& enforced)
{
    if (enforced) {
        (bytes >= 0) ? this->m_data_stats.update_statistic_entry (operation, 1, bytes)
                     : this->m_data_stats.update_statistic_entry (operation, 1, 0, 1);
    } else {
        this->m_data_stats.update_bypassed_statistic_entry (operation, 1);
    }
}

// update_statistic_entry_metadata call. (...)
void LdPreloadedPosix::update_statistic_entry_metadata (const int& operation,
    const int& result,
    const bool& enforced)
{
    if (enforced) {
        (result >= 0) ? this->m_metadata_stats.update_statistic_entry (operation, 1, 0)
                      : this->m_metadata_stats.update_statistic_entry (operation, 1, 0, 1);
    } else {
        this->m_metadata_stats.update_bypassed_statistic_entry (operation, 1);
    }
}

// update_statistic_entry_dir call. (...)
void LdPreloadedPosix::update_statistic_entry_dir (const int& operation,
    const int& result,
    const bool& enforced)
{
    if (enforced) {
        (result == 0) ? this->m_dir_stats.update_statistic_entry (operation, 1, 0)
                      : this->m_dir_stats.update_statistic_entry (operation, 1, 0, 1);
    } else {
        this->m_dir_stats.update_bypassed_statistic_entry (operation, 1);
    }
}

// update_statistic_entry_ext_attr call. (...)
void LdPreloadedPosix::update_statistic_entry_ext_attr (const int& operation,
    const ssize_t& result,
    const bool& enforced)
{
    if (enforced) {
        (result >= 0) ? this->m_ext_attr_stats.update_statistic_entry (operation, 1, result)
                      : this->m_ext_attr_stats.update_statistic_entry (operation, 1, 0, 1);
    } else {
        this->m_ext_attr_stats.update_bypassed_statistic_entry (operation, 1);
    }
}

// update_statistic_entry_special call. (...)
void LdPreloadedPosix::update_statistic_entry_special (const int& operation,
    const int& result,
    const bool& enforced)
{
    if (enforced) {
        (result >= 0) ? this->m_special_stats.update_statistic_entry (operation, 1, 0)
                      : this->m_special_stats.update_statistic_entry (operation, 1, 0, 1);
    } else {
        this->m_special_stats.update_bypassed_statistic_entry (operation, 1);
    }
}

// update_statistics call. (...)
void LdPreloadedPosix::update_statistics (const OperationType& operation_type,
    const int& operation,
    const long& result,
    const bool& enforced)
{
    if (this->m_collect) {
        switch (operation_type) {
            case OperationType::data_calls:
                this->update_statistic_entry_data (operation, result, enforced);
                break;

            case OperationType::metadata_calls:
                this->update_statistic_entry_metadata (operation, result, enforced);
                break;

            case OperationType::directory_calls:
                this->update_statistic_entry_dir (operation, result, enforced);
                break;

            case OperationType::ext_attr_calls:
                this->update_statistic_entry_ext_attr (operation, result, enforced);
                break;

            case OperationType::special_calls:
                this->update_statistic_entry_special (operation, result, enforced);
                break;

            default:
                break;
        }
    }
}

// ld_preloaded_posix_read call.
ssize_t LdPreloadedPosix::ld_preloaded_posix_read (int fd, void* buf, size_t counter)
{
    // hook POSIX read operation to m_data_operations.m_read
    this->m_dlsym_hook.hook_posix_read (m_data_operations.m_read);

    // select workflow-id to submit I/O request
    auto workflow_id = this->m_mount_point_table.pick_workflow_id (fd);

    // enforce read request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::read),
        static_cast<int> (paio::core::POSIX_META::data_op),
        counter);

    // perform original POSIX read operation
    ssize_t result = m_data_operations.m_read (fd, buf, counter);

    // update statistic entry
    this->update_statistics (OperationType::data_calls,
        static_cast<int> (Data::read),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_write call.
ssize_t LdPreloadedPosix::ld_preloaded_posix_write (int fd, const void* buf, size_t counter)
{
    // hook POSIX write operation to m_data_operations.m_write
    this->m_dlsym_hook.hook_posix_write (m_data_operations.m_write);

    // select workflow-id to submit I/O request
    auto workflow_id = this->m_mount_point_table.pick_workflow_id (fd);

    // enforce write request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::write),
        static_cast<int> (paio::core::POSIX_META::data_op),
        counter);

    // perform original POSIX write operation
    ssize_t result = m_data_operations.m_write (fd, buf, counter);

    // update statistic entry
    this->update_statistics (OperationType::data_calls,
        static_cast<int> (Data::write),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_pread call.
ssize_t LdPreloadedPosix::ld_preloaded_posix_pread (int fd, void* buf, size_t counter, off_t offset)
{
    // hook POSIX pread operation to m_data_operations.m_pread
    this->m_dlsym_hook.hook_posix_pread (m_data_operations.m_pread);

    // select workflow-id to submit I/O request
    auto workflow_id = this->m_mount_point_table.pick_workflow_id (fd);

    // enforce pread request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::pread),
        static_cast<int> (paio::core::POSIX_META::data_op),
        counter);

    // perform original POSIX pread operation
    ssize_t result = m_data_operations.m_pread (fd, buf, counter, offset);

    // update statistic entry
    this->update_statistics (OperationType::data_calls,
        static_cast<int> (Data::pread),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_pwrite call.
ssize_t
LdPreloadedPosix::ld_preloaded_posix_pwrite (int fd, const void* buf, size_t counter, off_t offset)
{
    // hook POSIX pwrite operation to m_data_operations.m_pwrite
    this->m_dlsym_hook.hook_posix_pwrite (m_data_operations.m_pwrite);

    // select workflow-id to submit I/O request
    auto workflow_id = this->m_mount_point_table.pick_workflow_id (fd);

    // enforce pwrite request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::pwrite),
        static_cast<int> (paio::core::POSIX_META::data_op),
        counter);

    // perform original POSIX pwrite operation
    ssize_t result = m_data_operations.m_pwrite (fd, buf, counter, offset);

    // update statistic entry
    this->update_statistics (OperationType::data_calls,
        static_cast<int> (Data::pwrite),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_pread64 call.
#if defined(__USE_LARGEFILE64)
ssize_t
LdPreloadedPosix::ld_preloaded_posix_pread64 (int fd, void* buf, size_t counter, off64_t offset)
{
    // hook POSIX pread64 operation to m_data_operations.m_pread64
    this->m_dlsym_hook.hook_posix_pread64 (m_data_operations.m_pread64);

    // select workflow-id to submit I/O request
    auto workflow_id = this->m_mount_point_table.pick_workflow_id (fd);

    // enforce pread64 request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::pread64),
        static_cast<int> (paio::core::POSIX_META::data_op),
        counter);

    // perform original POSIX pread64 operation
    ssize_t result = m_data_operations.m_pread64 (fd, buf, counter, offset);

    // update statistic entry
    this->update_statistics (OperationType::data_calls,
        static_cast<int> (Data::pread64),
        result,
        enforced);

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
    // hook POSIX pwrite64 operation to m_data_operations.m_pwrite64
    this->m_dlsym_hook.hook_posix_pwrite64 (m_data_operations.m_pwrite64);

    // select workflow-id to submit I/O request
    auto workflow_id = this->m_mount_point_table.pick_workflow_id (fd);

    // enforce pwrite64 request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::pwrite64),
        static_cast<int> (paio::core::POSIX_META::data_op),
        counter);

    // perform original POSIX pwrite64 operation
    ssize_t result = m_data_operations.m_pwrite64 (fd, buf, counter, offset);

    // update statistic entry
    this->update_statistics (OperationType::data_calls,
        static_cast<int> (Data::pwrite64),
        result,
        enforced);

    return result;
}
#endif

// ld_preloaded_posix_mmap call.
void* LdPreloadedPosix::ld_preloaded_posix_mmap (void* addr,
    size_t lenght,
    int prot,
    int flags,
    int fd,
    off_t offset)
{
    // hook POSIX mmap operation to m_data_operations.m_mmap
    this->m_dlsym_hook.hook_posix_mmap (m_data_operations.m_mmap);

    // select workflow-id to submit I/O request
    auto workflow_id = this->m_mount_point_table.pick_workflow_id (fd);

    // enforce write request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::mmap),
        static_cast<int> (paio::core::POSIX_META::data_op),
        1);

    // perform original POSIX write operation
    void* mem_ptr = m_data_operations.m_mmap (addr, lenght, prot, flags, fd, offset);

    // validate memory pointer
    auto result = (mem_ptr == MAP_FAILED) ? -1 : 0;

    // update statistic entry
    this->update_statistics (OperationType::data_calls,
        static_cast<int> (Data::mmap),
        result,
        enforced);

    return mem_ptr;
}

// ld_preloaded_posix_munmap call.
int LdPreloadedPosix::ld_preloaded_posix_munmap (void* addr, size_t lenght)
{
    // hook POSIX munmap operation to m_data_operations.m_munmap
    this->m_dlsym_hook.hook_posix_munmap (m_data_operations.m_munmap);

    // BUG: Reported defects -@gsd at 4/15/2022, 3:01:31 PM
    // Not sure how to handle this scenario ...
    // select workflow-id to submit I/O request
    auto workflow_id = this->m_mount_point_table.pick_workflow_id (-1);

    // enforce write request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::munmap),
        static_cast<int> (paio::core::POSIX_META::data_op),
        1);

    // perform original POSIX write operation
    auto result = m_data_operations.m_munmap (addr, lenght);

    // update statistic entry
    this->update_statistics (OperationType::data_calls,
        static_cast<int> (Data::munmap),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_open call.
int LdPreloadedPosix::ld_preloaded_posix_open (const char* path, int flags, mode_t mode)
{
    // hook POSIX open operation to m_metadata_operations.m_open_var
    this->m_dlsym_hook.hook_posix_open_var (m_metadata_operations.m_open_var);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce open request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::open),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX open operation
    int fd = m_metadata_operations.m_open_var (path, flags, mode);

    // create_mount_point_entry
    this->m_mount_point_table.create_mount_point_entry (fd, path, mountpoint, this->get_metadata_unit (path));

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::open_variadic),
        fd,
        enforced);

    return fd;
}

// ld_preloaded_posix_open call.
int LdPreloadedPosix::ld_preloaded_posix_open (const char* path, int flags)
{
    // hook POSIX open operation to m_metadata_operations.m_open
    this->m_dlsym_hook.hook_posix_open (m_metadata_operations.m_open);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce open request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::open),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX open operation
    int fd = m_metadata_operations.m_open (path, flags);

    // create_mount_point_entry
    this->m_mount_point_table.create_mount_point_entry (fd, path, mountpoint, this->get_metadata_unit (path));

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::open),
        fd,
        enforced);

    return fd;
}

// ld_preloaded_posix_creat call.
int LdPreloadedPosix::ld_preloaded_posix_creat (const char* path, mode_t mode)
{
    // hook POSIX creat operation to m_metadata_operations.m_creat
    this->m_dlsym_hook.hook_posix_creat (m_metadata_operations.m_creat);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce creat request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::creat),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX creat operation
    int fd = m_metadata_operations.m_creat (path, mode);

    // create_mount_point_entry
    this->m_mount_point_table.create_mount_point_entry (fd, path, mountpoint, this->get_metadata_unit (path));

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::creat),
        fd,
        enforced);

    return fd;
}

// ld_preloaded_posix_creat64 call.
int LdPreloadedPosix::ld_preloaded_posix_creat64 (const char* path, mode_t mode)
{
    // hook POSIX creat64 operation to m_metadata_operations.m_creat64
    this->m_dlsym_hook.hook_posix_creat64 (m_metadata_operations.m_creat64);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce creat64 request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::creat64),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX creat64 operation
    int fd = m_metadata_operations.m_creat64 (path, mode);

    // create_mount_point_entry
    this->m_mount_point_table.create_mount_point_entry (fd, path, mountpoint, this->get_metadata_unit (path));

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::creat64),
        fd,
        enforced);

    return fd;
}

// ld_preloaded_posix_openat call.
int LdPreloadedPosix::ld_preloaded_posix_openat (int dirfd,
    const char* path,
    int flags,
    mode_t mode)
{
    // hook POSIX openat variadic operation to m_metadata_operations.m_openat_var
    this->m_dlsym_hook.hook_posix_openat_var (m_metadata_operations.m_openat_var);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce openat request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::openat),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX openat operation
    int fd = m_metadata_operations.m_openat_var (dirfd, path, flags, mode);

    // create_mount_point_entry
    this->m_mount_point_table.create_mount_point_entry (fd, path, mountpoint, this->get_metadata_unit (path));

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::openat_variadic),
        fd,
        enforced);

    return fd;
}

// ld_preloaded_posix_openat call.
int LdPreloadedPosix::ld_preloaded_posix_openat (int dirfd, const char* path, int flags)
{
    // hook POSIX openat operation to m_metadata_operations.m_openat
    this->m_dlsym_hook.hook_posix_openat (m_metadata_operations.m_openat);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce openat request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::openat),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX openat operation
    int fd = m_metadata_operations.m_openat (dirfd, path, flags);

    // create_mount_point_entry
    this->m_mount_point_table.create_mount_point_entry (fd, path, mountpoint, this->get_metadata_unit (path));

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::openat),
        fd,
        enforced);

    return fd;
}

// ld_preloaded_posix_open64 call. (...)
int LdPreloadedPosix::ld_preloaded_posix_open64 (const char* path, int flags, mode_t mode)
{
    // hook POSIX open64_var operation to m_metadata_operations.m_open64_var
    this->m_dlsym_hook.hook_posix_open64_variadic (m_metadata_operations.m_open64_var);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce open64 request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::open64),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX open64 operation
    int fd = m_metadata_operations.m_open64_var (path, flags, mode);

    // create_mount_point_entry
    this->m_mount_point_table.create_mount_point_entry (fd, path, mountpoint, this->get_metadata_unit (path));

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::open64_variadic),
        fd,
        enforced);

    return fd;
}

// ld_preloaded_posix_open64 call. (...)
int LdPreloadedPosix::ld_preloaded_posix_open64 (const char* path, int flags)
{
    // hook POSIX open64 operation to m_metadata_operations.m_open64
    this->m_dlsym_hook.hook_posix_open64 (m_metadata_operations.m_open64);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce open64 request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::open64),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX open64 operation
    int fd = m_metadata_operations.m_open64 (path, flags);

    // create_mount_point_entry
    this->m_mount_point_table.create_mount_point_entry (fd, path, mountpoint, this->get_metadata_unit (path));

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::open64),
        fd,
        enforced);

    return fd;
}

// ld_preloaded_posix_close call.
int LdPreloadedPosix::ld_preloaded_posix_close (int fd)
{
    // hook POSIX close operation to m_metadata_operations.m_close
    this->m_dlsym_hook.hook_posix_close (m_metadata_operations.m_close);

    // select workflow-id to submit I/O request
    auto workflow_id = this->m_mount_point_table.pick_workflow_id (fd);

    // enforce close request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::close),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX close operation
    int result = m_metadata_operations.m_close (fd);

    // remove entry from MountPointTable
    if (option_hard_remove) {
        this->m_mount_point_table.remove_mount_point_entry (fd);
    }

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::close),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_sync call. (...)
void LdPreloadedPosix::ld_preloaded_posix_sync ()
{
    // hook POSIX sync operation to m_metadata_operations.m_sync
    this->m_dlsym_hook.hook_posix_sync (m_metadata_operations.m_sync);

    // enforce sync request to PAIO data plane stage
    // TODO: don't really know what to do here
    // this->m_stage->enforce_request (this->m_mount_point_table.pick_workflow_id (path),
    // static_cast<int> (paio::core::POSIX::sync),
    //    static_cast<int> (paio::core::POSIX_META::meta_op),
    //    1);

    // perform original POSIX sync operation
    m_metadata_operations.m_sync ();

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::sync),
        1,
        true);
}

// ld_preloaded_posix_statfs call. (...)
int LdPreloadedPosix::ld_preloaded_posix_statfs (const char* path, struct statfs* buf)
{
    // hook POSIX statfs operation to m_metadata_operations.m_statfs
    this->m_dlsym_hook.hook_posix_statfs (m_metadata_operations.m_statfs);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce statfs request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::statfs),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX statfs operation
    int result = m_metadata_operations.m_statfs (path, buf);

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::statfs),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_fstatfs call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fstatfs (int fd, struct statfs* buf)
{
    // hook POSIX fstatfs operation to m_metadata_operations.m_fstatfs
    this->m_dlsym_hook.hook_posix_fstatfs (m_metadata_operations.m_fstatfs);

    // select workflow-id to submit I/O request
    auto workflow_id = this->m_mount_point_table.pick_workflow_id (fd);

    // enforce fstatfs request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::fstatfs),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX fstatfs operation
    int result = m_metadata_operations.m_fstatfs (fd, buf);

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::fstatfs),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_statfs64 call. (...)
int LdPreloadedPosix::ld_preloaded_posix_statfs64 (const char* path, struct statfs64* buf)
{
    // hook POSIX statfs64 operation to m_metadata_operations.m_statfs64
    this->m_dlsym_hook.hook_posix_statfs64 (m_metadata_operations.m_statfs64);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce statfs64 request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::statfs64),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX statfs64 operation
    int result = m_metadata_operations.m_statfs64 (path, buf);

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::statfs64),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_fstatfs64 call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fstatfs64 (int fd, struct statfs64* buf)
{
    // hook POSIX fstatfs64 operation to m_metadata_operations.m_fstatfs64
    this->m_dlsym_hook.hook_posix_fstatfs64 (m_metadata_operations.m_fstatfs64);

    // select workflow-id to submit I/O request
    auto workflow_id = this->m_mount_point_table.pick_workflow_id (fd);

    // enforce fstatfs64 request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::fstatfs64),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX fstatfs64 operation
    int result = m_metadata_operations.m_fstatfs64 (fd, buf);

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::fstatfs64),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_unlink call. (...)
int LdPreloadedPosix::ld_preloaded_posix_unlink (const char* path)
{
    // hook POSIX unlink operation to m_metadata_operations.m_unlink
    this->m_dlsym_hook.hook_posix_unlink (m_metadata_operations.m_unlink);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce unlink request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::unlink),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX unlink operation
    int result = m_metadata_operations.m_unlink (path);

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::unlink),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_unlinkat call. (...)
int LdPreloadedPosix::ld_preloaded_posix_unlinkat (int dirfd, const char* pathname, int flags)
{
    // hook POSIX unlinkat operation to m_metadata_operations.m_unlinkat
    this->m_dlsym_hook.hook_posix_unlinkat (m_metadata_operations.m_unlinkat);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (pathname);

    // enforce unlinkat request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::unlinkat),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX unlinkat operation
    int result = m_metadata_operations.m_unlinkat (dirfd, pathname, flags);

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::unlinkat),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_rename call. (...)
int LdPreloadedPosix::ld_preloaded_posix_rename (const char* old_path, const char* new_path)
{
    // hook POSIX rename operation to m_metadata_operations.m_rename
    this->m_dlsym_hook.hook_posix_rename (m_metadata_operations.m_rename);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (new_path);

    // enforce rename request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::rename),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX rename operation
    int result = m_metadata_operations.m_rename (old_path, new_path);

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::rename),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_renameat call. (...)
int LdPreloadedPosix::ld_preloaded_posix_renameat (int olddirfd,
    const char* old_path,
    int newdirfd,
    const char* new_path)
{
    // hook POSIX renameat operation to m_metadata_operations.m_renameat
    this->m_dlsym_hook.hook_posix_renameat (m_metadata_operations.m_renameat);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (new_path);

    // enforce renameat request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::renameat),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX renameat operation
    int result = m_metadata_operations.m_renameat (olddirfd, old_path, newdirfd, new_path);

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::renameat),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_fopen call. (...)
FILE* LdPreloadedPosix::ld_preloaded_posix_fopen (const char* pathname, const char* mode)
{
    // hook POSIX fopen operation to m_metadata_operations.m_fopen
    this->m_dlsym_hook.hook_posix_fopen (m_metadata_operations.m_fopen);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (pathname);

    // enforce fopen request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::fopen),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX fopen operation
    FILE* fptr = m_metadata_operations.m_fopen (pathname, mode);

    // create_mount_point_entry
    this->m_mount_point_table.create_mount_point_entry (fptr, pathname, mountpoint, this->get_metadata_unit (pathname));

    // verify if fopen operation was successful
    auto result = (fptr != nullptr) ? 0 : -1;

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::fopen),
        result,
        enforced);

    return fptr;
}

// ld_preloaded_posix_fopen64 call. (...)
FILE* LdPreloadedPosix::ld_preloaded_posix_fopen64 (const char* pathname, const char* mode)
{
    // hook POSIX fopen64 operation to m_metadata_operations.m_fopen64
    this->m_dlsym_hook.hook_posix_fopen64 (m_metadata_operations.m_fopen64);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (pathname);

    // enforce fopen64 request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::fopen64),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX fopen64 operation
    FILE* fptr = m_metadata_operations.m_fopen64 (pathname, mode);

    // create_mount_point_entry
    this->m_mount_point_table.create_mount_point_entry (fptr, pathname, mountpoint, this->get_metadata_unit (pathname));

    // verify if fopen operation was successful
    auto result = (fptr != nullptr) ? 0 : -1;

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::fopen64),
        result,
        enforced);

    return fptr;
}

// ld_preloaded_posix_fclose call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fclose (FILE* stream)
{
    // hook POSIX fclose operation to m_metadata_operations.m_fclose
    this->m_dlsym_hook.hook_posix_fclose (m_metadata_operations.m_fclose);

    // select workflow-id to submit I/O request
    auto workflow_id = this->m_mount_point_table.pick_workflow_id (stream);

    // enforce fstatfs request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::fclose),
        static_cast<int> (paio::core::POSIX_META::meta_op),
        1);

    // perform original POSIX fclose operation
    int result = m_metadata_operations.m_fclose (stream);

    // remove entry from MountPointTable
    this->m_mount_point_table.remove_mount_point_entry (stream);

    // update statistic entry
    this->update_statistics (OperationType::metadata_calls,
        static_cast<int> (Metadata::fclose),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_mkdir call. (...)
int LdPreloadedPosix::ld_preloaded_posix_mkdir (const char* path, mode_t mode)
{
    // hook POSIX mkdir operation to m_directory_operations.m_mkdir
    this->m_dlsym_hook.hook_posix_mkdir (m_directory_operations.m_mkdir);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce mkdir request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::mkdir),
        static_cast<int> (paio::core::POSIX_META::dir_op),
        1);

    // perform original POSIX mkdir operation
    int result = m_directory_operations.m_mkdir (path, mode);

    // update statistic entry
    this->update_statistics (OperationType::directory_calls,
        static_cast<int> (Directory::mkdir),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_mkdirat call. (...)
int LdPreloadedPosix::ld_preloaded_posix_mkdirat (int dirfd, const char* path, mode_t mode)
{
    // hook POSIX mkdirat operation to m_directory_operations.m_mkdirat
    this->m_dlsym_hook.hook_posix_mkdirat (m_directory_operations.m_mkdirat);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce mkdirat request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::mkdirat),
        static_cast<int> (paio::core::POSIX_META::dir_op),
        1);

    // perform original POSIX mkdirat operation
    int result = m_directory_operations.m_mkdirat (dirfd, path, mode);

    // update statistic entry
    this->update_statistics (OperationType::directory_calls,
        static_cast<int> (Directory::mkdirat),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_mknod call. (...)
int LdPreloadedPosix::ld_preloaded_posix_mknod (const char* path, mode_t mode, dev_t dev)
{
    // hook POSIX mknod operation to m_directory_operations.m_mknod
    this->m_dlsym_hook.hook_posix_mknod (m_directory_operations.m_mknod);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce mknod request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::mknod),
        static_cast<int> (paio::core::POSIX_META::dir_op),
        1);

    // perform original POSIX mknod operation
    int result = m_directory_operations.m_mknod (path, mode, dev);

    // update statistic entry
    this->update_statistics (OperationType::directory_calls,
        static_cast<int> (Directory::mknod),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_mknodat call. (...)
int LdPreloadedPosix::ld_preloaded_posix_mknodat (int dirfd,
    const char* path,
    mode_t mode,
    dev_t dev)
{
    // hook POSIX mknodat operation to m_directory_operations.m_mknodat
    this->m_dlsym_hook.hook_posix_mknodat (m_directory_operations.m_mknodat);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce mknodat request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::mknodat),
        static_cast<int> (paio::core::POSIX_META::dir_op),
        1);

    // perform original POSIX mknod operation
    int result = m_directory_operations.m_mknodat (dirfd, path, mode, dev);

    // update statistic entry
    this->update_statistics (OperationType::directory_calls,
        static_cast<int> (Directory::mknodat),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_rmdir call. (...)
int LdPreloadedPosix::ld_preloaded_posix_rmdir (const char* path)
{
    // hook POSIX rmdir operation to m_directory_operations.m_rmdir
    this->m_dlsym_hook.hook_posix_rmdir (m_directory_operations.m_rmdir);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce rmdir request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::rmdir),
        static_cast<int> (paio::core::POSIX_META::dir_op),
        1);

    // perform original POSIX rmdir operation
    int result = m_directory_operations.m_rmdir (path);

    // update statistic entry
    this->update_statistics (OperationType::directory_calls,
        static_cast<int> (Directory::rmdir),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_getxattr call. (...)
ssize_t LdPreloadedPosix::ld_preloaded_posix_getxattr (const char* path,
    const char* name,
    void* value,
    size_t size)
{
    // hook POSIX getxattr operation to m_extattr_operations.m_getxattr
    this->m_dlsym_hook.hook_posix_getxattr (m_extattr_operations.m_getxattr);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce getxattr request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::getxattr),
        static_cast<int> (paio::core::POSIX_META::ext_attr_op),
        1);

    // perform original POSIX getxattr operation
    ssize_t result = m_extattr_operations.m_getxattr (path, name, value, size);

    // update statistic entry
    this->update_statistics (OperationType::ext_attr_calls,
        static_cast<int> (ExtendedAttributes::getxattr),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_lgetxattr call. (...)
ssize_t LdPreloadedPosix::ld_preloaded_posix_lgetxattr (const char* path,
    const char* name,
    void* value,
    size_t size)
{
    // hook POSIX lgetxattr operation to m_extattr_operations.m_lgetxattr
    this->m_dlsym_hook.hook_posix_lgetxattr (m_extattr_operations.m_lgetxattr);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce lgetxattr request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::lgetxattr),
        static_cast<int> (paio::core::POSIX_META::ext_attr_op),
        1);

    // perform original POSIX lgetxattr operation
    ssize_t result = m_extattr_operations.m_lgetxattr (path, name, value, size);

    // update statistic entry
    this->update_statistics (OperationType::ext_attr_calls,
        static_cast<int> (ExtendedAttributes::lgetxattr),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_fgetxattr call. (...)
ssize_t
LdPreloadedPosix::ld_preloaded_posix_fgetxattr (int fd, const char* name, void* value, size_t size)
{
    // hook POSIX fgetxattr operation to m_extattr_operations.m_fgetxattr
    this->m_dlsym_hook.hook_posix_fgetxattr (m_extattr_operations.m_fgetxattr);

    // select workflow-id to submit I/O request
    auto workflow_id = this->m_mount_point_table.pick_workflow_id (fd);

    // enforce fgetxattr request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::fgetxattr),
        static_cast<int> (paio::core::POSIX_META::ext_attr_op),
        1);

    // perform original POSIX fgetxattr operation
    ssize_t result = m_extattr_operations.m_fgetxattr (fd, name, value, size);

    // update statistic entry
    this->update_statistics (OperationType::ext_attr_calls,
        static_cast<int> (ExtendedAttributes::fgetxattr),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_setxattr call. (...)
int LdPreloadedPosix::ld_preloaded_posix_setxattr (const char* path,
    const char* name,
    const void* value,
    size_t size,
    int flags)
{
    // hook POSIX setxattr operation to m_extattr_operations.m_setxattr
    this->m_dlsym_hook.hook_posix_setxattr (m_extattr_operations.m_setxattr);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce setxattr request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::setxattr),
        static_cast<int> (paio::core::POSIX_META::ext_attr_op),
        1);

    // perform original POSIX setxattr operation
    int result = m_extattr_operations.m_setxattr (path, name, value, size, flags);

    // update statistic entry
    this->update_statistics (OperationType::ext_attr_calls,
        static_cast<int> (ExtendedAttributes::setxattr),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_lsetxattr call. (...)
int LdPreloadedPosix::ld_preloaded_posix_lsetxattr (const char* path,
    const char* name,
    const void* value,
    size_t size,
    int flags)
{
    // hook POSIX lsetxattr operation to m_extattr_operations.m_lsetxattr
    this->m_dlsym_hook.hook_posix_lsetxattr (m_extattr_operations.m_lsetxattr);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce lsetxattr request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::lsetxattr),
        static_cast<int> (paio::core::POSIX_META::ext_attr_op),
        1);

    // perform original POSIX lsetxattr operation
    int result = m_extattr_operations.m_lsetxattr (path, name, value, size, flags);

    // update statistic entry
    this->update_statistics (OperationType::ext_attr_calls,
        static_cast<int> (ExtendedAttributes::lsetxattr),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_fsetxattr call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fsetxattr (int fd,
    const char* name,
    const void* value,
    size_t size,
    int flags)
{
    // hook POSIX fsetxattr operation to m_extattr_operations.m_fsetxattr
    this->m_dlsym_hook.hook_posix_fsetxattr (m_extattr_operations.m_fsetxattr);

    // select workflow-id to submit I/O request
    auto workflow_id = this->m_mount_point_table.pick_workflow_id (fd);

    // enforce fsetxattr request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::fsetxattr),
        static_cast<int> (paio::core::POSIX_META::ext_attr_op),
        1);

    // perform original POSIX fsetxattr operation
    int result = m_extattr_operations.m_fsetxattr (fd, name, value, size, flags);

    // update statistic entry
    this->update_statistics (OperationType::ext_attr_calls,
        static_cast<int> (ExtendedAttributes::fsetxattr),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_listxattr call. (...)
ssize_t LdPreloadedPosix::ld_preloaded_posix_listxattr (const char* path, char* list, size_t size)
{
    // hook POSIX listxattr operation to m_extattr_operations.m_listxattr
    this->m_dlsym_hook.hook_posix_listxattr (m_extattr_operations.m_listxattr);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce listxattr request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::listxattr),
        static_cast<int> (paio::core::POSIX_META::ext_attr_op),
        1);

    // perform original POSIX listxattr operation
    ssize_t result = m_extattr_operations.m_listxattr (path, list, size);

    // update statistic entry
    this->update_statistics (OperationType::ext_attr_calls,
        static_cast<int> (ExtendedAttributes::listxattr),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_llistxattr call. (...)
ssize_t LdPreloadedPosix::ld_preloaded_posix_llistxattr (const char* path, char* list, size_t size)
{
    // hook POSIX llistxattr operation to m_extattr_operations.m_llistxattr
    this->m_dlsym_hook.hook_posix_llistxattr (m_extattr_operations.m_llistxattr);

    // extract mountpoint and pick workflow-id
    auto [mountpoint, workflow_id] = this->m_mount_point_table.pick_workflow_id (path);

    // enforce llistxattr request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::llistxattr),
        static_cast<int> (paio::core::POSIX_META::ext_attr_op),
        1);

    // perform original POSIX llistxattr operation
    ssize_t result = m_extattr_operations.m_llistxattr (path, list, size);

    // update statistic entry
    this->update_statistics (OperationType::ext_attr_calls,
        static_cast<int> (ExtendedAttributes::llistxattr),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_flistxattr call. (...)
ssize_t LdPreloadedPosix::ld_preloaded_posix_flistxattr (int fd, char* list, size_t size)
{
    // hook POSIX flistxattr operation to m_extattr_operations.m_flistxattr
    this->m_dlsym_hook.hook_posix_flistxattr (m_extattr_operations.m_flistxattr);

    // select workflow-id to submit I/O request
    auto workflow_id = this->m_mount_point_table.pick_workflow_id (fd);

    // enforce flistxattr request to PAIO data plane stage
    auto enforced = this->enforce_request (__func__,
        workflow_id,
        static_cast<int> (paio::core::POSIX::flistxattr),
        static_cast<int> (paio::core::POSIX_META::ext_attr_op),
        1);

    // perform original POSIX flistxattr operation
    ssize_t result = m_extattr_operations.m_flistxattr (fd, list, size);

    // update statistic entry
    this->update_statistics (OperationType::ext_attr_calls,
        static_cast<int> (ExtendedAttributes::flistxattr),
        result,
        enforced);

    return result;
}

// ld_preloaded_posix_socket call. (...)
int LdPreloadedPosix::ld_preloaded_posix_socket (int domain, int type, int protocol)
{
    // hook POSIX socket operation to m_special_operations.m_socket
    this->m_dlsym_hook.hook_posix_socket (m_special_operations.m_socket);

    // perform original POSIX socket operation
    int fd = m_special_operations.m_socket (domain, type, protocol);

// detailed logging message
#if OPTION_DETAILED_LOGGING
    this->m_log->log_debug (std::string { __func__ } + " : " + std::to_string (fd));
#endif

    // update statistic entry
    this->update_statistics (OperationType::special_calls,
        static_cast<int> (Special::socket),
        fd,
        false);

    return fd;
}

// ld_preloaded_posix_fcntl call. (...)
int LdPreloadedPosix::ld_preloaded_posix_fcntl (int fd, int cmd, void* arg)
{
    // hook POSIX fcntl operation to m_special_operations.m_socket
    this->m_dlsym_hook.hook_posix_fcntl (m_special_operations.m_fcntl);

    // perform original POSIX fcntl operation
    int result_value = m_special_operations.m_fcntl (fd, cmd, arg);

    switch (cmd) {
        case F_DUPFD:
        case F_DUPFD_CLOEXEC: {
// detailed logging message
#if OPTION_DETAILED_LOGGING
            this->m_log->log_debug (std::string { __func__ } + " duplicated file descriptor "
                + std::to_string (fd) + " to " + std::to_string (result_value));
#endif
            // replace fd with entry in m_fd_table
            auto replaced = this->m_mount_point_table.replace_file_descriptor (fd, result_value);

            if (!replaced) {
                this->m_log->log_error ("Error while replacing file descriptor.");
            }
            break;
        }

        default: {
// detailed logging message
#if OPTION_DETAILED_LOGGING
            this->m_log->log_debug (std::string { __func__ } + " : " + std::to_string (result_value)
                + ", " + std::to_string (cmd));
#endif
            break;
        }
    }

    // update statistic entry
    this->update_statistics (OperationType::special_calls,
        static_cast<int> (Special::fcntl),
        result_value,
        false);

    return result_value;
}

} // namespace padll::interface::ldpreloaded
