#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_EXPAND_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_EXPAND_H_

#include "model.h"
#include "utility.h"
#include <algorithm>

namespace phase_vocoder {
class Expand {
    int P;
public:
    explicit Expand(int P) : P{P} {}

    template<typename T>
    void expand(const_signal_type<T> x, signal_type<T> y) {
        zero<T>(begin(y), end(y));
        for (signal_index_type<T> i{0}; i < size(x); ++i)
            // gsl namespace has function called "at".
            // explicit name resolves ambiguous call.
            phase_vocoder::at(y, i*P) = phase_vocoder::at(x, i);
    }
};
}

#endif
