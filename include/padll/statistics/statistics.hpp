/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#ifndef PADLL_STATISTICS_H
#define PADLL_STATISTICS_H

#include <fcntl.h>
#include <iostream>
#include <padll/library_headers/libc_enums.hpp>
#include <padll/statistics/statistic_entry.hpp>
#include <padll/third_party/tabulate.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using namespace padll::headers;
using namespace tabulate;

namespace padll::stats {

/**
 * Statistics class.
 * This class keeps track of all statistics entries of a given category.
 */
class Statistics {

private:
    std::string m_stats_identifier { "stats" };
    int m_stats_size { 0 };
    std::vector<StatisticEntry> m_statistic_entries {};
    std::mutex m_stats_mutex;

    /**
     * aggregate_counters:
     * @param type
     * @return long
     */
    [[nodiscard]] long aggregate_counters (int type);

public:
    /**
     * Statistics default constructor.
     */
    Statistics ();

    /**
     * Statistics parameterized constructor.
     * @param operation_type
     */
    Statistics (const std::string& identifier, const OperationType& operation_type);

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
     * get_statistic_entry: copy a given StatisticObject of the m_statistics_entries container.
     * This method is thread-safe.
     * @param operation Defines the operation entry to be retrieved.
     * @return Returns a copy of the respective StatisticEntry object.
     */
    [[nodiscard]] StatisticEntry get_statistic_entry (const int& operation);

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
     * update_bypassed_statistic_entry: updatethe bypassed operation counter of a specific
     * StatisticEntry of the m_statistics_entries container.
     * @param operation_type Defines the operation entry to be registered.
     * @param bypassed_value Defines the value to be incremented in the operation counter.
     */
    void update_bypassed_statistic_entry (const int& operation_type,
        const uint64_t& bypassed_value);

    /**
     * get_stats_identifier_call: get the identifier of the Statistics object.
     * @return Returns a copy of the m_stats_identifier parameter.
     */
    [[nodiscard]] std::string get_stats_identifier () const;

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
    [[maybe_unused]] std::string to_string (const bool& print_header);

    /**
     * tabulate:
     */
    void tabulate ();
};
} // namespace padll::stats

#endif // PADLL_STATISTICS_H
