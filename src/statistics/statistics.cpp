/**
*   Written by Ricardo Macedo.
*   Copyright (c) 2021 INESC TEC.
**/

#include <ldpaio/statistics/statistics.hpp>

namespace ldpaio {

Statistics::Statistics () = default;

Statistics::~Statistics() = default;

void Statistics::initialize (const OperationType& operation_type)
{
    std::unique_lock<std::mutex> lock (this->m_stats_mutex);

    switch (operation_type) {
        case OperationType::metadata_calls:
            this->m_stats_size = metadata_calls_size;
            break;

        case OperationType::data_calls:
            this->m_stats_size = data_calls_size;
            break;

        case OperationType::directory_calls:
            this->m_stats_size = directory_calls_size;
            break;

        case OperationType::ext_attr_calls:
            this->m_stats_size = ext_attr_calls_size;
            break;

        default:
            break;
    }

    for (int i = 0; i < this->m_stats_size; i++) {
        std::string operation_name { static_cast<char>(i) };
        this->m_statistic_entries.emplace_back (operation_name);
    }
}

void Statistics::update_statistic_entry (const int& operation_type,
    const uint64_t& operation_value,
    const uint64_t& byte_value)
{
    // calculate the operation's position in the statistics container
    int position = operation_type % this->m_stats_size;

    // update operation and byte counters
    this->m_statistic_entries[position].increment_operation_counter (operation_value);
    this->m_statistic_entries[position].increment_byte_counter (byte_value);
}

}