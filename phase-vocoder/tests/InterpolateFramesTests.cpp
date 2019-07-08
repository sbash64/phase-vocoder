#include "assert-utility.h"
#include <phase-vocoder/InterpolateFrames.h>
#include <gtest/gtest.h>

namespace {
	using namespace std::complex_literals;

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

		std::complex<double> doubledPhase(std::complex<double> x) {
			return x * std::exp(phase(x) * 1i);
		}

		std::vector<std::complex<double>> doubledPhase(std::vector<std::complex<double>> x) {
			std::transform(
				x.begin(),
				x.end(),
				x.begin(),
				[&](std::complex<double> a) { return doubledPhase(a); }
			);
			return x;
		}

		double averageMagnitude(std::complex<double> a, std::complex<double> b) {
			return (magnitude(a) + magnitude(b)) / 2;
		}

		double summedPhase(std::complex<double> a, std::complex<double> b) {
			return phase(a) + phase(b);
		}
	};

	TEST_F(InterpolateFramesTests, interpolatesComplexMagnitudesAndAdvancesPhase) {
		assertInterpolatedFrames(
			{1.0 + 2i, 3.0 + 4i, 5.0 + 6i},
			{
				{ 0.5 + 1i, 1.5 + 2i, 2.5 + 3i},
				doubledPhase({1.0 + 2i, 3.0 + 4i, 5.0 + 6i})
			},
			1e-15
		);
	}

	TEST_F(InterpolateFramesTests, interpolatesComplexMagnitudesAndAdvancesPhase2) {
		addConsume({ 1.0 + 2i, 3.0 + 4i, 5.0 + 6i });
		assertInterpolatedFrames(
			{ 7.0 + 8i, 9.0 + 10i, 11.0 + 12i },
			{
				{
					toComplex(averageMagnitude(1.0 + 2i, 7.0 + 8i), summedPhase(7.0 + 8i, 1.0 + 2i)),
					toComplex(averageMagnitude(3.0 + 4i, 9.0 + 10i), summedPhase(9.0 + 10i, 3.0 + 4i)),
					toComplex(averageMagnitude(5.0 + 6i, 11.0 + 12i), summedPhase(11.0 + 12i, 5.0 + 6i)),
				},
				doubledPhase({ 7.0 + 8i, 9.0 + 10i, 11.0 + 12i })
			},
			1e-15
		);
	}
}