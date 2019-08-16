#ifndef PHASEVOCODER_OVERLAPEXTRACT_H
#define PHASEVOCODER_OVERLAPEXTRACT_H

#include <gsl/gsl>
#include <vector>
#include <algorithm>

namespace phase_vocoder {
template<typename T>
class OverlapExtract {
	std::vector<T> cached;
	gsl::span<T> signal;
	typename gsl::span<T>::index_type head;
	int hop;
	int N;
public:
	OverlapExtract(int N, int hop) : cached(N), head{ 0 }, hop{ hop }, N{ N } {}

	void add(gsl::span<T> x) {
		auto toFill = std::min(N - head, x.size());
		std::copy(x.begin(), x.begin() + toFill, cached.begin() + head);
		signal = x.last(x.size() - toFill);
		head += toFill;
	}

	bool hasNext() {
		return head == N;
	}

	void next(gsl::span<T> out) {
		std::copy(cached.begin(), cached.end(), out.begin());
		for (std::size_t i = 0; i < cached.size() - hop; ++i)
			cached.at(i) = cached.at(i + hop);
		head = N - hop;
		auto toFill = std::min(N - head, signal.size());
		std::copy(signal.begin(), signal.begin() + toFill, cached.begin() + head);
		signal = signal.last(signal.size() - toFill);
		head += toFill;
	}
};
}

#endif
