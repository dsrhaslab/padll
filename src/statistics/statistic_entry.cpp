/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <padll/statistics/statistic_entry.hpp>

namespace padll {

// StatisticEntry default constructor.
StatisticEntry::StatisticEntry () = default;

// StatisticEntry parameterized constructor.
StatisticEntry::StatisticEntry (std::string name) : m_entry_name { std::move (name) }
{ }

StatisticEntry::StatisticEntry (const StatisticEntry& entry) :
    m_entry_name { entry.m_entry_name },
    m_operation_counter { entry.m_operation_counter },
    m_byte_counter { entry.m_byte_counter },
    m_error_counter { entry.m_error_counter }
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
    std::unique_lock<std::mutex> unique_lock (this->m_lock);
    return this->m_operation_counter;
}

// get_byte_counter call. Get the number of registered bytes.
uint64_t StatisticEntry::get_byte_counter ()
{
    std::unique_lock<std::mutex> unique_lock (this->m_lock);
    return this->m_byte_counter;
}

// get_error_counter call. Get the number of registered errors.
uint64_t StatisticEntry::get_error_counter ()
{
    std::unique_lock<std::mutex> unique_lock (this->m_lock);
    return this->m_error_counter;
}

// increment_operation_counter call. Increments the number of operations.
void StatisticEntry::increment_operation_counter (const uint64_t& count)
{
    std::unique_lock<std::mutex> unique_lock (this->m_lock);
    this->m_operation_counter += count;
}

// increment_byte_counter call. Increments the number of bytes.
void StatisticEntry::increment_byte_counter (const uint64_t& bytes)
{
    std::unique_lock<std::mutex> unique_lock (this->m_lock);
    this->m_byte_counter += bytes;
}

// increment_error_counter call. Increments the number of errors.
void StatisticEntry::increment_error_counter (const uint64_t& count)
{
    std::unique_lock<std::mutex> unique_lock (this->m_lock);
    this->m_error_counter += count;
}

// to_string call. Generate a string-based format of the contents of the StatisticEntry object.
std::string StatisticEntry::to_string ()
{
    std::unique_lock<std::mutex> unique_lock (this->m_lock);
    std::stringstream stream;

    stream << "\t" << this->m_entry_name << "\t";
    stream << "\t" << this->m_operation_counter << "\t";
    stream << "\t" << this->m_byte_counter << "\t";
    stream << "\t" << this->m_error_counter;

//    stream << "{" << this->m_entry_name << ": ";
//    stream << this->m_operation_counter << ", ";
//    stream << this->m_byte_counter << ", ";
//    stream << this->m_error_counter << "}";

    return stream.str ();
}

} // namespace padll
