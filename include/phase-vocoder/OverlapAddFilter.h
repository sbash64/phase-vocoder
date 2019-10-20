#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPADDFILTER_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPADDFILTER_H_

#include "model.h"
#include "utility.h"
#include "OverlapAdd.h"
#include <gsl/gsl>
#include <algorithm>
#include <functional>
#include <memory>

namespace phase_vocoder {
class FourierTransformer {
public:
    virtual ~FourierTransformer() = default;
    virtual void dft(
        signal_type<double>,
        complex_signal_type<double>
    ) = 0;
    virtual void idft(
        complex_signal_type<double>,
        signal_type<double>
    ) = 0;
    virtual void dft(
        signal_type<float>,
        complex_signal_type<float>
    ) = 0;
    virtual void idft(
        complex_signal_type<float>,
        signal_type<float>
    ) = 0;

    class Factory {
    public:
        virtual ~Factory() = default;
        virtual std::shared_ptr<FourierTransformer> make(int N) = 0;
    };
};

template<typename T>
class OverlapAddFilter {
public:
    OverlapAddFilter(
        const buffer_type<T> &b,
        FourierTransformer::Factory &factory
    );
    void filter(signal_type<T> x);

private:
    void dft(signal_type<T> x, complex_signal_type<T> X);
    void filter_(signal_type<T> x);

    OverlapAdd<T> overlap;
    buffer_type<complex_type<T>> complexBuffer;
    buffer_type<complex_type<T>> H;
    buffer_type<T> realBuffer;
    std::shared_ptr<FourierTransformer> transformer;
    int L;
};
}

#endif
