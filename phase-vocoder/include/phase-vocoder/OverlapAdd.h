#ifndef PHASEVOCODER_OVERLAPADD_H
#define PHASEVOCODER_OVERLAPADD_H

#include <gsl/gsl>
#include <vector>
#include <functional>
#include <algorithm>

namespace phase_vocoder {
template<typename T>
class OverlapAdd {
    std::vector<T> overlap_;
    std::vector<T> next_;
    int hop;
public:
    OverlapAdd(int N, int hop) : overlap_(N), next_(hop), hop{hop} {}

    void add(gsl::span<const T> x) {
        std::copy(x.rbegin(), x.rbegin() + hop, next_.rbegin());
        auto begin_ = overlap_.begin() + hop;
        std::transform(
            begin_,
            overlap_.end(),
            x.begin(),
            begin_,
            std::plus<>{}
        );
    }

    void next(gsl::span<T> y) {
        std::copy(overlap_.begin(), overlap_.end(), y.begin());
        for (size_t i{0}; i < overlap_.size() - hop; ++i)
            overlap_.at(i) = overlap_.at(i+hop);
        std::copy(next_.rbegin(), next_.rend(), overlap_.rbegin());
    }
};
}

#endif
