#ifndef PHASEVOCODER_OVERLAPEXTRACT_H
#define PHASEVOCODER_OVERLAPEXTRACT_H

#include <gsl/gsl>
#include <vector>

template<typename T>
class OverlapExtract {
	std::vector<T> signal;
	int head;
	int hop;
	int N;
public:
	OverlapExtract(int N, int hop) : N{ N }, hop{ hop }, head{ 0 } {}

	void add(gsl::span<T> x) {
		signal.insert(signal.end(), x.begin(), x.end());
	}

	bool hasNext() {
		return signal.size() - head >= N;
	}

	gsl::span<T> next() {
		T* head_ = signal.data() + head;
		head += hop;
		return { head_, N };
	}
};

#endif