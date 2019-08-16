#include <gsl/gsl>
#include <vector>

namespace phase_vocoder {
template<typename T>
class Expand {
    int P;
public:
    explicit Expand(int P) : P{P} {}

    void expand(gsl::span<const T> x, gsl::span<T> y) {
        std::fill(y.begin(), y.end(), 0);
        for (typename gsl::span<T>::index_type i{0}; i < x.size(); ++i)
            y.at(i*P) = x.at(i);
    }
};
}

#include "assert-utility.h"
#include <gtest/gtest.h>

namespace {
class ExpandTests : public ::testing::Test {
protected:
    void assertExpanded(
        const std::vector<double> &x,
        int P,
        const std::vector<double> &y
    ) {
        phase_vocoder::Expand<double> expand{P};
        std::vector<double> expanded(x.size() * P, 1);
        expand.expand(x, expanded);
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

TEST_F(ExpandTests, insertsPMinusOneZeros2) {
    assertExpanded(
        { 1, 2, 3, 4, 5 },
        1,
        { 1, 2, 3, 4, 5 }
    );
}
}