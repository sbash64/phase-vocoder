#include <gsl/gsl>

namespace phase_vocoder {
class Decimate {
    int Q;
public:
    explicit Decimate(int Q) : Q{Q} {}

    template<typename T>
    void decimate(gsl::span<const T> x, gsl::span<T> y) {
        for (typename gsl::span<T>::index_type i{0}; i < y.size(); ++i)
            y.at(i) = x.at(i*Q);
    }
};
}

#include "assert-utility.h"
#include <gtest/gtest.h>

namespace {
class DecimateTests : public ::testing::Test {
protected:
    void assertDecimated(
        const std::vector<double> &x,
        int Q,
        const std::vector<double> &y
    ) {
        phase_vocoder::Decimate decimate{Q};
        std::vector<double> decimated(x.size()/Q);
        decimate.decimate<double>(x, decimated);
        assertEqual(y, decimated);
    }
};

TEST_F(DecimateTests, tbd) {
    assertDecimated(
        { 1, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 5, 0, 0 },
        3,
        { 1, 2, 3, 4, 5 }
    );
}
}