#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPEXTRACT_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPEXTRACT_H_

#include "model.h"
#include <vector>
#include <algorithm>

namespace phase_vocoder {
template<typename T>
class OverlapExtract {
	using signal_size_type = typename const_signal_type<T>::size_type;
	std::vector<T> cached;
	const_signal_type<T> signal;
	typename signal_type<T>::index_type head;
	int hop;
	int N;
public:
	OverlapExtract(int N, int hop) :
		cached(sizeNarrow<T>(N)),
		head{0},
		hop{hop},
		N{N} {}

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

	signal_size_type leftToFill(const_signal_type<T> x) {
		return std::min(N - head, size(x));
	}

	void copyToCached(const_signal_type<T> x, signal_size_type n) {
		std::copy(begin(x), begin(x) + n, begin(cached) + head);
	}

	void assignRemainingSignal(const_signal_type<T> x, signal_size_type used) {
		signal = x.last(size(x) - used);
	}

	void addToHead(signal_size_type n) {
		head += n;
	}
};
}

#endif
