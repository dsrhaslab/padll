/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021 INESC TEC.
 **/

#include <ldpaio/statistics/statistics.hpp>
#include <random>

namespace ldpaio {

class StatisticsTest {

public:
    /**
     * test_statistics_constructors:
     */
    void test_statistics_constructors ()
    {
        Statistics stats {};
        Logging::log_info (stats.to_string ());

        Statistics stats_param { "stats-parameterized-test", OperationType::metadata_calls };
        Logging::log_info (stats_param.to_string ());
    }

    /**
     * test_initialize_statistics:
     * @param stats
     * @param type
     */
    void test_initialize_statistics (Statistics* stats, OperationType type)
    {
        stats->initialize (type);
    }

    /**
     * test_get_statistic_entry:
     * @param stats
     * @param iterations
     */
    void test_get_statistic_entry (Statistics* stats, int iterations)
    {
        // load the Statistics object
        for (int i = 0; i < iterations; i++) {
            int operation = static_cast<int> (random ()) % stats->get_stats_size ();
            stats->update_statistic_entry (operation, 1, 1);
        }

        for (int i = 0; i < (iterations / 10); i++) {
            int operation = static_cast<int> (random ()) % stats->get_stats_size ();
            auto entry = stats->get_statistic_entry (operation);
            std::cout << entry.to_string() << "\n";
        }
    }

    /**
     * test_update_entries:
     * @param stats
     * @param iterations
     */
    void test_update_entries (Statistics* stats, int iterations)
    {
        for (int i = 0; i < iterations; i++) {
            int operation = static_cast<int> (random ()) % stats->get_stats_size ();
            stats->update_statistic_entry (operation, 1, 1);
        }
        std::cout << stats->to_string () << "\n";
    }
};
} // namespace ldpaio

int main (int argc, char** argv)
{
    ldpaio::StatisticsTest test {};
    ldpaio::Statistics stats_obj {};

    test.test_initialize_statistics (&stats_obj, ldpaio::OperationType::metadata_calls);
    test.test_update_entries (&stats_obj, 1000);
    test.test_get_statistic_entry (&stats_obj, 1000);

    test.test_statistics_constructors ();
}