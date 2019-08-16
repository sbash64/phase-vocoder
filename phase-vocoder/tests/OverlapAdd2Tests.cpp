#include <gsl/gsl>
#include <vector>

namespace phase_vocoder {
template<typename T>
class OverlapAdd2 {
    std::vector<T> overlap_;
    std::vector<T> next_;
    int hop;
public:
    OverlapAdd2(int N, int hop) : overlap_(N), next_(hop), hop{hop} {}

    void add(gsl::span<const T> x) {
        std::copy(x.rbegin(), x.rbegin() + hop, next_.rbegin());
        auto begin_ = overlap_.begin() + hop;
        std::transform(
            begin_,
            overlap_.end(),
            x.begin(),
            begin_,
            std::plus<>{}
        );
    }

    void next(gsl::span<T> y) {
        std::copy(overlap_.begin(), overlap_.end(), y.begin());
        for (size_t i{0}; i < overlap_.size() - hop; ++i)
            overlap_.at(i) = overlap_.at(i+hop);
        std::copy(next_.rbegin(), next_.rend(), overlap_.rbegin());
    }
};
}

#include "assert-utility.h"
#include <gtest/gtest.h>

namespace {
constexpr auto N = 5;
constexpr auto hop = 2;
class OverlapAdd2Tests : public ::testing::Test {
    phase_vocoder::OverlapAdd2<double> overlapAdd{N, hop};
    std::vector<double> overlap_;
protected:
    OverlapAdd2Tests() : overlap_(N) {}
    
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

TEST_F(OverlapAdd2Tests, firstBlockAddedToZeros) {
    assertOverlap({ 1, 2, 3, 4, 5 }, { 0, 0, 1, 2, 3 });
}

TEST_F(OverlapAdd2Tests, nextBlockOverlapAddedToPrevious) {
    consumeAdd({ 1, 2, 3, 4, 5 });
    assertOverlap({ 6, 7, 8, 9, 10 }, { 1, 2, 3+6, 4+7, 5+8 });
}
}
