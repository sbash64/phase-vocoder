#include "assert-utility.h"
#include <phase-vocoder/OverlapAdd.hpp>
#include <gtest/gtest.h>

namespace phase_vocoder::test { namespace {
constexpr auto N = 5;
constexpr auto hop = 2;
class OverlapAddTests : public ::testing::Test {
    OverlapAdd<double> overlapAdd{N};
    std::vector<double> overlap_;
protected:
    OverlapAddTests() : overlap_(hop) {}

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
    assertOverlap({ 1, 2, 3, 4, 5 }, { 1, 2 });
}

TEST_F(OverlapAddTests, nextBlockOverlapAddedToPrevious) {
    consumeAdd({ 1, 2, 3, 4, 5 });
    assertOverlap({ 6, 7, 8, 9, 10 }, { 3+6, 4+7 });
    assertOverlap({ 0, 0, 0, 0, 0 }, { 5+8, 9 });
    assertOverlap({ 0, 0, 0, 0, 0 }, { 10, 0 });
}
}}
