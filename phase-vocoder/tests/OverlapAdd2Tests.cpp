#include <gsl/gsl>
#include <vector>

namespace phase_vocoder {
template<typename T>
class OverlapAdd2 {
    std::vector<T> overlap_;
    std::vector<T> next_;
    int hop;
public:
    OverlapAdd2(int N, int hop) : overlap_(N), next_(N), hop{hop} {}

    void add(gsl::span<const T> x) {
        std::copy(x.begin(), x.end(), next_.begin());
        auto begin_ = overlap_.begin() + hop;
        std::transform(
            begin_,
            overlap_.end(),
            x.begin(),
            begin_,
            std::plus<>{}
        );
    }

    void overlap(gsl::span<T> y) {
        std::copy(overlap_.begin(), overlap_.end(), y.begin());
    }
};
}

#include "assert-utility.h"
#include <gtest/gtest.h>

namespace {
constexpr auto N = 5;
constexpr auto hop = 2;
class OverlapAdd2Tests : public ::testing::Test {
protected:
    void assertOverlap(
        const std::vector<double> &x,
        const std::vector<double> &y
    ) {
        phase_vocoder::OverlapAdd2<double> overlapAdd(N, hop);
        overlapAdd.add(x);
        std::vector<double> overlap_(N);
        overlapAdd.overlap(overlap_);
        assertEqual(y, overlap_);
    }
};

TEST_F(OverlapAdd2Tests, firstBlockAddedToZeros) {
    assertOverlap({ 1, 2, 3, 4, 5 }, { 0, 0, 1, 2, 3 });
}
}
