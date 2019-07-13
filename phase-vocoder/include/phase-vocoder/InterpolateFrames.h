#ifndef PHASEVOCODOER_INTERPOLATEFRAMES_H
#define PHASEVOCODOER_INTERPOLATEFRAMES_H

#include <gsl/gsl>
#include <vector>
#include <complex>

template<typename T>
class InterpolateFrames {
	using complex_type = std::complex<T>;
	using frame_type = std::vector<complex_type>;
	frame_type previousFrame;
	frame_type currentFrame;
	std::vector<T> accumulatedPhase;
	std::vector<T> phaseAdvance;
	std::vector<T> resampledMagnitude;
	int numerator;
	int P;
	int Q;
	bool hasNext_{};
public:
	InterpolateFrames(int P, int Q, int N) :
		previousFrame(N),
		currentFrame(N),
		accumulatedPhase(N),
		phaseAdvance(N),
		resampledMagnitude(N),
		numerator{ P },
		P{ P },
		Q{ Q }
	{
	}

	void add(gsl::span<complex_type> x) {
		copy(currentFrame, previousFrame);
		copy(x, currentFrame);
		transformFrames(
			phaseAdvance,
			[&](complex_type a, complex_type b) {
				return phase(b) - phase(a);
			}
		);
		hasNext_ = true;
	}

	bool hasNext() { return hasNext_; }

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
		numerator += P;
		if (numerator > Q) {
			numerator -= Q;
			hasNext_ = false;
		}
	}

private:
	void copy(gsl::span<complex_type> source, std::vector<complex_type>& destination) {
		std::copy(source.begin(), source.end(), destination.begin());
	}

	T phase(complex_type x) {
		return std::arg(x);
	}

	T magnitude(complex_type x) {
		return std::abs(x);
	}

	void transformFrames(
		std::vector<T> &out, 
		std::function<T(complex_type, complex_type)> f
	) {
		std::transform(
			previousFrame.begin(),
			previousFrame.end(),
			currentFrame.begin(),
			out.begin(),
			f
		);
	}

	void resampleMagnitude() {
		transformFrames(
			resampledMagnitude,
			[&](complex_type a, complex_type b) {
				T denominator = Q;
				auto ratio = numerator / denominator;
				return magnitude(a) * (1 - ratio) + magnitude(b) * ratio;
			}
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
};

#endif