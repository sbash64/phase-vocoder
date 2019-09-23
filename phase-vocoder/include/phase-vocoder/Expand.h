#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_EXPAND_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_EXPAND_H_

#include "common-utility.h"
#include <algorithm>

namespace phase_vocoder {
class Expand {
    int P;
public:
    explicit Expand(int P) : P{P} {}

    template<typename T>
    void expand(const_signal_type<T> x, signal_type<T> y) {
        std::fill(begin(y), end(y), T{0});
        for (typename signal_type<T>::index_type i{0}; i < size(x); ++i)
            at(y, i*P) = at(x, i);
    }
};
}

#endif
