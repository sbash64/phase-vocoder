#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_EXPAND_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_EXPAND_H_

#include "common-utility.h"
#include <gsl/gsl>
#include <algorithm>

namespace phase_vocoder {
class Expand {
    int P;
public:
    explicit Expand(int P) : P{P} {}

    template<typename T>
    void expand(gsl::span<const T> x, gsl::span<T> y) {
        std::fill(begin(y), end(y), T{0});
        for (typename gsl::span<T>::index_type i{0}; i < x.size(); ++i)
            at(y, i*P) = at(x, i);
    }
};
}

#endif
