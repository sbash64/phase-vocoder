#include <gsl/gsl>
#include <vector>

template<typename T>
class FirFilter {
	std::vector<T> b;
public:
	FirFilter(std::vector<T> b) : b{ std::move(b) } {}

	void filter(gsl::span<T> x) {
		for (auto i{ x.size() - 1 }; i >= 0; --i) {
			T accumulate{ 0 };
			for (auto j{ 0 }; j <= b.size() - 1; ++j) {
				auto delayed = i - j < 0
					? 0
					: x[i - j];
				accumulate += b[j] * delayed;
			}
			x[i] = accumulate;
		}
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

	TEST_F(FirFilterTests, simpleConvolution) {
		setCoefficients({ 1, 2, 3 });
		assertFilteredOutput({ 4, 5, 6 }, { 1*4, 1*5. + 2*4., 1*6. + 2*5. + 3*4. });
	}
}