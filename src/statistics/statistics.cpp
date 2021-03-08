/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <ldpaio/statistics/statistics.hpp>

namespace ldpaio {

// Statistics default constructor.
Statistics::Statistics () = default;

// Statistics parameterized constructor.
Statistics::Statistics (std::string identifier, const OperationType& operation_type) :
    m_stats_identifier { std::move (identifier) }
{
    Logging::log_debug ("Statistics parameterized constructor.");
    this->initialize (operation_type);
}

// Statistics default destructor.
Statistics::~Statistics () = default;

// initialize call. (...)
void Statistics::initialize (const OperationType& operation_type)
{
    std::unique_lock<std::mutex> lock (this->m_stats_mutex);

    switch (operation_type) {
        case OperationType::metadata_calls: {
            // assign size of statistics container
            this->m_stats_size = Metadata::_size ();
            // retrieves all Metadata operations' names in order
            for (Metadata elem : Metadata::_values ()) {
                this->m_statistic_entries.emplace_back (elem._to_string ());
            }

            break;
        }

        case OperationType::data_calls: {
            // assign size of statistics container
            this->m_stats_size = Data::_size ();
            // retrieves all Data operations' names in order
            for (Data elem : Data::_values ()) {
                this->m_statistic_entries.emplace_back (elem._to_string ());
            }

            break;
        }

        case OperationType::directory_calls: {
            // assign size of statistics container
            this->m_stats_size = Directory::_size ();
            // retrieves all Directory operations' names in order
            for (Directory elem : Directory::_values ()) {
                this->m_statistic_entries.emplace_back (elem._to_string ());
            }

            break;
        }

        case OperationType::ext_attr_calls: {
            // assign size of statistics container
            this->m_stats_size = ExtendedAttributes::_size ();
            // retrieves all ExtendedAttributes operations' names in order
            for (ExtendedAttributes elem : ExtendedAttributes::_values ()) {
                this->m_statistic_entries.emplace_back (elem._to_string ());
            }

            break;
        }

        default:
            break;
    }

    Logging::log_debug (this->to_string ());
}

// get_stats_identifier call. (...)
std::string Statistics::get_stats_identifier () const
{
    return this->m_stats_identifier;
}

// update_statistics_entry call. (...)
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

// get_stats_size call. (...)
int Statistics::get_stats_size () const
{
    return this->m_stats_size;
}

// to_string call. (...)
std::string Statistics::to_string ()
{
    std::stringstream stream;
    stream << "Statistics { " << this->m_stats_identifier << ": ";
    for (auto& elem : this->m_statistic_entries) {
        stream << elem.to_string () << ";";
    }
    stream << " }";

    return stream.str ();
}

} // namespace ldpaio