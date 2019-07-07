#include <gsl/gsl>
#include <vector>
#include <complex>

template<typename T>
class InterpolateFrames {
	using complex_type = std::complex<T>;
	std::vector<complex_type> previous;
	std::vector<complex_type> current;
	int numerator;
	int P;
	int Q;
public:
	InterpolateFrames(int P, int Q, int N) : 
		previous(N), 
		current(N),
		numerator{ 0 },
		P{ P },
		Q{ Q }
	{
	}

	void add(gsl::span<complex_type> x) {
		std::copy(x.begin(), x.end(), current.begin());
		numerator += P;
	}

	bool hasNext() { return numerator != 0; }

	void next(gsl::span<complex_type> x) {
		std::transform(
			previous.begin(),
			previous.end(),
			current.begin(),
			x.begin(),
			[&](complex_type a, complex_type b) {
				T denominator = Q;
				auto ratio = numerator / denominator;
				auto magnitude = std::abs(a) * (1 - ratio) + std::abs(b) * ratio;
				auto phase = std::arg(b) - std::arg(a);
				return std::polar(magnitude, phase);
			}
		);
		if (numerator == Q)
			numerator -= Q;
		else
			numerator += P;
	}
};

#include <gtest/gtest.h>

namespace {
	template<typename T>
	bool normDifferenceBelowTolerance(std::complex<T> a, std::complex<T> b, T e) {
		return std::norm(a - b) < e;
	}

	template<typename T>
	void assertEqual(std::complex<T> a, std::complex<T> b, T e) {
		EXPECT_PRED3(normDifferenceBelowTolerance<T>, a, b, e);
	}

	template<typename T>
	void assertEqual(
		std::vector<std::complex<T>> expected, 
		std::vector<std::complex<T>> actual, 
		T tolerance
	) {
		EXPECT_EQ(expected.size(), actual.size());
		for (std::size_t i = 0; i < expected.size(); ++i)
			assertEqual(expected.at(i), actual.at(i), tolerance);
	}

	template<typename T>
	void assertEqual(std::vector<T> expected, std::vector<T> actual) {
		EXPECT_EQ(expected.size(), actual.size());
		for (std::size_t i = 0; i < expected.size(); ++i)
			EXPECT_EQ(expected.at(i), actual.at(i));
	}

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