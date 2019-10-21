#include "assert-utility.h"
#include <phase-vocoder/HannWindow.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <cmath>

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

std::vector<double> fiveSamples(
    double a,
    double b,
    double c,
    double d,
    double e
) {
    return {a, b, c, d, e};
}

double pi() {
    return std::acos(-1.);
}

double sinSquared(double x) {
    return std::sin(x) * std::sin(x);
}

class HannWindowTests : public ::testing::Test {};

#define ASSERT_YIELDS_SIX_WINDOW_SAMPLES(a, b, c, d, e, f, g)\
    assertEqual(sixSamples(b, c, d, e, f, g), hannWindow<double>(a), 1e-15)

#define ASSERT_YIELDS_FIVE_WINDOW_SAMPLES(a, b, c, d, e, f)\
    assertEqual(fiveSamples(b, c, d, e, f), hannWindow<double>(a), 1e-15)

TEST_F(HannWindowTests, even) {
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

TEST_F(HannWindowTests, odd) {
    ASSERT_YIELDS_FIVE_WINDOW_SAMPLES(
        4,
        sinSquared(pi() * 0. / 4.),
        sinSquared(pi() * 1. / 4.),
        sinSquared(pi() * 2. / 4.),
        sinSquared(pi() * 3. / 4.),
        sinSquared(pi() * 4. / 4.)
    );
}
}}
