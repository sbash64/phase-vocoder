#include "assert-utility.h"
#include <phase-vocoder/FirFilter.h>
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

		void assertFilteredOutputs(
			std::vector<std::vector<double>> x,
			const std::vector<std::vector<double>>& y
		) {
			FirFilter<double> filter{ b };
			for (size_t i{ 0 }; i < y.size(); ++i) {
				filter.filter(x[i]);
				assertEqual(y[i], x[i]);
			}
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

	TEST_F(FirFilterTests, simpleConvolutionInSteps) {
		setCoefficients({ 1, 2, 3 });
		assertFilteredOutputs(
			{ 
				{4}, 
				{5}, 
				{6}
			},
			{
				{ 1*4 }, 
				{ 1*5. + 2*4. }, 
				{ 1*6. + 2*5. + 3*4. }
			}
		);
	}

	TEST_F(FirFilterTests, changingSteps) {
		setCoefficients({ 1, 2, 3, 4 });
		assertFilteredOutputs(
			{ 
				{10}, 
				{11, 12}, 
				{13, 14, 15}
			},
			{
				{ 1*10 }, 
				{ 
					1*11. + 2*10., 
					1*12. + 2*11. + 3*10. 
				}, 
				{ 
					1*13. + 2*12. + 3*11. + 4*10.,  
					1*14. + 2*13. + 3*12. + 4*11., 
					1*15. + 2*14. + 3*13. + 4*12.
				}
			}
		);
	}
}