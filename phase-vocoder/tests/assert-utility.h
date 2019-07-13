#ifndef ASSERTUTILITY_H
#define ASSERTUTILITY_H

#include <gtest/gtest.h>
#include <complex>
#include <vector>

template<typename T>
bool normDifferenceBelowTolerance(std::complex<T> a, std::complex<T> b, T e) {
	return std::norm(a - b) < e;
}

template<typename T>
void assertEqual(std::complex<T> a, std::complex<T> b, T e) {
	EXPECT_PRED3(normDifferenceBelowTolerance<T>, a, b, e);
}

template<typename T>
void assertEqual(
	const std::vector<std::complex<T>> &expected,
	const std::vector<std::complex<T>> &actual,
	T tolerance
) {
	EXPECT_EQ(expected.size(), actual.size());
	for (std::size_t i = 0; i < expected.size(); ++i)
		assertEqual(expected.at(i), actual.at(i), tolerance);
}

template<typename T>
void assertEqual(const std::vector<T> &expected, const std::vector<T> &actual) {
	EXPECT_EQ(expected.size(), actual.size());
	for (std::size_t i = 0; i < expected.size(); ++i)
		EXPECT_EQ(expected.at(i), actual.at(i));
}

#endif