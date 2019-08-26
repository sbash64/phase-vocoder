#include "assert-utility.h"
#include <phase-vocoder/InterpolateFrames.h>
#include <gtest/gtest.h>

namespace {
using namespace std::complex_literals;

double magnitude(const std::complex<double> &x) {
	return std::abs(x);
}

double phase(const std::complex<double> &x) {
	return std::arg(x);
}

std::complex<double> complex(double magnitude, double radians) {
	return std::polar(magnitude, radians);
}

std::complex<double> exp(const std::complex<double> &x) {
	return std::exp(x);
}

std::complex<double> doublePhase(const std::complex<double> &x) {
	return x * exp(phase(x) * 1i);
}

std::complex<double> triplePhase(const std::complex<double> &x) {
	return x * exp(phase(x) * 2i);
}

std::vector<std::complex<double>> transform(
	std::vector<std::complex<double>> a,
	std::complex<double>(*f)(const std::complex<double> &)
) {
	std::transform(
		a.begin(),
		a.end(),
		a.begin(),
		[=](const std::complex<double> &a_) { return (*f)(a_); }
	);
	return a;
}

std::vector<std::complex<double>> doublePhase(
	std::vector<std::complex<double>> x
) {
	return transform(std::move(x), doublePhase);
}

std::vector<std::complex<double>> triplePhase(
	std::vector<std::complex<double>> x
) {
	return transform(std::move(x), triplePhase);
}

double averageMagnitude(
	const std::complex<double> &a,
	const std::complex<double> &b
) {
	return (magnitude(a) + magnitude(b)) / 2;
}

double summedPhase(
	const std::complex<double> &a,
	const std::complex<double> &b
) {
	return phase(a) + phase(b);
}

std::complex<double> averageMagnitudesAndSumPhases(
	const std::complex<double> &a,
	const std::complex<double> &b
) {
	return complex(averageMagnitude(a, b), summedPhase(a, b));
}

std::vector<std::complex<double>> transform(
	std::vector<std::complex<double>> a,
	const std::vector<std::complex<double>> &b,
	std::complex<double>(*f)(
		const std::complex<double> &,
		const std::complex<double> &
	)
) {
	std::transform(
		a.begin(),
		a.end(),
		b.begin(),
		a.begin(),
		[=](
			const std::complex<double> &a_,
			const std::complex<double> &b_
		) { return (*f)(a_, b_); }
	);
	return a;
}

std::vector<std::complex<double>> averageMagnitudesAndSumPhases(
	std::vector<std::complex<double>> a,
	const std::vector<std::complex<double>> &b
) {
	return transform(std::move(a), b, averageMagnitudesAndSumPhases);
}

std::complex<double>
twoThirdsMagnitudeFirstPlusOneThirdSecondAndDoublePhaseFirstPlusSecond(
	const std::complex<double>& a,
	const std::complex<double>& b
) {
	return complex(
		(2 * magnitude(a) + magnitude(b)) / 3,
		2 * phase(a) + phase(b)
	);
}

std::complex<double>
twoThirdsMagnitudeFirstPlusOneThirdSecondAndDoublePhaseFirst(
	const std::complex<double>& a,
	const std::complex<double>& b
) {
	return complex(
		(2 * magnitude(a) + magnitude(b)) / 3,
		2 * phase(a)
	);
}

std::complex<double> magnitudeSecondAndSummedPhase(
	const std::complex<double>& a,
	const std::complex<double>& b
) {
	return complex(magnitude(b), summedPhase(a, b));
}

std::complex<double>
oneThirdMagnitudeFirstPlusTwoThirdsSecondAndPhaseFirstPlusDoubleSecond(
	const std::complex<double>& a,
	const std::complex<double>& b
) {
	return complex(
		(magnitude(a) + 2 * magnitude(b)) / 3,
		phase(a) + 2 * phase(b)
	);
}

std::complex<double> magnitudeSecondAndPhaseFirst(
	const std::complex<double>& a,
	const std::complex<double>& b
) {
	return complex(magnitude(b), phase(a));
}

std::complex<double> averageMagnitudesAndPhaseFirst(
	const std::complex<double>& a,
	const std::complex<double>& b
) {
	return complex(averageMagnitude(a, b), phase(a));
}

std::vector<std::complex<double>>
twoThirdsMagnitudeFirstPlusOneThirdSecondAndDoublePhaseFirst(
	std::vector<std::complex<double>> a,
	const std::vector<std::complex<double>> &b
) {
	return transform(
		std::move(a),
		b,
		twoThirdsMagnitudeFirstPlusOneThirdSecondAndDoublePhaseFirst
	);
}

std::vector<std::complex<double>> magnitudeSecondAndSummedPhase(
	std::vector<std::complex<double>> a,
	const std::vector<std::complex<double>> &b
) {
	return transform(
		std::move(a),
		b,
		magnitudeSecondAndSummedPhase
	);
}

std::vector<std::complex<double>>
twoThirdsMagnitudeFirstPlusOneThirdSecondAndDoublePhaseFirstPlusSecond(
	std::vector<std::complex<double>> a,
	const std::vector<std::complex<double>>& b
) {
	return transform(
		std::move(a),
		b,
		twoThirdsMagnitudeFirstPlusOneThirdSecondAndDoublePhaseFirstPlusSecond
	);
}

std::vector<std::complex<double>>
oneThirdMagnitudeFirstPlusTwoThirdsSecondAndPhaseFirstPlusDoubleSecond(
	std::vector<std::complex<double>> a,
	const std::vector<std::complex<double>>& b
) {
	return transform(
		std::move(a),
		b,
		oneThirdMagnitudeFirstPlusTwoThirdsSecondAndPhaseFirstPlusDoubleSecond
	);
}

std::vector<std::complex<double>>
magnitudeSecondAndPhaseFirst(
	std::vector<std::complex<double>> a,
	const std::vector<std::complex<double>>& b
) {
	return transform(
		std::move(a),
		b,
		magnitudeSecondAndPhaseFirst
	);
}

std::vector<std::complex<double>> averageMagnitudesAndPhaseFirst(
	std::vector<std::complex<double>> a,
	const std::vector<std::complex<double>>& b
) {
	return transform(
		std::move(a),
		b,
		averageMagnitudesAndPhaseFirst
	);
}

class InterpolateFramesFacade {
	phase_vocoder::InterpolateFrames<double> interpolate;
	int N;
public:
	InterpolateFramesFacade(int P, int Q, int N) :
		interpolate{ P, Q, N },
		N{ N } {}

	void assertInterpolatedFrames(
		const std::vector<std::complex<double>> &x,
		const std::vector<std::vector<std::complex<double>>> &frames,
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

	void add(const std::vector<std::complex<double>> &x) {
		interpolate.add(x);
	}

	void consumeAdd(const std::vector<std::complex<double>> &x) {
		add(x);
		while (hasNext())
			next();
	}

	std::vector<std::complex<double>> next() {
		std::vector<std::complex<double>> out(N);
		interpolate.next(out);
		return out;
	}

	void assertNextEquals(
		const std::vector<std::complex<double>> &expected,
		double tolerance
	) {
		assertEqual(expected, next(), tolerance);
	}
};

void consumeAdd(
	InterpolateFramesFacade &interpolate,
	const std::vector<std::complex<double>>& x
) {
	interpolate.consumeAdd(x);
}

void assertInterpolatedFrames(
	InterpolateFramesFacade& interpolate,
	const std::vector<std::complex<double>>& x,
	const std::vector<std::vector<std::complex<double>>>& frames,
	double tolerance
) {
	interpolate.assertInterpolatedFrames(x, frames, tolerance);
}

class InterpolateFramesP1Q2Tests : public ::testing::Test {
	int P = 1;
	int Q = 2;
	int N = 3;
	InterpolateFramesFacade interpolate{ P, Q, N };
protected:
	void assertInterpolatedFrames(
		const std::vector<std::complex<double>> &x,
		const std::vector<std::vector<std::complex<double>>> &frames,
		double tolerance
	) {
		::assertInterpolatedFrames(interpolate, x, frames, tolerance);
	}

	void consumeAdd(const std::vector<std::complex<double>> &x) {
		::consumeAdd(interpolate, x);
	}
};

TEST_F(
	InterpolateFramesP1Q2Tests,
	interpolatesComplexMagnitudesAndAdvancesPhase
) {
	assertInterpolatedFrames(
		{1. + 2i, 3. + 4i, 5. + 6i},
		{
			{ 1./2 + 1i, 3./2 + 2i, 5./2 + 3i},
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
		const std::vector<std::complex<double>>& x,
		const std::vector<std::vector<std::complex<double>>>& frames,
		double tolerance
	) {
		::assertInterpolatedFrames(interpolate, x, frames, tolerance);
	}

	void consumeAdd(const std::vector<std::complex<double>>& x) {
		::consumeAdd(interpolate, x);
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
		const std::vector<std::complex<double>>& x,
		const std::vector<std::vector<std::complex<double>>>& frames,
		double tolerance
	) {
		::assertInterpolatedFrames(interpolate, x, frames, tolerance);
	}

	void consumeAdd(const std::vector<std::complex<double>>& x) {
		::consumeAdd(interpolate, x);
	}
};

TEST_F(
	InterpolateFramesP1Q3Tests,
	interpolatesComplexMagnitudesAndAdvancesPhase
) {
	assertInterpolatedFrames(
		{ 1. + 2i, 3. + 4i, 5. + 6i },
		{
			{ 1/3. + 2i/3., 1. + 4i/3., 5/3. + 2i },
			doublePhase({ 2/3. + 4i/3., 2. + 8i/3., 10/3. + 4i }),
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
		const std::vector<std::complex<double>>& x,
		const std::vector<std::vector<std::complex<double>>>& frames,
		double tolerance
	) {
		::assertInterpolatedFrames(interpolate, x, frames, tolerance);
	}

	void consumeAdd(const std::vector<std::complex<double>>& x) {
		::consumeAdd(interpolate, x);
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
	assertInterpolatedFrames(
		{ 7. + 8i, 9. + 10i, 11. + 12i },
		{},
		1e-15
	);
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
		const std::vector<std::complex<double>>& x,
		const std::vector<std::vector<std::complex<double>>>& frames,
		double tolerance
	) {
		::assertInterpolatedFrames(interpolate, x, frames, tolerance);
	}

	void consumeAdd(const std::vector<std::complex<double>>& x) {
		::consumeAdd(interpolate, x);
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
	assertInterpolatedFrames(
		{ 7. + 8i, 9. + 10i, 11. + 12i },
		{},
		1e-15
	);
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
}