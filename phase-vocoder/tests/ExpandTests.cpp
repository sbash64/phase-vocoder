#include <gsl/gsl>
#include <vector>

namespace phase_vocoder {
template<typename T>
class Expand {
    int P;
public:
    Expand(int, int P) : P{P} {}

    void expand(gsl::span<const T> x, gsl::span<T> y) {
        for (typename gsl::span<T>::index_type i{0}; i < x.size(); ++i)
            y.at(i*P) = x.at(i);
    }
};
}

#include "assert-utility.h"
#include <gtest/gtest.h>

namespace {
constexpr auto N = 5;
class ExpandTests : public ::testing::Test {
protected:
    void assertExpanded(
        std::vector<double> x,
        int P,
        std::vector<double> y
    ) {
        phase_vocoder::Expand<double> expand{N, P};
        std::vector<double> expanded(5 * P);
        expand.expand(x, expanded);
        assertEqual(y, expanded);
    }
};

TEST_F(ExpandTests, tbd) {
    assertExpanded(
        {1, 2, 3, 4, 5},
        3,
        {1, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 5, 0, 0}
    );
}
}