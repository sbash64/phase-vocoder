#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPEXTRACT_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPEXTRACT_H_

#include "common-utility.h"
#include <gsl/gsl>
#include <vector>
#include <algorithm>

namespace phase_vocoder {
template<typename T>
class OverlapExtract {
	std::vector<T> cached;
	gsl::span<const T> signal;
	typename gsl::span<T>::index_type head;
	int hop;
	int N;
public:
	OverlapExtract(int N, int hop) : cached(N), head{ 0 }, hop{ hop }, N{ N } {}

	void add(gsl::span<const T> x) {
		add_(x);
	}

	bool hasNext() {
		return head == N;
	}

	void next(gsl::span<T> out) {
		copy<T>(cached, out);
		shift<T>(cached, hop);
		head = N - hop;
		add_(signal);
	}

private:
	void add_(gsl::span<const T> x) {
		auto toFill = leftToFill(x);
		copyToCached(x, toFill);
		assignRemainingSignal(x, toFill);
		addToHead(toFill);
	}

	int leftToFill(gsl::span<const T> x) {
		return std::min(N - head, size(x));
	}

	void copyToCached(gsl::span<const T> x, int n) {
		std::copy(begin(x), begin(x) + n, begin(cached) + head);
	}

	void assignRemainingSignal(gsl::span<const T> x, int used) {
		signal = x.last(size(x) - used);
	}

	void addToHead(int n) {
		head += n;
	}
};
}

#endif
