#ifndef PHASEVOCODER_OVERLAPEXTRACT_H
#define PHASEVOCODER_OVERLAPEXTRACT_H

#include <gsl/gsl>
#include <vector>

template<typename T>
class OverlapExtract {
	std::vector<T> cached_;
	gsl::span<T> signal;
	typename gsl::span<T>::index_type head;
	int hop;
	int N;
public:
	OverlapExtract(int N, int hop) : cached_(N), head{ 0 }, hop{ hop }, N{ N } {}

	void add(gsl::span<T> x) {
		auto toFill = std::min(N - head, x.size());
		std::copy(x.begin(), x.begin() + toFill, cached_.begin() + head);
		signal = x.last(x.size() - toFill);
		head += toFill;
	}

	bool hasNext() {
		return head == N;
	}

	void next(gsl::span<T> out) {
		std::copy(cached_.begin(), cached_.end(), out.begin());
		for (std::size_t i = 0; i < cached_.size() - hop; ++i)
			cached_.at(i) = cached_.at(i + hop);
		head = N - hop;
		auto toFill = std::min(N - head, signal.size());
		std::copy(signal.begin(), signal.begin() + toFill, cached_.begin() + head);
		signal = signal.last(signal.size() - toFill);
		head += toFill;
	}
};

#endif