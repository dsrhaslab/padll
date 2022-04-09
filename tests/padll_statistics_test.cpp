/**
 *   Written by Ricardo Macedo.
 *   Copyright (c) 2021-2022 INESC TEC.
 **/

#include <padll/statistics/statistics.hpp>
#include <random>

using namespace padll::headers;
using namespace padll::stats;

namespace padll::tests {

/**
 * StatisticsTest class.
 * Complete me ...
 */
class StatisticsTest {

private:
    FILE* m_fd { stdout };

public:
    /**
     * StatisticsTest default constructor.
     */
    StatisticsTest () = default;

    /**
     * StatisticTest parameterized constructor
     */
    explicit StatisticsTest (FILE* fd) : m_fd { fd } {};

    /**
     * test_statistics_constructors:
     */
    void test_statistics_constructors ()
    {
        Statistics stats {};
        std::fprintf (this->m_fd, "%s\n", stats.to_string ().c_str ());

        Statistics stats_param { "stats-parameterized-test", OperationType::metadata_calls };
        std::fprintf (this->m_fd, "%s\n", stats_param.to_string ().c_str ());
    }

    /**
     * test_initialize_statistics:
     * @param stats
     * @param type
     */
    void test_initialize_statistics (Statistics* stats, OperationType type)
    {
        std::fprintf (this->m_fd, "----------------------------------------------\n");
        std::fprintf (this->m_fd, "StatisticsTest (test_initialize_statistics)\n");
        std::fprintf (this->m_fd, "----------------------------------------------\n");

        // initialize statistics
        stats->initialize (type);
        std::fprintf (this->m_fd, "%s\n", stats->to_string ().c_str ());
    }

    /**
     * test_get_statistic_entry:
     * @param stats
     * @param iterations
     */
    void test_get_statistic_entry (Statistics* stats, int iterations)
    {
        std::fprintf (this->m_fd, "----------------------------------------------\n");
        std::fprintf (this->m_fd, "StatisticsTest (test_get_statistics_entry)\n");
        std::fprintf (this->m_fd, "----------------------------------------------\n");

        // load the Statistics object
        this->test_update_statistic_entry (stats, iterations, false);

        for (int i = 0; i < (iterations / 10); i++) {
            int operation = static_cast<int> (random ()) % stats->get_stats_size ();
            auto entry = stats->get_statistic_entry (operation);
            std::cout << entry.to_string () << "\n";
        }
    }

    /**
     * test_update_statistic_entry:
     * @param stats
     * @param iterations
     * @param debug
     */
    void test_update_statistic_entry (Statistics* stats, int iterations, bool debug)
    {
        if (debug) {
            std::fprintf (this->m_fd, "----------------------------------------------\n");
            std::fprintf (this->m_fd, "StatisticsTest (test_update_statistic_entry)\n");
            std::fprintf (this->m_fd, "----------------------------------------------\n");
        }

        for (int i = 0; i < iterations; i++) {
            int operation = static_cast<int> (random ()) % stats->get_stats_size ();
            stats->update_statistic_entry (operation, 1, 1);
        }

        if (debug) {
            std::cout << stats->to_string () << "\n";
        }
    }
};
} // namespace padll::tests

namespace tests = padll::tests;

int main ()
{
    tests::StatisticsTest test { stdout };
    Statistics stats_obj {};
    bool debug { true };

    test.test_statistics_constructors ();
    test.test_initialize_statistics (&stats_obj, OperationType::metadata_calls);
    test.test_update_statistic_entry (&stats_obj, 1000, debug);
    test.test_get_statistic_entry (&stats_obj, 1000);
}