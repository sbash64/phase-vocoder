#ifndef PHASEVOCODOER_INTERPOLATEFRAMES_H
#define PHASEVOCODOER_INTERPOLATEFRAMES_H

#include "common-utility.h"
#include <gsl/gsl>
#include <vector>
#include <complex>
#include <algorithm>
#include <functional>
#include <iostream>

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
		numerator{ P },
		P{ P },
		Q{ Q } 
	{
		if (P > Q)
			numerator = Q;
	}

	void add(gsl::span<const complex_type> x) {
		copy<complex_type>(currentFrame, previousFrame);
		copy<complex_type>(x, currentFrame);
		transformFrames(
			phaseAdvance,
			&InterpolateFrames::phaseDifference
		);
		std::cout << "add\n";
		if (P > Q && first_)
			accumulatePhase();
		if (numerator != Q && !skip_) {
			std::cout << "add: acc\n";
			accumulatePhase();
		}
		hasNext_ = true;
		skip_ = false;
		if (numerator > Q) {
			numerator -= Q;
			hasNext_ = false;
			if (numerator < Q)
				skip_ = true;
		}
		first_ = false;
	}

	bool hasNext() { return hasNext_; }

	void next(gsl::span<complex_type> x) {
		std::cout << "next\n";
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
		if (numerator != Q) {
			std::cout << "next: acc\n";
			accumulatePhase();
			if (numerator + P > Q)
				skip_ = true;
		}
		numerator += P;
		if (numerator > Q) {
			numerator -= Q;
			hasNext_ = false;
		}
	}

private:
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
