#ifndef PHASEVOCODER_OVERLAPEXTRACT_H
#define PHASEVOCODER_OVERLAPEXTRACT_H
#include <vector>

template<typename T>
class OverlapExtract {
	std::vector<T> signal;
	int head;
	int hop;
	int N;
public:
	OverlapExtract(int N, int hop) : N{ N }, hop{ hop }, head{ 0 } {}

	void add(std::vector<T> x) {
		signal = x;
	}

	std::vector<T> next() {
		auto head_ = signal.begin() + head;
		head += hop;
		return { head_, head_ + N };
	}
};

#endif