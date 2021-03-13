/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <ldpaio/statistics/statistics.hpp>
#include <numeric>

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

        case OperationType::file_mode_calls: {
            // assign size of statistics container
            this->m_stats_size = FileModes::_size ();
            // retrieves all FileModes operations' names in order
            for (FileModes elem : FileModes::_values ()) {
                this->m_statistic_entries.emplace_back (elem._to_string ());
            }

            break;
        }

        default:
            break;
    }
}

// get_stats_identifier call. (...)
std::string Statistics::get_stats_identifier () const
{
    return this->m_stats_identifier;
}

// get_statistic_entry call. (...)
StatisticEntry Statistics::get_statistic_entry (const int& operation)
{
    std::unique_lock<std::mutex> unique_lock (this->m_stats_mutex);
    // calculate the operation's position in the statistics container
    int position = operation % this->m_stats_size;

    return StatisticEntry { this->m_statistic_entries[position] };
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

// update_statistics_entry call. (...)
void Statistics::update_statistic_entry (const int& operation_type,
    const uint64_t& operation_value,
    const uint64_t& byte_value,
    const uint64_t& error_value)
{
    // calculate the operation's position in the statistics container
    int position = operation_type % this->m_stats_size;

    // update operation, byte, and error counters
    this->m_statistic_entries[position].increment_operation_counter (operation_value);
    this->m_statistic_entries[position].increment_byte_counter (byte_value);
    this->m_statistic_entries[position].increment_error_counter (error_value);
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

    this->tabulate_results ();

    return stream.str ();
}

// aggregate_counters call. (...)
long Statistics::aggregate_counters (int counter_type)
{
    long sum_value = 0;
    switch (counter_type) {
        // aggregate operation counter
        case 0:
            for (auto& element : this->m_statistic_entries) {
                sum_value += element.get_operation_counter ();
            }
            break;

        // aggregate byte counter
        case 1:
            for (auto& element : this->m_statistic_entries) {
                sum_value += element.get_byte_counter ();
            }
            break;

        // aggregate error counter
        case 2:
            for (auto& element : this->m_statistic_entries) {
                sum_value += element.get_error_counter ();
            }
            break;

        default:
            return 0;
    }

    return sum_value;
}

// tabulate_results call.
void Statistics::tabulate_results ()
{
    int rows = 0;
    int columns = 4;
    Table table_stats;

    // add table header
    table_stats.add_row ({ this->m_stats_identifier, "#iops", "#bytes", "#errors" });
    rows++;

    // format table title
    table_stats[0][0]
        .format ()
        .font_color (Color::yellow)
        .font_align (FontAlign::left)
        .font_style ({ FontStyle::bold })
        .width (20);

    // add statistic entries
    for (auto& elem : this->m_statistic_entries) {
        table_stats.add_row ({
            elem.get_entry_name (),
            std::to_string (elem.get_operation_counter ()),
            std::to_string (elem.get_byte_counter ()),
            std::to_string (elem.get_error_counter ()),
        });
        rows++;
    }

    // cumulative statistics entry
    table_stats.add_row ({ "total",
        std::to_string (this->aggregate_counters (0)),
        std::to_string (this->aggregate_counters (1)),
        std::to_string (this->aggregate_counters (2)) });
    rows++;

    // format header cells
    for (int i = 1; i < columns; i++) {
        table_stats[0][i]
            .format ()
            .font_color (Color::yellow)
            .font_align (FontAlign::center)
            .font_style ({ FontStyle::bold })
            .width (10);
    }

    // format columns alignment
    for (int i = 1; i < columns; i++) {
        table_stats.column (i).format ().font_align (FontAlign::center);
    }

    // format first column elements
    for (int i = 0; i < rows; i++) {
        table_stats[i][0].format ().font_style ({ FontStyle::bold });
    }

    // format cumulative statistic entry
    table_stats[rows - 1][0].format ().font_style ({ FontStyle::bold, FontStyle::underline });
    table_stats[rows - 1][1].format ().font_color (Color::green).font_style ({ FontStyle::bold });
    table_stats[rows - 1][2].format ().font_color (Color::yellow).font_style ({ FontStyle::bold });
    table_stats[rows - 1][3].format ().font_color (Color::red).font_style ({ FontStyle::bold });

    // format statistic entries - underline values greater 0
    for (int i = 1; i < rows; i++) {
        for (int j = 1; j < columns; j++) {
            if (std::stol(table_stats[i][j].get_text()) > 0) {
                table_stats[i][j].format().font_style ({ FontStyle::underline });
            }
        }
    }

    // print to stdout
    std::cout << table_stats << std::endl;
}

} // namespace ldpaio