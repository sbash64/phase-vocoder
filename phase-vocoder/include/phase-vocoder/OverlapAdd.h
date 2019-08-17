#ifndef PHASEVOCODER_OVERLAPADD_H
#define PHASEVOCODER_OVERLAPADD_H

#include <gsl/gsl>
#include <vector>
#include <functional>
#include <algorithm>

namespace phase_vocoder {
template<typename T>
void shift(gsl::span<T> x, int n) {
    for (typename gsl::span<T>::index_type i{0}; i < x.size() - n; ++i)
        x.at(i) = x.at(i+n);
    std::fill(x.rbegin(), x.rbegin() + n, T{0});
}

template<typename T>
class OverlapAdd {
    std::vector<T> buffer;
    int hop;
public:
    OverlapAdd(int N, int hop) : buffer(N), hop{hop} {}

    void add(gsl::span<const T> x) {
        auto begin_ = buffer.begin();
        std::transform(
            begin_,
            buffer.end(),
            x.begin(),
            begin_,
            std::plus<>{}
        );
    }

    void next(gsl::span<T> y) {
        auto begin_ = buffer.begin();
        std::copy(begin_, begin_ + hop, y.begin());
        shift<T>(buffer, hop);
    }
};
}

#endif
