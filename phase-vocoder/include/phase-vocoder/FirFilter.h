#ifndef PHASEVOCODOER_FIRFILTER_H
#define PHASEVOCODOER_FIRFILTER_H
#include <gsl/gsl>
#include <vector>
#include <algorithm>

template<typename T>
class FirFilter {
	std::vector<T> b;
	std::vector<T> delayLine;
public:
	FirFilter(std::vector<T> b) : 
		b{ std::move(b) },
		delayLine(this->b.size())
	{}
	void filter(gsl::span<T> x) {
		for (auto &x_ : x) {
			delayLine.front() = x_;
			T accumulate{ 0 };
			for (typename std::vector<T>::size_type j{ 0 }; j < b.size(); ++j)
				accumulate += b[j] * delayLine[j];
			x_ = accumulate;
			std::rotate(delayLine.rbegin(), delayLine.rbegin() + 1, delayLine.rend());
		}
	}
};

#endif
