#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPADDFILTER_HPP_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPADDFILTER_HPP_

#include "model.hpp"
#include "utility.hpp"
#include "OverlapAdd.hpp"
#include "SampleRateConverter.hpp"
#include <memory>

namespace phase_vocoder {
template <typename T> class FourierTransformer {
  public:
    virtual ~FourierTransformer() = default;
    virtual void dft(signal_type<T>, complex_signal_type<T>) = 0;
    virtual void idft(complex_signal_type<T>, signal_type<T>) = 0;

    class Factory {
      public:
        virtual ~Factory() = default;
        virtual auto make(index_type N)
            -> std::shared_ptr<FourierTransformer> = 0;
    };
};

template <typename T> class OverlapAddFilter : public Filter<T> {
  public:
    OverlapAddFilter(const buffer_type<T> &b,
        typename FourierTransformer<T>::Factory &factory);
    void filter(signal_type<T> x) override;

  private:
    void dft(signal_type<T> x, complex_signal_type<T> X);
    void filter_(signal_type<T> x);

    OverlapAdd<T> overlap;
    complex_buffer_type<T> complexBuffer;
    complex_buffer_type<T> H;
    buffer_type<T> realBuffer;
    std::shared_ptr<FourierTransformer<T>> transformer;
    index_type L;
};

extern template class OverlapAddFilter<float>;
extern template class OverlapAddFilter<double>;
}

#endif
