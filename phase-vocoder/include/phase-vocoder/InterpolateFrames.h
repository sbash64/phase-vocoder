#ifndef PHASEVOCODOER_INTERPOLATEFRAMES_H
#define PHASEVOCODOER_INTERPOLATEFRAMES_H

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

#endif