/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2023 INESC TEC.
 **/

#include <padll/statistics/statistics.hpp>

namespace padll::stats {

// Statistics default constructor.
Statistics::Statistics () = default;

// Statistics parameterized constructor.
Statistics::Statistics (const std::string& identifier, const OperationType& operation_type) :
    m_stats_identifier { identifier }
{
    this->initialize (operation_type);
}

// Statistics default destructor.
Statistics::~Statistics () = default;

// initialize call. (...)
void Statistics::initialize (const OperationType& operation_type)
{
    // unique_lock over mutex
    std::unique_lock lock (this->m_stats_mutex);

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

        case OperationType::special_calls: {
            // assign size of statistics container
            this->m_stats_size = Special::_size ();
            // retrieves all ExtendedAttributes operations' names in order
            for (Special elem : Special::_values ()) {
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
    // unique_lock over mutex
    std::unique_lock lock (this->m_stats_mutex);
    // calculate the operation's position in the statistics container
    int position = operation % this->m_stats_size;

    return StatisticEntry { this->m_statistic_entries[position] };
}

// update_statistics_entry call. (...)
void Statistics::update_statistic_entry (const int& operation_type,
    const uint64_t& operation_value,
    const uint64_t& byte_value)
{
    // calculate the operation's position (index) in the statistics container
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
    // calculate the operation's position (index) in the statistics container
    int position = operation_type % this->m_stats_size;

    // update operation, byte, and error counters
    this->m_statistic_entries[position].increment_operation_counter (operation_value);
    this->m_statistic_entries[position].increment_byte_counter (byte_value);
    this->m_statistic_entries[position].increment_error_counter (error_value);
}

// update_bypass_statistic_entry call. (...)
void Statistics::update_bypassed_statistic_entry (const int& operation_type,
    const uint64_t& bypass_value)
{
    // calculate the operation's position (index) in the statistics container
    int position = operation_type % this->m_stats_size;

    // update bypass counters
    this->m_statistic_entries[position].increment_bypass_counter (bypass_value);
}

// get_stats_size call. (...)
int Statistics::get_stats_size () const
{
    return this->m_stats_size;
}

// to_string call. (...)
std::string Statistics::to_string (const bool& print_header)
{
    std::stringstream stream;
    std::vector<StatisticEntry> entries {};

    for (auto& elem : this->m_statistic_entries) {
        if ((elem.get_operation_counter () + elem.get_error_counter () + elem.get_bypass_counter ())
            > 0) {
            entries.push_back (elem);
        }
    }

    if (!entries.empty ()) {
        // TODO: use fmtlib/fmt for easier and faster formatting
        char header[75];
        if (print_header) {
            std::sprintf (header,
                "%15s %12s %12s %12s %15s",
                "syscall",
                "calls",
                "errors",
                "bypassed",
                "bytes");
            stream << header << "\n";
        }

        std::sprintf (header,
            "%15s %12s %12s %12s %15s",
            "-------",
            "-----",
            "------",
            "--------",
            "-----");
        stream << header << "\n";

        for (auto& elem : entries) {
            stream << elem.to_string () << "\n";
        }
    }

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

        // aggregate bypass counter
        case 3:
            for (auto& element : this->m_statistic_entries) {
                sum_value += element.get_bypass_counter ();
            }
            break;

        default:
            return 0;
    }

    return sum_value;
}

// tabulate call.
void Statistics::tabulate ()
{
    int rows = 0;
    int columns = 5;
    Table table_stats;

    // add table header
    table_stats.add_row ({ this->m_stats_identifier, "#iops", "#bytes", "#errors", "#bypass" });
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
            std::to_string (elem.get_bypass_counter ()),
        });
        rows++;
    }

    // cumulative statistics entry
    table_stats.add_row ({ "total",
        std::to_string (this->aggregate_counters (0)),
        std::to_string (this->aggregate_counters (1)),
        std::to_string (this->aggregate_counters (2)),
        std::to_string (this->aggregate_counters (3)) });
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
    table_stats[rows - 1][4].format ().font_color (Color::blue).font_style ({ FontStyle::bold });

    // format statistic entries - underline values greater 0
    for (int i = 1; i < (rows - 1); i++) {
        for (int j = 1; j < columns; j++) {
            if (std::stol (table_stats[i][j].get_text ()) > 0) {
                table_stats[i][j].format ().font_style ({ FontStyle::bold, FontStyle::underline });
            }
        }
    }

    // print to stdout
    std::cout << table_stats << "\n" << std::endl;
}

} // namespace padll::stats
