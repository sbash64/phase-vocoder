#include <phase-vocoder/utility.h>
#include <vector>
#include <algorithm>
#include <cmath>

namespace phase_vocoder {
template<typename T>
T pi() {
    return std::acos(T{-1});
}

template<typename T>
std::vector<T> hannWindow(int N) {
    std::vector<T> window(sizeNarrow<T>(N+1));
    std::generate(
        window.begin(),
        window.end(),
        [=, n = 0]() mutable {
            return T{0.5} * (1 - std::cos(2 * pi<T>()*n++/N));
    });
    return window;
}
}

#include "assert-utility.h"
#include <gtest/gtest.h>

namespace phase_vocoder::test { namespace {
std::vector<double> sixSamples(
    double a,
    double b,
    double c,
    double d,
    double e,
    double f
) {
    return {a, b, c, d, e, f};
}

double pi() {
    return std::acos(-1.);
}

double sinSquared(double x) {
    return std::sin(x) * std::sin(x);
}

class HannWindowTests : public ::testing::Test {

};

#define ASSERT_YIELDS_SIX_WINDOW_SAMPLES(a, b, c, d, e, f, g)\
    assertEqual(sixSamples(b, c, d, e, f, g), hannWindow<double>(a), 1e-15)

TEST_F(HannWindowTests, tbd) {
    ASSERT_YIELDS_SIX_WINDOW_SAMPLES(
        5,
        sinSquared(pi() * 0. / 5.),
        sinSquared(pi() * 1. / 5.),
        sinSquared(pi() * 2. / 5.),
        sinSquared(pi() * 3. / 5.),
        sinSquared(pi() * 4. / 5.),
        sinSquared(pi() * 5. / 5.)
    );
}
}}
