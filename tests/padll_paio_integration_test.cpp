#include <paio/stage/paio_stage.hpp>

namespace padll::tests {

class StageIntegrationTest {

public:
    StageIntegrationTest () = default;

    ~StageIntegrationTest () = default;

    void initialize () {
        std::this_thread::sleep_for (std::chrono::seconds (3));
        std::cout << "\n\n-------------------------------------------------------\n";
        paio::PaioStage stage {};
        std::cout << stage.stage_info_to_string () << std::endl;
        std::cout << "\n-------------------------------------------------------\n\n";
    }
};
}

using namespace padll::tests;

int main () {

    StageIntegrationTest test {};

    test.initialize ();

    return 0;
}