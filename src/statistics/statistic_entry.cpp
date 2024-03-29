/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#include <cinttypes>
#include <padll/statistics/statistic_entry.hpp>

namespace padll::stats {

// StatisticEntry default constructor.
StatisticEntry::StatisticEntry () = default;

// StatisticEntry parameterized constructor.
StatisticEntry::StatisticEntry (const std::string& name) : m_entry_name { name }
{ }

// StatisticEntry copy constructor.
StatisticEntry::StatisticEntry (const StatisticEntry& entry) :
    m_entry_name { entry.m_entry_name },
    m_operation_counter { entry.m_operation_counter },
    m_byte_counter { entry.m_byte_counter },
    m_error_counter { entry.m_error_counter },
    m_bypass_counter { entry.m_bypass_counter }
{ }

// StatisticEntry default destructor.
StatisticEntry::~StatisticEntry () = default;

// get_entry_name call. Get the name of the StatisticEntry object.
std::string StatisticEntry::get_entry_name () const
{
    return this->m_entry_name;
}

// get_operation_counter call. Get the number of registered operations.
uint64_t StatisticEntry::get_operation_counter ()
{
    // lock_guard over mutex
    std::lock_guard lock (this->m_lock);
    return this->m_operation_counter;
}

// get_byte_counter call. Get the number of registered bytes.
uint64_t StatisticEntry::get_byte_counter ()
{
    // lock_guard over mutex
    std::lock_guard lock (this->m_lock);
    return this->m_byte_counter;
}

// get_error_counter call. Get the number of registered errors.
uint64_t StatisticEntry::get_error_counter ()
{
    // lock_guard over mutex
    std::lock_guard lock (this->m_lock);
    return this->m_error_counter;
}

// get_bypass_counter call. Get the number of registered bypassed operations.
uint64_t StatisticEntry::get_bypass_counter ()
{
    // lock_guard over mutex
    std::lock_guard lock (this->m_lock);
    return this->m_bypass_counter;
}

// increment_operation_counter call. Increments the number of operations.
void StatisticEntry::increment_operation_counter (const uint64_t& count)
{
    // lock_guard over mutex
    std::lock_guard lock (this->m_lock);
    this->m_operation_counter += count;
}

// increment_byte_counter call. Increments the number of bytes.
void StatisticEntry::increment_byte_counter (const uint64_t& bytes)
{
    // lock_guard over mutex
    std::lock_guard lock (this->m_lock);
    this->m_byte_counter += bytes;
}

// increment_error_counter call. Increments the number of errors.
void StatisticEntry::increment_error_counter (const uint64_t& count)
{
    // lock_guard over mutex
    std::lock_guard lock (this->m_lock);
    this->m_error_counter += count;
}

// increment_bypass_counter call. Increments the number of bypass operations.
void StatisticEntry::increment_bypass_counter (const uint64_t& count)
{
    // lock_guard over mutex
    std::lock_guard lock (this->m_lock);
    this->m_bypass_counter += count;
}

// to_string call. Generate a string-based format of the contents of the StatisticEntry object.
std::string StatisticEntry::to_string ()
{
    // lock_guard over mutex
    std::lock_guard lock (this->m_lock);

    // TODO: use fmtlib/fmt for easier and faster formatting
    char stream[75];
    std::sprintf (stream,
        "%15s %12" PRIu64 " %12" PRIu64 " %12" PRIu64 " %15" PRIu64 "",
        this->m_entry_name.c_str (),
        this->m_operation_counter,
        this->m_error_counter,
        this->m_bypass_counter,
        this->m_byte_counter);

    return { stream };
}

} // namespace padll::stats
