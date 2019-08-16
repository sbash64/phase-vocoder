#include "assert-utility.h"
#include <phase-vocoder/Decimate.h>
#include <gtest/gtest.h>

namespace {
class DecimateTests : public ::testing::Test {
protected:
    void assertDecimated(
        const std::vector<double> &x,
        int Q,
        const std::vector<double> &y
    ) {
        phase_vocoder::Decimate decimate{Q};
        std::vector<double> decimated(x.size()/Q);
        decimate.decimate<double>(x, decimated);
        assertEqual(y, decimated);
    }
};

TEST_F(DecimateTests, extractsEveryQthElement) {
    assertDecimated(
        { 1, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 5, 0, 0 },
        3,
        { 1, 2, 3, 4, 5 }
    );
}
}