#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_SIGNALCONVERTER_HPP_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_SIGNALCONVERTER_HPP_

#include "model.hpp"
#include "utility.hpp"
#include "SampleRateConverter.hpp"

namespace phase_vocoder {
template <typename T> class SignalConverter : public ISignalConverter<T> {
  public:
    // gsl namespace has function called "at".
    // explicit name resolves ambiguous call.
    void expand(const_signal_type<T> x, signal_type<T> y) override {
        auto P = y.size() / x.size();
        zero<T>(begin(y), end(y));
        for (signal_index_type<T> i{0}; i < size(x); ++i)
            phase_vocoder::at(y, i * P) = phase_vocoder::at(x, i);
    }

    void decimate(const_signal_type<T> x, signal_type<T> y) override {
        auto Q = x.size() / y.size();
        for (signal_index_type<T> i{0}; i < size(y); ++i)
            phase_vocoder::at(y, i) = phase_vocoder::at(x, i * Q);
    }
};
}

#endif
