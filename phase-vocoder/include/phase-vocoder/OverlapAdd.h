#ifndef PHASEVOCODER_OVERLAPADD_H
#define PHASEVOCODER_OVERLAPADD_H

#include <gsl/gsl>
#include <vector>
#include <functional>
#include <algorithm>

namespace phase_vocoder {
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
        for (size_t i{0}; i < buffer.size() - hop; ++i)
            buffer.at(i) = buffer.at(i+hop);
        std::fill(buffer.rbegin(), buffer.rbegin() + hop, 0);
    }
};
}

#endif
