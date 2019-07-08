#include "assert-utility.h"
#include <phase-vocoder/InterpolateFrames.h>
#include <gtest/gtest.h>

namespace {
	using namespace std::complex_literals;

	double magnitude(std::complex<double> x) {
		return std::abs(x);
	}

	double phase(std::complex<double> x) {
		return std::arg(x);
	}

	std::complex<double> complex(double magnitude, double radians) {
		return std::polar(magnitude, radians);
	}

	std::complex<double> doublePhase(std::complex<double> x) {
		return x * std::exp(phase(x) * 1i);
	}

	std::vector<std::complex<double>> doublePhase(std::vector<std::complex<double>> x) {
		std::transform(
			x.begin(),
			x.end(),
			x.begin(),
			[](std::complex<double> a) { return doublePhase(a); }
		);
		return x;
	}

	double averageMagnitude(std::complex<double> a, std::complex<double> b) {
		return (magnitude(a) + magnitude(b)) / 2;
	}

	double summedPhase(std::complex<double> a, std::complex<double> b) {
		return phase(a) + phase(b);
	}

	std::complex<double> averageMagnitudesAndSumPhases(std::complex<double> a, std::complex<double> b) {
		return complex(averageMagnitude(a, b), summedPhase(a, b));
	}

	std::vector<std::complex<double>> averageMagnitudesAndSumPhases(
		std::vector<std::complex<double>> a,
		std::vector<std::complex<double>> b
	) {
		std::vector<std::complex<double>> out;
		std::transform(
			a.begin(),
			a.end(),
			b.begin(),
			std::back_inserter(out),
			[](std::complex<double> a_, std::complex<double> b_) { 
				return averageMagnitudesAndSumPhases(a_, b_); 
			}
		);
		return out;
	}

	std::complex<double> twoThirdsMagnitudeOfFirstOneThirdOfSecondAndPhaseSecond(
		std::complex<double> a, 
		std::complex<double> b
	) {
		return complex(2 * magnitude(a) / 3 + magnitude(b) / 3, phase(b));
	}

	std::vector<std::complex<double>> twoThirdsMagnitudeOfFirstOneThirdOfSecondAndPhaseSecond(
		std::vector<std::complex<double>> a,
		std::vector<std::complex<double>> b
	) {
		std::vector<std::complex<double>> out;
		std::transform(
			a.begin(),
			a.end(),
			b.begin(),
			std::back_inserter(out),
			[](std::complex<double> a_, std::complex<double> b_) {
				return twoThirdsMagnitudeOfFirstOneThirdOfSecondAndPhaseSecond(a_, b_);
			}
		);
		return out;
	}

	class InterpolateFramesFacade {
		InterpolateFrames<double> interpolate;
		int N;
	public:
		InterpolateFramesFacade(int P, int Q, int N) : interpolate{ P, Q, N }, N{ N } {}

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

		void consumeAdd(std::vector<std::complex<double>> x) {
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
	};

	class InterpolateFramesP1Q2Tests : public ::testing::Test {
		int P = 1;
		int Q = 2;
		int N = 3;
		InterpolateFramesFacade interpolate{ P, Q, N };
	protected:
		void assertInterpolatedFrames(
			std::vector<std::complex<double>> x, 
			std::vector<std::vector<std::complex<double>>> frames,
			double tolerance
		) {
			interpolate.assertInterpolatedFrames(x, frames, tolerance);
		}

		void consumeAdd(std::vector<std::complex<double>> x) {
			interpolate.consumeAdd(x);
		}
	};

	TEST_F(InterpolateFramesP1Q2Tests, interpolatesComplexMagnitudesAndAdvancesPhase) {
		assertInterpolatedFrames(
			{1.0 + 2i, 3.0 + 4i, 5.0 + 6i},
			{
				{ 0.5 + 1i, 1.5 + 2i, 2.5 + 3i},
				doublePhase({1.0 + 2i, 3.0 + 4i, 5.0 + 6i})
			},
			1e-15
		);
	}

	TEST_F(InterpolateFramesP1Q2Tests, interpolatesComplexMagnitudesAndAdvancesPhase2) {
		consumeAdd({ 1.0 + 2i, 3.0 + 4i, 5.0 + 6i });
		assertInterpolatedFrames(
			{ 7.0 + 8i, 9.0 + 10i, 11.0 + 12i },
			{
				averageMagnitudesAndSumPhases(
					{ 1.0 + 2i, 3.0 + 4i, 5.0 + 6i }, 
					{ 7.0 + 8i, 9.0 + 10i, 11.0 + 12i }
				),
				doublePhase({ 7.0 + 8i, 9.0 + 10i, 11.0 + 12i })
			},
			1e-15
		);
	}

	class InterpolateFramesP2Q3Tests : public ::testing::Test {
		int P = 2;
		int Q = 3;
		int N = 3;
		InterpolateFramesFacade interpolate{ P, Q, N };
	protected:
		void assertInterpolatedFrames(
			std::vector<std::complex<double>> x,
			std::vector<std::vector<std::complex<double>>> frames,
			double tolerance
		) {
			interpolate.assertInterpolatedFrames(x, frames, tolerance);
		}

		void consumeAdd(std::vector<std::complex<double>> x) {
			interpolate.consumeAdd(x);
		}
	};

	TEST_F(InterpolateFramesP2Q3Tests, interpolatesComplexMagnitudesAndAdvancesPhase) {
		assertInterpolatedFrames(
			{ 1.0 + 2i, 3.0 + 4i, 5.0 + 6i },
			{
				{ 2/3.0 + 4i/3.0, 2.0 + 8i/3.0, 10/3.0 + 4i }
			},
			1e-15
		);
		assertInterpolatedFrames(
			{ 7.0 + 8i, 9.0 + 10i, 11.0 + 12i },
			{
				twoThirdsMagnitudeOfFirstOneThirdOfSecondAndPhaseSecond(
					{ 1.0 + 2i, 3.0 + 4i, 5.0 + 6i },
					{ 7.0 + 8i, 9.0 + 10i, 11.0 + 12i }
				),
				{
					complex(magnitude(7.0 + 8i), 2*phase(7.0 + 8i) - phase(1.0 + 2i)),
					complex(magnitude(9.0 + 10i), 2*phase(9.0 + 10i) - phase(3.0 + 4i)),
					complex(magnitude(11.0 + 12i), 2*phase(11.0 + 12i) - phase(5.0 + 6i))
				}
			},
			1e-15
		);
	}
}