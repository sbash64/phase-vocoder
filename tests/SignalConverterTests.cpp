#include "assert-utility.h"
#include <phase-vocoder/SignalConverter.h>
#include <gtest/gtest.h>

namespace phase_vocoder::test { namespace {
class SignalConverterTests : public ::testing::Test {
protected:
    void assertExpanded(
        const std::vector<double> &x,
        int P,
        const std::vector<double> &y
    ) {
        std::vector<double> expanded(x.size() * gsl::narrow_cast<size_t>(P), 1);
        converter.expand<double>(x, expanded);
        assertEqual(y, expanded);
    }

    void assertDecimated(
        const std::vector<double> &x,
        int Q,
        const std::vector<double> &y
    ) {
        std::vector<double> decimated(x.size()/gsl::narrow_cast<size_t>(Q));
        converter.decimate<double>(x, decimated);
        assertEqual(y, decimated);
    }
private:
    SignalConverter converter;
};

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
