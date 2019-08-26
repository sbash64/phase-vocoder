#ifndef PHASEVOCODOER_INTERPOLATEFRAMES_H
#define PHASEVOCODOER_INTERPOLATEFRAMES_H

#include "common-utility.h"
#include <gsl/gsl>
#include <vector>
#include <complex>
#include <algorithm>
#include <functional>

namespace phase_vocoder {
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
	bool hasNext_{true};
	bool first_{true};
	bool skip_{};
public:
	InterpolateFrames(int P, int Q, int N) :
		previousFrame(N),
		currentFrame(N),
		accumulatedPhase(N),
		phaseAdvance(N),
		resampledMagnitude(N),
		numerator{ std::min(P, Q) },
		P{ P },
		Q{ Q } {}

	void add(gsl::span<const complex_type> x) {
		copy<complex_type>(currentFrame, previousFrame);
		copy<complex_type>(x, currentFrame);
		transformFrames(
			phaseAdvance,
			&InterpolateFrames::phaseDifference
		);
		if (P > Q && first_) {
			accumulatePhase();
			first_ = false;
		}
		hasNext_ = true;
		accumulatePhaseIfNeeded();
		updateSkip();
		checkIfNeedMore();
	}

	bool hasNext() { return hasNext_; }

	void next(gsl::span<complex_type> x) {
		resampleMagnitude();
		std::transform(
			resampledMagnitude.begin(),
			resampledMagnitude.end(),
			accumulatedPhase.begin(),
			x.begin(),
			[](T magnitude, T phase) {
				return std::polar(magnitude, phase);
			}
		);
		accumulatePhaseIfNeeded();
		numerator += P;
		updateSkip();
		checkIfNeedMore();
	}

private:
	void updateSkip() {
		skip_ = Q < numerator && numerator < Q + P;
	}

	void accumulatePhaseIfNeeded() {
		if (numerator != Q && !skip_)
			accumulatePhase();
	}

	void checkIfNeedMore() {
		if (numerator > Q) {
			numerator -= Q;
			hasNext_ = false;
		}
	}

	T phaseDifference(const complex_type& a, const complex_type& b) {
		return phase(b) - phase(a);
	}

	T phase(const complex_type& x) {
		return std::arg(x);
	}

	T magnitude(const complex_type &x) {
		return std::abs(x);
	}

	void transformFrames(
		std::vector<T> &out,
		T(InterpolateFrames::*f)(const complex_type &, const complex_type &)
	) {
		std::transform(
			previousFrame.begin(),
			previousFrame.end(),
			currentFrame.begin(),
			out.begin(),
			[&](const complex_type& a, const complex_type& b) {
				return (this->*f)(a, b);
			}
		);
	}

	T resampleMagnitude_(const complex_type& a, const complex_type& b) {
		T denominator = Q;
		auto ratio = numerator / denominator;
		return magnitude(a) * (1 - ratio) + magnitude(b) * ratio;
	}

	void resampleMagnitude() {
		transformFrames(
			resampledMagnitude,
			&InterpolateFrames::resampleMagnitude_
		);
	}

	void accumulatePhase() {
		addFirstToSecond<T>(phaseAdvance, accumulatedPhase);
	}
};
}

#endif
