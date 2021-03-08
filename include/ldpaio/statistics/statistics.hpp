/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef LDPAIO_STATISTICS_H
#define LDPAIO_STATISTICS_H

#include <iostream>
#include <ldpaio/statistics/statistic_entry.hpp>
#include <ldpaio/utils/logging.hpp>
#include <ldpaio/utils/operation_utils.hpp>
#include <vector>

namespace ldpaio {

/**
 * Statistics class.
 * missing: complete me ...
 */
class Statistics {

private:
    std::string m_stats_identifier { "stats" };
    int m_stats_size { 0 };
    std::vector<StatisticEntry> m_statistic_entries {};
    std::mutex m_stats_mutex;

public:
    /**
     * Statistics default constructor.
     */
    Statistics ();

    /**
     * Statistics parameterized constructor.
     * @param operation_type
     */
    Statistics (std::string identifier, const OperationType& operation_type);

    /**
     * Statistics default destructor.
     */
    ~Statistics ();

    /**
     * initialize: initializes the m_statistic_entries container with the respective operations
     * to be collected.
     * This method is thread-safe.
     * @param operation_type Defines hte type of statistics to be collected.
     *  - OperationType::metadata_calls receives Metadata-based operations;
     *  - OperationType::data_calls receives Data-based operations;
     *  - OperationType::directory_calls receives Directory-based operations;
     *  - OperationType::ext_attr_calls receives ExtendedAttributes-based operations.
     */
    void initialize (const OperationType& operation_type);

    /**
     * update_statistic_entry: update both byte and operation counters of a specific StatisticEntry
     * of the m_statistics_entries container.
     * @param operation_type Defines the operation entry to be registered.
     * @param operation_value Defines the value to be incremented in the operation counter.
     * @param byte_value Defines the value to be incremented in the bytes counter.
     */
    void update_statistic_entry (const int& operation_type,
        const uint64_t& operation_value,
        const uint64_t& byte_value);

    /**
     * update_statistic_entry: update both byte, operation, and error counters of a specific
     * StatisticEntry of the m_statistics_entries container.
     * @param operation_type Defines the operation entry to be registered.
     * @param operation_value Defines the value to be incremented in the operation counter.
     * @param byte_value Defines the value to be incremented in the bytes counter.
     * @param error_value Defines the value to be incremented in the error counter.
     */
    void update_statistic_entry (const int& operation_type,
        const uint64_t& operation_value,
        const uint64_t& byte_value,
        const uint64_t& error_value);

    /**
     * get_stats_identifier_call: get the identifier of the Statistics object.
     * @return Returns a copy of the m_stats_identifier parameter.
     */
    std::string get_stats_identifier () const;

    /**
     * get_stats_size: Get the total of statistics entries stored in the container.
     * @return Returns a copy of the m_stats_size parameter.
     */
    int get_stats_size () const;

    /**
     * to_string: generate a string with all statistic entries stored in the m_statistic_entries
     * container.
     * @return Returns the description of all StatisticEntry objects in string-based format.
     */
    std::string to_string ();
};
} // namespace ldpaio

#endif // LDPAIO_STATISTICS_H
