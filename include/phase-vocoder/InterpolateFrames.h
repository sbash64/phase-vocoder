#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_INTERPOLATEFRAMES_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_INTERPOLATEFRAMES_H_

#include "model.h"
#include "utility.h"
#include <algorithm>
#include <functional>

namespace phase_vocoder {
template<typename T>
class InterpolateFrames {
	using frame_type = complex_buffer_type<T>;
	frame_type previousFrame;
	frame_type currentFrame;
	buffer_type<T> accumulatedPhase;
	buffer_type<T> phaseAdvance;
	buffer_type<T> resampledMagnitude;
	int numerator;
	int P;
	int Q;
	bool hasNext_{true};
	bool first_{true};
	bool skip_{};
public:
	InterpolateFrames(int P, int Q, int N) :
		previousFrame(sizeNarrow<complex_type<T>>(N)),
		currentFrame(sizeNarrow<complex_type<T>>(N)),
		accumulatedPhase(sizeNarrow<T>(N)),
		phaseAdvance(sizeNarrow<T>(N)),
		resampledMagnitude(sizeNarrow<T>(N)),
		numerator{std::min(P, Q)},
		P{P},
		Q{Q} {}

	void add(const_complex_signal_type<T> x) {
		copy<complex_type<T>>(currentFrame, previousFrame);
		copy(x, currentFrame);
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

	void next(complex_signal_type<T> x) {
		resampleMagnitude();
		std::transform(
			begin(resampledMagnitude),
			end(resampledMagnitude),
			begin(accumulatedPhase),
			begin(x),
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

	T phaseDifference(const complex_type<T>& a, const complex_type<T>& b) {
		return phase(b) - phase(a);
	}

	T phase(const complex_type<T>& x) {
		return std::arg(x);
	}

	T magnitude(const complex_type<T> &x) {
		return std::abs(x);
	}

	void transformFrames(
		buffer_type<T> &out,
		T(InterpolateFrames::*f)(const complex_type<T> &, const complex_type<T> &)
	) {
		std::transform(
			begin(previousFrame),
			end(previousFrame),
			begin(currentFrame),
			begin(out),
			[&](const complex_type<T>& a, const complex_type<T>& b) {
				return (this->*f)(a, b);
			}
		);
	}

	T resampleMagnitude(const complex_type<T>& a, const complex_type<T>& b) {
		T denominator = Q;
		auto ratio = numerator / denominator;
		return magnitude(a) * (1 - ratio) + magnitude(b) * ratio;
	}

	void resampleMagnitude() {
		transformFrames(
			resampledMagnitude,
			&InterpolateFrames::resampleMagnitude
		);
	}

	void accumulatePhase() {
		addFirstToSecond<T>(phaseAdvance, accumulatedPhase);
	}
};
}

#endif
