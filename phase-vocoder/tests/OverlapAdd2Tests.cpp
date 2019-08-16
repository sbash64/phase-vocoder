#include <gsl/gsl>
#include <vector>

namespace phase_vocoder {
template<typename T>
class OverlapAdd2 {
    std::vector<T> buffer_;
public:
    explicit OverlapAdd2(int N) : buffer_(N) {}
    
    void add(std::vector<T> x) {
        std::copy(x.begin(), x.end(), buffer_.begin());
    }

    void overlap(gsl::span<T> y) {
        std::copy(buffer_.begin(), buffer_.end(), y.begin());
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
    void assertOverlap(std::vector<double> x, const std::vector<double> &y) {
        phase_vocoder::OverlapAdd2<double> overlapAdd(N);
        overlapAdd.add(std::move(x));
        std::vector<double> overlap_(N);
        overlapAdd.overlap(overlap_);
        assertEqual(y, overlap_);
    }
};

TEST_F(OverlapAdd2Tests, tbd) {
    assertOverlap({ 1, 2, 3, 4, 5 }, { 1, 2, 3, 4, 5 });
}
}
