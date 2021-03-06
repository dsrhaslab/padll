/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#ifndef LDPAIO_STATISTICS_H
#define LDPAIO_STATISTICS_H

#include <vector>
#include <ldpaio/statistics/statistic_entry.hpp>
#include <ldpaio/utils/operation_utils.hpp>

namespace ldpaio {

class Statistics {

private:
    int m_stats_size { 0 };
    std::vector<StatisticEntry> m_statistic_entries {};
    std::mutex m_stats_mutex;

public:

    Statistics ();

    ~Statistics ();

    void initialize (const OperationType& operation_type);

    void update_statistic_entry (const int& operation_type,
        const uint64_t& operation_value,
        const uint64_t& byte_value);

};
}

#endif //LDPAIO_STATISTICS_H
