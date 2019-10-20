#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_SIGNALCONVERTER_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_SIGNALCONVERTER_H_

#include "model.h"
#include "utility.h"

namespace phase_vocoder {
class SignalConverter {
public:
    // gsl namespace has function called "at".
    // explicit name resolves ambiguous call.
    template<typename T>
    void expand(const_signal_type<T> x, signal_type<T> y) {
        auto P = y.size() / x.size();
        zero<T>(begin(y), end(y));
        for (signal_index_type<T> i{0}; i < size(x); ++i)
            phase_vocoder::at(y, i*P) = phase_vocoder::at(x, i);
    }

    template<typename T>
    void decimate(const_signal_type<T> x, signal_type<T> y) {
        auto Q = x.size() / y.size();
        for (signal_index_type<T> i{0}; i < size(y); ++i)
            phase_vocoder::at(y, i) = phase_vocoder::at(x, i*Q);
    }
};
}

#endif