#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPEXTRACT_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPEXTRACT_H_

#include "common-utility.h"
#include <vector>
#include <algorithm>

namespace phase_vocoder {
template<typename T>
class OverlapExtract {
	std::vector<T> cached;
	const_signal_type<T> signal;
	typename signal_type<T>::index_type head;
	int hop;
	int N;
public:
	OverlapExtract(int N, int hop) : cached(N), head{0}, hop{hop}, N{N} {}

	void add(const_signal_type<T> x) {
		add_(x);
	}

	bool hasNext() {
		return head == N;
	}

	void next(signal_type<T> out) {
		copy<T>(cached, out);
		shift<T>(cached, hop);
		head = N - hop;
		add_(signal);
	}

private:
	void add_(const_signal_type<T> x) {
		auto toFill = leftToFill(x);
		copyToCached(x, toFill);
		assignRemainingSignal(x, toFill);
		addToHead(toFill);
	}

	int leftToFill(const_signal_type<T> x) {
		return std::min(N - head, size(x));
	}

	void copyToCached(const_signal_type<T> x, int n) {
		std::copy(begin(x), begin(x) + n, begin(cached) + head);
	}

	void assignRemainingSignal(const_signal_type<T> x, int used) {
		signal = x.last(size(x) - used);
	}

	void addToHead(int n) {
		head += n;
	}
};
}

#endif
