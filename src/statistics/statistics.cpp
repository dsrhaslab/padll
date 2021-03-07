/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <ldpaio/statistics/statistics.hpp>

namespace ldpaio {

Statistics::Statistics () = default;

Statistics::~Statistics () = default;

void Statistics::initialize (const OperationType& operation_type)
{
    std::unique_lock<std::mutex> lock (this->m_stats_mutex);

    switch (operation_type) {
        case OperationType::metadata_calls: {
            // assign size of statistics container
            this->m_stats_size = magic_enum::enum_count<Metadata> ();
            // retrieves all Metadata operations' names in order
            constexpr auto& operation_names = magic_enum::enum_names<Metadata> ();

            // initialize all statistics entries
            for (auto element : operation_names) {
                this->m_statistic_entries.emplace_back (element.data ());
            }

            break;
        }

        case OperationType::data_calls: {
            // assign size of statistics container
            this->m_stats_size = magic_enum::enum_count<Data> ();
            // retrieves all Data operations' names in order
            constexpr auto& operation_names = magic_enum::enum_names<Data> ();

            // initialize all statistics entries
            for (auto element : operation_names) {
                this->m_statistic_entries.emplace_back (element.data ());
            }

            break;
        }

        case OperationType::directory_calls: {
            // assign size of statistics container
            this->m_stats_size = magic_enum::enum_count<Directory> ();
            // retrieves all Directory operations' names in order
            constexpr auto& operation_names = magic_enum::enum_names<Directory> ();

            // initialize all statistics entries
            for (auto element : operation_names) {
                this->m_statistic_entries.emplace_back (element.data ());
            }

            break;
        }

        case OperationType::ext_attr_calls: {
            // assign size of statistics container
            this->m_stats_size = magic_enum::enum_count<ExtendedAttributes> ();
            // retrieves all ExtendedAttributes operations' names in order
            constexpr auto& operation_names = magic_enum::enum_names<ExtendedAttributes> ();

            // initialize all statistics entries
            for (auto element : operation_names) {
                this->m_statistic_entries.emplace_back (element.data ());
            }

            break;
        }

        default:
            break;
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

int Statistics::get_stats_size () const
{
    return this->m_stats_size;
}

std::string Statistics::to_string ()
{
    std::stringstream stream;
    stream << "Statistics { \n";
    for (auto& elem : this->m_statistic_entries) {
        stream << elem.to_string () << ";";
    }
    stream << " }";

    return stream.str ();
}

} // namespace ldpaio