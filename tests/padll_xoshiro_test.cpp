#include <chrono>
#include <iostream>
#include <padll/third_party/xoshiro.hpp>
#include <random>

using namespace XoshiroCpp;

namespace padll::tests {

#define OPTION_XOSHIRO_TEST_DEBUG false

class XoshiroTest {

private:
    FILE* m_fd { stdout };
    const uint64_t seed { 12345 };

    void log_results (const std::string_view& generator,
        const long& iterations,
        const std::chrono::duration<double>& elapsed_seconds)
    {
        std::string header {};
        header.append ("| ").append (generator);

        std::fprintf (this->m_fd, "\n-----------------------------------\n");
        std::fprintf (this->m_fd, "%s\n", header.c_str ());
        std::fprintf (this->m_fd, "-----------------------------------\n");
        std::fprintf (this->m_fd,
            "Ops: %ld;\t Duration: %.3f\n",
            iterations,
            elapsed_seconds.count ());
        std::fprintf (this->m_fd,
            "Thr: %.1f KOps/s\n",
            static_cast<double> (iterations) / elapsed_seconds.count () / 1000);
        std::fprintf (this->m_fd,
            "Lat: %.3e\n",
            (elapsed_seconds.count () / static_cast<double> (iterations)));
        std::fprintf (this->m_fd, "-----------------------------------\n");
        std::fflush (this->m_fd);
    }

public:
    XoshiroTest () = default;

    explicit XoshiroTest (FILE* fd) : m_fd { fd } {};

    /**
     * test_cpp_random:
     */
    void test_cpp_random (const uint64_t& iterations, const int& min, const int& max)
    {
        auto start = std::chrono::system_clock::now ();
        for (uint64_t i = 0; i < iterations; i++) {
            [[maybe_unused]] auto value = static_cast<int> ((random () % max) - min);
#if OPTION_XOSHIRO_TEST_DEBUG
            std::cout << "cpp-random (" << i << "): " << value << std::endl;
#endif
        }
        auto end = std::chrono::system_clock::now ();
        std::chrono::duration<double> elapsed_seconds = end - start;

        this->log_results (std::string_view ("cpp-random"), iterations, elapsed_seconds);
    }

    /**
     * test_cpp_mt19937:
     */
    void test_cpp_mt19937 (const uint64_t& iterations, const int& min, const int& max)
    {
        std::mt19937 mt { this->seed };

        auto start = std::chrono::system_clock::now ();
        for (uint64_t i = 0; i < iterations; i++) {
            [[maybe_unused]] auto value = static_cast<int> ((mt () % max) - min);
#if OPTION_XOSHIRO_TEST_DEBUG
            std::cout << "cpp-mt19937 (" << i << "): " << value << std::endl;
#endif
        }
        auto end = std::chrono::system_clock::now ();
        std::chrono::duration<double> elapsed_seconds = end - start;

        this->log_results (std::string_view ("cpp-mt19937"), iterations, elapsed_seconds);
    }

    /**
     * test_xoshiro128plus:
     */
    void test_xoshiro128plus (const uint64_t& iterations, const int& min, const int& max)
    {
        Xoshiro128Plus xoshiro128 { this->seed };

        auto start = std::chrono::system_clock::now ();
        for (uint64_t i = 0; i < iterations; i++) {
            [[maybe_unused]] auto value = static_cast<int> ((xoshiro128 () % max) - min);
#if OPTION_XOSHIRO_TEST_DEBUG
            std::cout << "Xoshiro128+ (" << i << "): " << value << std::endl;
#endif
        }
        auto end = std::chrono::system_clock::now ();
        std::chrono::duration<double> elapsed_seconds = end - start;

        this->log_results (std::string_view ("xoshiro-128+"), iterations, elapsed_seconds);
    }

    /**
     * test_xoshiro128plusplus:
     */
    void test_xoshiro128plusplus (const uint64_t& iterations, const int& min, const int& max)
    {
        Xoshiro128PlusPlus xoshiro128 { this->seed };

        auto start = std::chrono::system_clock::now ();
        for (uint64_t i = 0; i < iterations; i++) {
            [[maybe_unused]] auto value = static_cast<int> ((xoshiro128 () % max) - min);
#if OPTION_XOSHIRO_TEST_DEBUG
            std::cout << "Xoshiro256 (" << i << "): " << value << std::endl;
#endif
        }
        auto end = std::chrono::system_clock::now ();
        std::chrono::duration<double> elapsed_seconds = end - start;

        this->log_results (std::string_view ("xoshiro-128++"), iterations, elapsed_seconds);
    }

    /**
     * test_xoshiro128starstar:
     */
    void test_xoshiro128starstar (const uint64_t& iterations, const int& min, const int& max)
    {
        Xoshiro128StarStar xoshiro128 { this->seed };

        auto start = std::chrono::system_clock::now ();
        for (uint64_t i = 0; i < iterations; i++) {
            [[maybe_unused]] auto value = static_cast<int> ((xoshiro128 () % max) - min);
#if OPTION_XOSHIRO_TEST_DEBUG
            std::cout << "Xoshiro128** (" << i << "): " << value << std::endl;
#endif
        }
        auto end = std::chrono::system_clock::now ();
        std::chrono::duration<double> elapsed_seconds = end - start;

        this->log_results (std::string_view ("xoshiro-128**"), iterations, elapsed_seconds);
    }

    /**
     * test_xoshiro256plus:
     */
    void test_xoshiro256plus (const uint64_t& iterations, const int& min, const int& max)
    {
        Xoshiro256Plus xoshiro256 { this->seed };

        auto start = std::chrono::system_clock::now ();
        for (uint64_t i = 0; i < iterations; i++) {
            [[maybe_unused]] auto value = static_cast<int> ((xoshiro256 () % max) - min);
#if OPTION_XOSHIRO_TEST_DEBUG
            std::cout << "Xoshiro256+ (" << i << "): " << value << std::endl;
#endif
        }
        auto end = std::chrono::system_clock::now ();
        std::chrono::duration<double> elapsed_seconds = end - start;

        this->log_results (std::string_view ("xoshiro-256+"), iterations, elapsed_seconds);
    }

    /**
     * test_xoshiro256plusplus:
     */
    void test_xoshiro256plusplus (const uint64_t& iterations, const int& min, const int& max)
    {
        Xoshiro256PlusPlus xoshiro256 { this->seed };

        auto start = std::chrono::system_clock::now ();
        for (uint64_t i = 0; i < iterations; i++) {
            [[maybe_unused]] auto value = static_cast<int> ((xoshiro256 () % max) - min);
#if OPTION_XOSHIRO_TEST_DEBUG
            std::cout << "Xoshiro256 (" << i << "): " << value << std::endl;
#endif
        }
        auto end = std::chrono::system_clock::now ();
        std::chrono::duration<double> elapsed_seconds = end - start;

        this->log_results (std::string_view ("xoshiro-256++"), iterations, elapsed_seconds);
    }

    /**
     * test_xoshiro256starstar:
     */
    void test_xoshiro256starstar (const uint64_t& iterations, const int& min, const int& max)
    {
        Xoshiro256StarStar xoshiro256 { this->seed };

        auto start = std::chrono::system_clock::now ();
        for (uint64_t i = 0; i < iterations; i++) {
            [[maybe_unused]] auto value = static_cast<int> ((xoshiro256 () % max) - min);
#if OPTION_XOSHIRO_TEST_DEBUG
            std::cout << "Xoshiro256** (" << i << "): " << value << std::endl;
#endif
        }
        auto end = std::chrono::system_clock::now ();
        std::chrono::duration<double> elapsed_seconds = end - start;

        this->log_results (std::string_view ("xoshiro-256**"), iterations, elapsed_seconds);
    }
};
} // namespace padll::tests

int main ()
{
    padll::tests::XoshiroTest xoshiro_test { stdout };
    const uint64_t iterations { 5000000000 };
    const int min { 0 };
    const int max { 100000 };

    xoshiro_test.test_cpp_random (iterations, min, max);
    xoshiro_test.test_cpp_mt19937 (iterations, min, max);
    xoshiro_test.test_xoshiro128plus (iterations, min, max);
    xoshiro_test.test_xoshiro128plusplus (iterations, min, max);
    xoshiro_test.test_xoshiro128starstar (iterations, min, max);
    xoshiro_test.test_xoshiro256plus (iterations, min, max);
    xoshiro_test.test_xoshiro256plusplus (iterations, min, max);
    xoshiro_test.test_xoshiro256starstar (iterations, min, max);
}