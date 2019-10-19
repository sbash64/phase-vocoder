#include "assert-utility.h"
#include <phase-vocoder/Expand.h>
#include <gtest/gtest.h>

namespace phase_vocoder::test { namespace {
class ExpandTests : public ::testing::Test {
protected:
    void assertExpanded(
        const std::vector<double> &x,
        int P,
        const std::vector<double> &y
    ) {
        Expand expand{P};
        std::vector<double> expanded(x.size() * gsl::narrow_cast<size_t>(P), 1);
        expand.expand<double>(x, expanded);
        assertEqual(y, expanded);
    }
};

TEST_F(ExpandTests, insertsPMinusOneZeros) {
    assertExpanded(
        { 1, 2, 3, 4, 5 },
        3,
        { 1, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 5, 0, 0 }
    );
}

TEST_F(ExpandTests, insertsNoZerosWhenPIsOne) {
    assertExpanded(
        { 1, 2, 3, 4, 5 },
        1,
        { 1, 2, 3, 4, 5 }
    );
}
}}