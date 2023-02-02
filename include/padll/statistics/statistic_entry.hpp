/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#ifndef PADLL_STATISTIC_ENTRY_H
#define PADLL_STATISTIC_ENTRY_H

#include <cstdint>
#include <mutex>
#include <sstream>
#include <string>

namespace padll::stats {

/**
 * StatisticEntry class.
 * Missing: complete me ...
 */
class StatisticEntry {

private:
    std::string m_entry_name {};
    uint64_t m_operation_counter { 0 };
    uint64_t m_byte_counter { 0 };
    uint64_t m_error_counter { 0 };
    uint64_t m_bypass_counter { 0 };
    std::mutex m_lock;

public:
    /**
     * StatisticEntry default constructor.
     */
    StatisticEntry ();

    /**
     * StatisticEntry parameterized constructor.
     * @param name
     */
    explicit StatisticEntry (const std::string& name);

    /**
     * StatisticEntry copy constructor.
     * @param entry
     */
    StatisticEntry (const StatisticEntry& entry);

    /**
     * StatisticEntry default destructor.
     */
    ~StatisticEntry ();

    /**
     * get_entry_name: Get the name of the StatisticEntry object.
     * @return Returns a copy of the m_entry_name parameter.
     */
    [[nodiscard]] std::string get_entry_name () const;

    /**
     * get_operation_counter: Get the total number of operations registered (counted) at the
     * StatisticEntry object.
     * This method is thread-safe.
     * @return Returns a copy of the m_operation_counter parameter.
     */
    [[nodiscard]] uint64_t get_operation_counter ();

    /**
     * get_byte_counter: Get the total number of bytes registered (counted) at the StatisticEntry
     * object.
     * This method is thread-safe.
     * @return Returns a copy of the m_byte_counter parameter.
     */
    [[nodiscard]] uint64_t get_byte_counter ();

    /**
     * get_error_counter: Get the total number of errors registered (counted) at the StatisticEntry
     * object.
     * This method is thread-safe.
     * @return Returns a copy of the m_error_counter parameter.
     */
    [[nodiscard]] uint64_t get_error_counter ();

    /**
     * get_bypass_counter: Get the total number of bypassed operations registered (counted) at the
     * StatisticEntry object. This method is thread-safe.
     * @return Returns a copy of the m_bypass_counter parameter.
     */
    [[nodiscard]] uint64_t get_bypass_counter ();

    /**
     * increment_operation_counter: Increments the total number of operations of the StatisticEntry
     * object by count.
     * This method is thread-safe.
     * @param count Defines the amount of operations to be incremented.
     */
    void increment_operation_counter (const uint64_t& count);

    /**
     * increment_byte_counter: Increments the total number of bytes of the StatisticEntry object by
     * bytes.
     * This method is thread-safe.
     * @param bytes Defines the amount of bytes to be incremented.
     */
    void increment_byte_counter (const uint64_t& bytes);

    /**
     * increment_error_counter: Increments the total times that a given syscalls had an error.
     * This method is thread-safe.
     * @param count Defines the amount of errors to be incremented.
     */
    void increment_error_counter (const uint64_t& count);

    /**
     * increment_bypass_counter: Increments the total times that a given syscalls has been bypassed.
     * This method is thread-safe.
     * @param count Defines the amount of bypassed operations to be incremented.
     */
    void increment_bypass_counter (const uint64_t& count);

    /**
     * to_string: generate a string-based format of the contents of the StatisticEntry object.
     * @return String containing the current values of all StatisticEntry elements.
     */
    std::string to_string ();
};
} // namespace padll::stats

#endif // PADLL_STATISTIC_ENTRY_H
