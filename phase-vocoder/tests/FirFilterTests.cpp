#include <gsl/gsl>
#include <vector>

template<typename T>
class FirFilter {
	std::vector<T> b;
public:
	FirFilter(std::vector<T> b) : b{ std::move(b) } {}

	void filter(gsl::span<T> x) {
		for (auto &x_ : x)
			x_ *= b.front();
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

	TEST_F(FirFilterTests, doubleYieldsDouble) {
		setCoefficients({ 2 });
		assertFilteredOutput({ 3, 4, 5 }, { 6, 8, 10 });
	}
}