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
		std::vector<double> b{ 0 };

		void assertFilteredOutput(
			std::vector<double> x, 
			const std::vector<double>& y
		) {
			FirFilter<double> filter{ b };
			filter.filter(x);
			assertEqual(y, x);
		}

		void setCoefficients(std::vector<double> b_) {
			b = std::move(b_);
		}
	};

	TEST_F(FirFilterTests, emptyInputYieldsEmpty) {
		assertFilteredOutput({}, {});
	}

	TEST_F(FirFilterTests, identityYieldsSame) {
		setCoefficients({ 1 });
		assertFilteredOutput({2, 3, 4}, {2, 3, 4});
	}
}