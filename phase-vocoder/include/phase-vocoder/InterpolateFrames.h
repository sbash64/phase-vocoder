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
		transformFrames(
			phaseAdvance.begin(),
			[&](complex_type a, complex_type b) {
				return phase(b) - phase(a);
			}
		);
		numerator += P;
	}

	bool hasNext() { return numerator != 0; }

	void next(gsl::span<complex_type> x) {
		resampleMagnitude();
		accumulatePhase();
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

private:
	T phase(complex_type x) {
		return std::arg(x);
	}

	T magnitude(complex_type x) {
		return std::abs(x);
	}

	void transformFrames(
		typename std::vector<T>::iterator it, 
		std::function<T(complex_type, complex_type)> f
	) {
		std::transform(
			previous.begin(),
			previous.end(),
			current.begin(),
			it,
			f
		);
	}

	void accumulatePhase() {
		std::transform(
			phaseAdvance.begin(),
			phaseAdvance.end(),
			accumulatedPhase.begin(),
			accumulatedPhase.begin(),
			std::plus<T>{}
		);
	}

	void resampleMagnitude() {
		transformFrames(
			resampledMagnitude.begin(),
			[&](complex_type a, complex_type b) {
				T denominator = Q;
				auto ratio = numerator / denominator;
				return magnitude(a) * (1 - ratio) + magnitude(b) * ratio;
			}
		);
	}
};

#endif