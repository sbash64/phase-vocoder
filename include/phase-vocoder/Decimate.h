#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_DECIMATE_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_DECIMATE_H_

#include "model.h"
#include "utility.h"

namespace phase_vocoder {
class Decimate {
public:
    template<typename T>
    void decimate(const_signal_type<T> x, signal_type<T> y, int Q) {
        for (signal_index_type<T> i{0}; i < size(y); ++i)
            // gsl namespace has function called "at".
            // explicit name resolves ambiguous call.
            phase_vocoder::at(y, i) = phase_vocoder::at(x, i*Q);
    }
};
}

#endif
