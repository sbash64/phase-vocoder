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
	std::vector<T> accumulatedPhase;
	std::vector<T> phaseAdvance;
	std::vector<T> resampledMagnitude;
	int numerator;
	int P;
	int Q;
public:
	InterpolateFrames(int P, int Q, int N) :
		previous(N),
		current(N),
		accumulatedPhase(N),
		phaseAdvance(N),
		resampledMagnitude(N),
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
			phaseAdvance.begin(),
			[&](complex_type a, complex_type b) {
				return std::arg(b) - std::arg(a);
			}
		);
		std::transform(
			previous.begin(),
			previous.end(),
			current.begin(),
			resampledMagnitude.begin(),
			[&](complex_type a, complex_type b) {
				T denominator = Q;
				auto ratio = numerator / denominator;
				return std::abs(a) * (1 - ratio) + std::abs(b) * ratio;
			}
		);
		std::transform(
			phaseAdvance.begin(),
			phaseAdvance.end(),
			accumulatedPhase.begin(),
			accumulatedPhase.begin(),
			std::plus<T>{}
		);
		std::transform(
			resampledMagnitude.begin(),
			resampledMagnitude.end(),
			accumulatedPhase.begin(),
			x.begin(),
			[&](T magnitude, T phase) {
				return std::polar(magnitude, phase);
			}
		);
		if (numerator == Q) {
			numerator -= Q;
			previous = current;
		}
		else
			numerator += P;
	}
};

#endif