#include "assert-utility.h"
#include <phase-vocoder/OverlapAdd.h>
#include <gtest/gtest.h>

namespace {
constexpr auto N = 5;
constexpr auto hop = 2;
class OverlapAddTests : public ::testing::Test {
    phase_vocoder::OverlapAdd<double> overlapAdd{N, hop};
    std::vector<double> overlap_;
protected:
    OverlapAddTests() : overlap_(N) {}

    void assertOverlap(
        const std::vector<double> &x,
        const std::vector<double> &y
    ) {
        consumeAdd_(x);
        assertEqual(y, overlap_);
    }

    void consumeAdd(const std::vector<double> &x) {
        consumeAdd_(x);
    }

    void consumeAdd_(const std::vector<double> &x) {
        overlapAdd.add(x);
        overlapAdd.next(overlap_);
    }
};

TEST_F(OverlapAddTests, firstBlockAddedToZeros) {
    assertOverlap({ 1, 2, 3, 4, 5 }, { 0, 0, 1, 2, 3 });
}

TEST_F(OverlapAddTests, nextBlockOverlapAddedToPrevious) {
    consumeAdd({ 1, 2, 3, 4, 5 });
    assertOverlap({ 6, 7, 8, 9, 10 }, { 1, 2, 3+6, 4+7, 5+8 });
}
}
