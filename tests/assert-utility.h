#ifndef PHASE_VOCODER_TESTS_ASSERT_UTILITY_H_
#define PHASE_VOCODER_TESTS_ASSERT_UTILITY_H_

#include <phase-vocoder/model.hpp>
#include <gtest/gtest.h>
#include <complex>
#include <vector>

namespace phase_vocoder::test {
template <typename T> void assertEqual(T expected, T actual) {
    EXPECT_EQ(expected, actual);
}

template <typename T>
auto normDifferenceBelowTolerance(complex_type<T> a, complex_type<T> b, T e)
    -> bool {
    return std::norm(a - b) < e;
}

template <typename T>
void assertEqual(complex_type<T> a, complex_type<T> b, T e) {
    EXPECT_PRED3(normDifferenceBelowTolerance<T>, a, b, e);
}

template <typename T> auto size(const std::vector<T> &x) { return x.size(); }

template <typename T> auto size(const const_signal_type<T> &x) {
    return x.size();
}

template <typename T> auto size(const signal_type<T> &x) { return x.size(); }

template <typename T> auto at(const std::vector<T> &x, std::size_t n) {
    return x.at(n);
}

template <typename T>
void assertEqual(const std::vector<complex_type<T>> &expected,
    const std::vector<complex_type<T>> &actual, T tolerance) {
    assertEqual(size(expected), size(actual));
    for (std::size_t i = 0; i < size(expected); ++i)
        assertEqual(at(expected, i), at(actual, i), tolerance);
}

template <typename T>
void assertEqual(
    const std::vector<T> &expected, const std::vector<T> &actual, T tolerance) {
    assertEqual(size(expected), size(actual));
    for (std::size_t i = 0; i < size(expected); ++i)
        EXPECT_NEAR(at(expected, i), at(actual, i), tolerance);
}

template <typename T>
void assertEqual(const std::vector<T> &expected, const std::vector<T> &actual) {
    assertEqual(size(expected), size(actual));
    for (std::size_t i = 0; i < size(expected); ++i)
        assertEqual(at(expected, i), at(actual, i));
}

template <typename T> constexpr auto sizeNarrow(signal_index_type<T> x) {
    return gsl::narrow_cast<typename std::vector<T>::size_type>(x);
}

template <typename T>
void assertEqual(const std::vector<T> &expected, const_signal_type<T> actual) {
    assertEqual(size(expected), sizeNarrow<T>(size(actual)));
    for (signal_index_type<T> i{0}; sizeNarrow<T>(i) < size(expected); ++i)
        assertEqual(at(expected, sizeNarrow<T>(i)), at(actual, i));
}

template <typename T>
void assertEqual(const std::vector<T> &expected, signal_type<T> actual) {
    assertEqual(size(expected), sizeNarrow<T>(size(actual)));
    for (signal_index_type<T> i{0}; sizeNarrow<T>(i) < size(expected); ++i)
        assertEqual(at(expected, sizeNarrow<T>(i)), at(actual, i));
}
}

#endif
