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

		void addConsume(std::vector<std::complex<double>> x) {
			add(x);
			while (hasNext())
				next();
		}

		std::vector<std::complex<double>> next() {
			std::vector<std::complex<double>> out(N);
			interpolate.next(out);
			return out;
		}

		void assertNextEquals(std::vector<std::complex<double>> expected, double tolerance) {
			assertEqual(std::move(expected), next(), tolerance);
		}

		double magnitude(std::complex<double> x) {
			return std::abs(x);
		}

		double phase(std::complex<double> x) {
			return std::arg(x);
		}

		std::complex<double> toComplex(double magnitude, double radians) {
			return std::polar(magnitude, radians);
		}
	};

	TEST_F(InterpolateFramesTests, interpolatesComplexMagnitudesAndAdvancesPhase) {
		using namespace std::complex_literals;
		assertInterpolatedFrames(
			{1.0 + 2i, 3.0 + 4i, 5.0 + 6i},
			{
				{ 0.5 + 1i, 1.5 + 2i, 2.5 + 3i},
				{
					toComplex(magnitude(1.0 + 2i), 2 * phase(1.0 + 2i)),
					toComplex(magnitude(3.0 + 4i), 2 * phase(3.0 + 4i)),
					toComplex(magnitude(5.0 + 6i), 2 * phase(5.0 + 6i))
				}
			},
			1e-15
		);
	}

	TEST_F(InterpolateFramesTests, interpolatesComplexMagnitudesAndAdvancesPhase2) {
		using namespace std::complex_literals;
		addConsume(
			{ 1.0 + 2i, 3.0 + 4i, 5.0 + 6i }
		);
		assertInterpolatedFrames(
			{ 7.0 + 8i, 9.0 + 10i, 11.0 + 12i },
			{
				{
					toComplex((magnitude(1.0 + 2i) + magnitude(7.0 + 8i)) / 2, phase(7.0 + 8i) + phase(1.0 + 2i)),
					toComplex((magnitude(3.0 + 4i) + magnitude(9.0 + 10i)) / 2, phase(9.0 + 10i) + phase(3.0 + 4i)),
					toComplex((magnitude(5.0 + 6i) + magnitude(11.0 + 12i)) / 2, phase(11.0 + 12i) + phase(5.0 + 6i)),
				},
				{
					toComplex(magnitude(7.0 + 8i), 2 * phase(7.0 + 8i)),
					toComplex(magnitude(9.0 + 10i), 2 * phase(9.0 + 10i)),
					toComplex(magnitude(11.0 + 12i), 2 * phase(11.0 + 12i))
				}
			},
			1e-15
		);
	}
}