/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#ifndef LDPAIO_STATISTICS_H
#define LDPAIO_STATISTICS_H

#include <iostream>
#include <ldpaio/statistics/statistic_entry.hpp>
#include <ldpaio/utils/operation_utils.hpp>
#include <magic_enum.hpp>
#include <vector>

namespace ldpaio {

/**
 * Statistics class.
 * missing: complete me ...
 */
class Statistics {

private:
    int m_stats_size { 0 };
    std::vector<StatisticEntry> m_statistic_entries {};
    std::mutex m_stats_mutex;

public:
    /**
     * Statistics default constructor.
     */
    Statistics ();

    /**
     * Statistics default destructor.
     */
    ~Statistics ();

    /**
     * initialize: initializes the m_statistic_entries container with the respective operations
     * to be collected.
     * This method is thread-safe.
     * @param operation_type DEfines hte type of statistics to be collected.
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
     * get_stats_size: Get the total of statistics entries stored in the container.
     * @return Returns a copy of the m_stats_size parameter.
     */
    [[nodiscard]] int get_stats_size () const;

    /**
     * to_string: generate a string with all statistic entries stored in the m_statistic_entries
     * container.
     * @return Returns the description of all StatisticEntry objects in string-based format.
     */
    std::string to_string ();
};
} // namespace ldpaio

#endif // LDPAIO_STATISTICS_H
