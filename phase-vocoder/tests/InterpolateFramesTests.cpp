#include "assert-utility.h"
#include <phase-vocoder/InterpolateFrames.h>
#include <gtest/gtest.h>

namespace {
	class InterpolateFramesTests : public ::testing::Test {
		int P = 1;
		int Q = 2;
		int N = 3;
		InterpolateFrames<double> interpolate{ P, Q, N };
	protected:
		void assertInterpolatedFrames(
			std::vector<std::complex<double>> x, 
			std::vector<std::vector<std::complex<double>>> frames,
			double tolerance
		) {
			add(std::move(x));
			for (auto frame : frames) {
				assertHasNext();
				assertNextEquals(std::move(frame), tolerance);
			}
			assertDoesNotHaveNext();
		}

		void assertHasNext() {
			EXPECT_TRUE(hasNext());
		}

		void assertDoesNotHaveNext() {
			EXPECT_FALSE(hasNext());
		}

		bool hasNext() {
			return interpolate.hasNext();
		}

		void add(std::vector<std::complex<double>> x) {
			interpolate.add(x);
		}

		void assertNextEquals(std::vector<std::complex<double>> expected, double tolerance) {
			std::vector<std::complex<double>> out(N);
			interpolate.next(out);
			assertEqual(std::move(expected), std::move(out), tolerance);
		}
	};

	TEST_F(InterpolateFramesTests, tbd) {
		using namespace std::complex_literals;
		assertInterpolatedFrames(
			{1.0 + 2i, 3.0 + 4i, 5.0 + 6i},
			{
				{ 0.5 + 1i, 1.5 + 2i, 2.5 + 3i},
				{ 1.0 + 2i, 3.0 + 4i, 5.0 + 6i}
			},
			1e-15
		);
	}
}