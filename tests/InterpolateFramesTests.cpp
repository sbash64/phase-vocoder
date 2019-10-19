#include "assert-utility.h"
#include <phase-vocoder/InterpolateFrames.h>
#include <gtest/gtest.h>

namespace phase_vocoder::test { namespace {
using namespace std::complex_literals;

double magnitude(const complex_type<double> &x) {
	return std::abs(x);
}

double phase(const complex_type<double> &x) {
	return std::arg(x);
}

complex_type<double> complex(double magnitude, double radians) {
	return std::polar(magnitude, radians);
}

complex_type<double> exp(const complex_type<double> &x) {
	return std::exp(x);
}

complex_type<double> doublePhase(const complex_type<double> &x) {
	return x * exp(phase(x) * 1i);
}

complex_type<double> triplePhase(const complex_type<double> &x) {
	return x * exp(phase(x) * 2i);
}

std::vector<complex_type<double>> transform(
	std::vector<complex_type<double>> a,
	complex_type<double>(*f)(const complex_type<double> &)
) {
	std::transform(
		a.begin(),
		a.end(),
		a.begin(),
		[=](const complex_type<double> &a_) { return (*f)(a_); }
	);
	return a;
}

std::vector<complex_type<double>> doublePhase(
	std::vector<complex_type<double>> x
) {
	return transform(std::move(x), doublePhase);
}

std::vector<complex_type<double>> triplePhase(
	std::vector<complex_type<double>> x
) {
	return transform(std::move(x), triplePhase);
}

double averageMagnitude(
	const complex_type<double> &a,
	const complex_type<double> &b
) {
	return (magnitude(a) + magnitude(b)) / 2;
}

double summedPhase(
	const complex_type<double> &a,
	const complex_type<double> &b
) {
	return phase(a) + phase(b);
}

complex_type<double> averageMagnitudesAndSumPhases(
	const complex_type<double> &a,
	const complex_type<double> &b
) {
	return complex(averageMagnitude(a, b), summedPhase(a, b));
}

std::vector<complex_type<double>> transform(
	std::vector<complex_type<double>> a,
	const std::vector<complex_type<double>> &b,
	complex_type<double>(*f)(
		const complex_type<double> &,
		const complex_type<double> &
	)
) {
	std::transform(
		a.begin(),
		a.end(),
		b.begin(),
		a.begin(),
		[=](
			const complex_type<double> &a_,
			const complex_type<double> &b_
		) { return (*f)(a_, b_); }
	);
	return a;
}

std::vector<complex_type<double>> averageMagnitudesAndSumPhases(
	std::vector<complex_type<double>> a,
	const std::vector<complex_type<double>> &b
) {
	return transform(std::move(a), b, averageMagnitudesAndSumPhases);
}

complex_type<double>
twoThirdsMagnitudeFirstPlusOneThirdSecondAndDoublePhaseFirstPlusSecond(
	const complex_type<double>& a,
	const complex_type<double>& b
) {
	return complex(
		(2*magnitude(a) + magnitude(b)) / 3,
		2*phase(a) + phase(b)
	);
}

complex_type<double>
twoThirdsMagnitudeFirstPlusOneThirdSecondAndDoublePhaseFirst(
	const complex_type<double>& a,
	const complex_type<double>& b
) {
	return complex(
		(2 * magnitude(a) + magnitude(b)) / 3,
		2 * phase(a)
	);
}

complex_type<double> magnitudeSecondAndSummedPhase(
	const complex_type<double>& a,
	const complex_type<double>& b
) {
	return complex(magnitude(b), summedPhase(a, b));
}

complex_type<double>
oneThirdMagnitudeFirstPlusTwoThirdsSecondAndPhaseFirstPlusDoubleSecond(
	const complex_type<double>& a,
	const complex_type<double>& b
) {
	return complex(
		(magnitude(a) + 2 * magnitude(b)) / 3,
		phase(a) + 2 * phase(b)
	);
}

complex_type<double> magnitudeSecondAndPhaseFirst(
	const complex_type<double>& a,
	const complex_type<double>& b
) {
	return complex(magnitude(b), phase(a));
}

complex_type<double> averageMagnitudesAndPhaseFirst(
	const complex_type<double>& a,
	const complex_type<double>& b
) {
	return complex(averageMagnitude(a, b), phase(a));
}

std::vector<complex_type<double>>
twoThirdsMagnitudeFirstPlusOneThirdSecondAndDoublePhaseFirst(
	std::vector<complex_type<double>> a,
	const std::vector<complex_type<double>> &b
) {
	return transform(
		std::move(a),
		b,
		twoThirdsMagnitudeFirstPlusOneThirdSecondAndDoublePhaseFirst
	);
}

std::vector<complex_type<double>> magnitudeSecondAndSummedPhase(
	std::vector<complex_type<double>> a,
	const std::vector<complex_type<double>> &b
) {
	return transform(
		std::move(a),
		b,
		magnitudeSecondAndSummedPhase
	);
}

std::vector<complex_type<double>>
twoThirdsMagnitudeFirstPlusOneThirdSecondAndDoublePhaseFirstPlusSecond(
	std::vector<complex_type<double>> a,
	const std::vector<complex_type<double>>& b
) {
	return transform(
		std::move(a),
		b,
		twoThirdsMagnitudeFirstPlusOneThirdSecondAndDoublePhaseFirstPlusSecond
	);
}

std::vector<complex_type<double>>
oneThirdMagnitudeFirstPlusTwoThirdsSecondAndPhaseFirstPlusDoubleSecond(
	std::vector<complex_type<double>> a,
	const std::vector<complex_type<double>>& b
) {
	return transform(
		std::move(a),
		b,
		oneThirdMagnitudeFirstPlusTwoThirdsSecondAndPhaseFirstPlusDoubleSecond
	);
}

std::vector<complex_type<double>> magnitudeSecondAndPhaseFirst(
	std::vector<complex_type<double>> a,
	const std::vector<complex_type<double>>& b
) {
	return transform(
		std::move(a),
		b,
		magnitudeSecondAndPhaseFirst
	);
}

std::vector<complex_type<double>> averageMagnitudesAndPhaseFirst(
	std::vector<complex_type<double>> a,
	const std::vector<complex_type<double>>& b
) {
	return transform(
		std::move(a),
		b,
		averageMagnitudesAndPhaseFirst
	);
}

class InterpolateFramesFacade {
	InterpolateFrames<double> interpolate;
	int N;
public:
	InterpolateFramesFacade(int P, int Q, int N) :
		interpolate{ P, Q, N },
		N{ N } {}

	void assertYieldsNoFrames(
		const std::vector<complex_type<double>> &x
	) {
		add(x);
		assertDoesNotHaveNext();
	}

	void assertInterpolatedFrames(
		const std::vector<complex_type<double>> &x,
		const std::vector<std::vector<complex_type<double>>> &frames,
		double tolerance
	) {
		add(x);
		for (auto frame : frames) {
			assertHasNext();
			assertNextEquals(frame, tolerance);
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

	void add(const std::vector<complex_type<double>> &x) {
		interpolate.add(x);
	}

	void consumeAdd(const std::vector<complex_type<double>> &x) {
		add(x);
		while (hasNext())
			next();
	}

	std::vector<complex_type<double>> next() {
		std::vector<complex_type<double>> out(gsl::narrow_cast<size_t>(N));
		interpolate.next(out);
		return out;
	}

	void assertNextEquals(
		const std::vector<complex_type<double>> &expected,
		double tolerance
	) {
		assertEqual(expected, next(), tolerance);
	}
};

void consumeAdd(
	InterpolateFramesFacade &interpolate,
	const std::vector<complex_type<double>>& x
) {
	interpolate.consumeAdd(x);
}

void assertInterpolatedFrames(
	InterpolateFramesFacade& interpolate,
	const std::vector<complex_type<double>>& x,
	const std::vector<std::vector<complex_type<double>>>& frames,
	double tolerance
) {
	interpolate.assertInterpolatedFrames(x, frames, tolerance);
}

void assertYieldsNoFrames(
	InterpolateFramesFacade& interpolate,
	const std::vector<complex_type<double>>& x
) {
	interpolate.assertYieldsNoFrames(x);
}

class InterpolateFramesP1Q2Tests : public ::testing::Test {
	int P = 1;
	int Q = 2;
	int N = 3;
	InterpolateFramesFacade interpolate{ P, Q, N };
protected:
	void assertInterpolatedFrames(
		const std::vector<complex_type<double>> &x,
		const std::vector<std::vector<complex_type<double>>> &frames,
		double tolerance
	) {
		phase_vocoder::test::assertInterpolatedFrames(interpolate, x, frames, tolerance);
	}

	void consumeAdd(const std::vector<complex_type<double>> &x) {
        phase_vocoder::test::consumeAdd(interpolate, x);
	}
};

TEST_F(
	InterpolateFramesP1Q2Tests,
	interpolatesComplexMagnitudesAndAdvancesPhase
) {
	assertInterpolatedFrames(
		{1. + 2i, 3. + 4i, 5. + 6i},
		{
			{ (1. + 2i)/2., (3. + 4i)/2., (5. + 6i)/2. },
			doublePhase({1. + 2i, 3. + 4i, 5. + 6i})
		},
		1e-15
	);
}

TEST_F(
	InterpolateFramesP1Q2Tests,
	interpolatesComplexMagnitudesAndAdvancesPhase2
) {
	consumeAdd({ 1. + 2i, 3. + 4i, 5. + 6i });
	assertInterpolatedFrames(
		{ 7. + 8i, 9. + 10i, 11. + 12i },
		{
			averageMagnitudesAndSumPhases(
				{ 1. + 2i, 3. + 4i, 5. + 6i },
				{ 7. + 8i, 9. + 10i, 11. + 12i }
			),
			doublePhase({ 7. + 8i, 9. + 10i, 11. + 12i })
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
		const std::vector<complex_type<double>>& x,
		const std::vector<std::vector<complex_type<double>>>& frames,
		double tolerance
	) {
        phase_vocoder::test::assertInterpolatedFrames(interpolate, x, frames, tolerance);
	}

	void consumeAdd(const std::vector<complex_type<double>>& x) {
        phase_vocoder::test::consumeAdd(interpolate, x);
	}
};

TEST_F(
	InterpolateFramesP2Q3Tests,
	interpolatesComplexMagnitudesAndAdvancesPhase
) {
	assertInterpolatedFrames(
		{ 1. + 2i, 3. + 4i, 5. + 6i },
		{
			{ 2.*(1. + 2i)/3., 2.*(3. + 4i)/3., 2.*(5. + 6i)/3. }
		},
		1e-15
	);
}

TEST_F(
	InterpolateFramesP2Q3Tests,
	interpolatesComplexMagnitudesAndAdvancesPhase2
) {
	consumeAdd({ 1. + 2i, 3. + 4i, 5. + 6i });
	assertInterpolatedFrames(
		{ 7. + 8i, 9. + 10i, 11. + 12i },
		{
			twoThirdsMagnitudeFirstPlusOneThirdSecondAndDoublePhaseFirst(
				{ 1. + 2i, 3. + 4i, 5. + 6i },
				{ 7. + 8i, 9. + 10i, 11. + 12i }
			),
			magnitudeSecondAndSummedPhase(
				{ 1. + 2i, 3. + 4i, 5. + 6i },
				{ 7. + 8i, 9. + 10i, 11. + 12i }
			)
		},
		1e-15
	);
}

class InterpolateFramesP1Q3Tests : public ::testing::Test {
	int P = 1;
	int Q = 3;
	int N = 3;
	InterpolateFramesFacade interpolate{ P, Q, N };
protected:
	void assertInterpolatedFrames(
		const std::vector<complex_type<double>>& x,
		const std::vector<std::vector<complex_type<double>>>& frames,
		double tolerance
	) {
        phase_vocoder::test::assertInterpolatedFrames(interpolate, x, frames, tolerance);
	}

	void consumeAdd(const std::vector<complex_type<double>>& x) {
        phase_vocoder::test::consumeAdd(interpolate, x);
	}
};

TEST_F(
	InterpolateFramesP1Q3Tests,
	interpolatesComplexMagnitudesAndAdvancesPhase
) {
	assertInterpolatedFrames(
		{ 1. + 2i, 3. + 4i, 5. + 6i },
		{
			{ (1. + 2i)/3., (3. + 4i)/3., (5. + 6i)/3. },
			doublePhase({ 2.*(1. + 2i)/3., 2.*(3. + 4i)/3., 2.*(5. + 6i)/3. }),
			triplePhase({ 1. + 2i, 3. + 4i, 5. + 6i })
		},
		1e-15
	);
}

TEST_F(
	InterpolateFramesP1Q3Tests,
	interpolatesComplexMagnitudesAndAdvancesPhase2
) {
	consumeAdd({ 1. + 2i, 3. + 4i, 5. + 6i });
	assertInterpolatedFrames(
		{ 7. + 8i, 9. + 10i, 11. + 12i },
		{
			twoThirdsMagnitudeFirstPlusOneThirdSecondAndDoublePhaseFirstPlusSecond(
				{ 1. + 2i, 3. + 4i, 5. + 6i },
				{ 7. + 8i, 9. + 10i, 11. + 12i }
			),
			oneThirdMagnitudeFirstPlusTwoThirdsSecondAndPhaseFirstPlusDoubleSecond(
				{ 1. + 2i, 3. + 4i, 5. + 6i },
				{ 7. + 8i, 9. + 10i, 11. + 12i }
			),
			triplePhase({ 7. + 8i, 9. + 10i, 11. + 12i })
		},
		1e-15
	);
}

class InterpolateFramesP2Q1Tests : public ::testing::Test {
	int P = 2;
	int Q = 1;
	int N = 3;
	InterpolateFramesFacade interpolate{ P, Q, N };
protected:
	void assertInterpolatedFrames(
		const std::vector<complex_type<double>>& x,
		const std::vector<std::vector<complex_type<double>>>& frames,
		double tolerance
	) {
        phase_vocoder::test::assertInterpolatedFrames(interpolate, x, frames, tolerance);
	}

	void assertYieldsNoFrames(
		const std::vector<complex_type<double>>& x
	) {
        phase_vocoder::test::assertYieldsNoFrames(interpolate, x);
	}

	void consumeAdd(const std::vector<complex_type<double>>& x) {
        phase_vocoder::test::consumeAdd(interpolate, x);
	}
};

TEST_F(
	InterpolateFramesP2Q1Tests, 
	interpolatesComplexMagnitudesAndAdvancesPhase
) {
	assertInterpolatedFrames(
		{ 1. + 2i, 3. + 4i, 5. + 6i },
		{
			{ 1. + 2i, 3. + 4i, 5. + 6i }
		},
		1e-15
	);
}

TEST_F(
	InterpolateFramesP2Q1Tests, 
	interpolatesComplexMagnitudesAndAdvancesPhase2
) {
	consumeAdd({ 1. + 2i, 3. + 4i, 5. + 6i });
	assertYieldsNoFrames({ 7. + 8i, 9. + 10i, 11. + 12i });
	assertInterpolatedFrames(
		{ 13. + 14i, 15. + 16i, 17. + 18i },
		{
			magnitudeSecondAndPhaseFirst(
				{ 7. + 8i, 9. + 10i, 11. + 12i },
				{ 13. + 14i, 15. + 16i, 17. + 18i }
			)
		},
		1e-15
	);
}

class InterpolateFramesP3Q2Tests : public ::testing::Test {
	int P = 3;
	int Q = 2;
	int N = 3;
	InterpolateFramesFacade interpolate{ P, Q, N };
protected:
	void assertInterpolatedFrames(
		const std::vector<complex_type<double>>& x,
		const std::vector<std::vector<complex_type<double>>>& frames,
		double tolerance
	) {
        phase_vocoder::test::assertInterpolatedFrames(interpolate, x, frames, tolerance);
	}

	void assertYieldsNoFrames(
		const std::vector<complex_type<double>>& x
	) {
        phase_vocoder::test::assertYieldsNoFrames(interpolate, x);
	}

	void consumeAdd(const std::vector<complex_type<double>>& x) {
        phase_vocoder::test::consumeAdd(interpolate, x);
	}
};

TEST_F(
	InterpolateFramesP3Q2Tests,
	interpolatesComplexMagnitudesAndAdvancesPhase
) {
	assertInterpolatedFrames(
		{ 1. + 2i, 3. + 4i, 5. + 6i },
		{
			{ 1. + 2i, 3. + 4i, 5. + 6i }
		},
		1e-15
	);
}

TEST_F(
	InterpolateFramesP3Q2Tests,
	interpolatesComplexMagnitudesAndAdvancesPhase2
) {
	consumeAdd({ 1. + 2i, 3. + 4i, 5. + 6i });
	assertYieldsNoFrames({ 7. + 8i, 9. + 10i, 11. + 12i });
	assertInterpolatedFrames(
		{ 13. + 14i, 15. + 16i, 17. + 18i },
		{
			averageMagnitudesAndPhaseFirst(
				{ 7. + 8i, 9. + 10i, 11. + 12i },
				{ 13. + 14i, 15. + 16i, 17. + 18i }
			)
		},
		1e-15
	);
}
}}
