#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_SIGNALCONVERTER_HPP_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_SIGNALCONVERTER_HPP_

#include "model.hpp"
#include "SampleRateConverter.hpp"

namespace phase_vocoder {
template <typename T> class SignalConverterImpl : public SignalConverter<T> {
  public:
    void expand(const_signal_type<T> x, signal_type<T> y) override;
    void decimate(const_signal_type<T> x, signal_type<T> y) override;
};
}

#endif
