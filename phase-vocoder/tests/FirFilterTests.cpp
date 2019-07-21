#include <gsl/gsl>
#include <vector>

template<typename T>
class FirFilter {
public:
	FirFilter(std::vector<T> b) {}

	void filter(gsl::span<T> x) {
		x;
	}
};

#include "assert-utility.h"
#include <gtest/gtest.h>

namespace {
	class FirFilterTests : public ::testing::Test {
	protected:
		void assertFilteredOutput(
			std::vector<double> b, 
			std::vector<double> x, 
			const std::vector<double>& y
		) {
			FirFilter<double> filter{ std::move(b) };
			filter.filter(x);
			assertEqual(y, x);
		}
	};

	TEST_F(FirFilterTests, emptyInputYieldsEmpty) {
		std::vector<double> b = { 1 };
		assertFilteredOutput(b, {}, {});
	}
}