#include "assert-utility.h"
#include <phase-vocoder/Expand.h>
#include <phase-vocoder/Decimate.h>
#include <gtest/gtest.h>

namespace phase_vocoder::test { namespace {
void assertDecimated(
    const std::vector<double> &x,
    int Q,
    const std::vector<double> &y
) {
    Decimate decimate;
    std::vector<double> decimated(x.size()/gsl::narrow_cast<size_t>(Q));
    decimate.decimate<double>(x, decimated, Q);
    assertEqual(y, decimated);
}

void assertExpanded(
    const std::vector<double> &x,
    int P,
    const std::vector<double> &y
) {
    Expand expand;
    std::vector<double> expanded(x.size() * gsl::narrow_cast<size_t>(P), 1);
    expand.expand<double>(x, expanded, P);
    assertEqual(y, expanded);
}

class SignalConverterTests : public ::testing::Test {};

TEST_F(SignalConverterTests, extractsEveryQthElement) {
    assertDecimated(
        { 1, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 5, 0, 0 },
        3,
        { 1, 2, 3, 4, 5 }
    );
}

TEST_F(SignalConverterTests, insertsPMinusOneZeros) {
    assertExpanded(
        { 1, 2, 3, 4, 5 },
        3,
        { 1, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 5, 0, 0 }
    );
}

TEST_F(SignalConverterTests, insertsNoZerosWhenPIsOne) {
    assertExpanded(
        { 1, 2, 3, 4, 5 },
        1,
        { 1, 2, 3, 4, 5 }
    );
}
}}
