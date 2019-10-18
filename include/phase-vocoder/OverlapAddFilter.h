#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPADDFILTER_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPADDFILTER_H_

#include "model.h"
#include "utility.h"
#include <gsl/gsl>
#include <vector>
#include <complex>
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

    class Factory {
    public:
        virtual ~Factory() = default;
        virtual std::shared_ptr<FourierTransformer> make(int N) = 0;
    };
};

constexpr int nearestGreaterPowerTwo(std::size_t n) {
    int power{1};
    while (n >>= 1)
        ++power;
    return 1 << power;
}

template<typename T>
void resize(buffer_type<T> &x, size_t n) {
    x.resize(n);
}

template<typename T>
class OverlapAddFilter {
    buffer_type<complex_type<T>> complexBuffer;
    buffer_type<complex_type<T>> H;
    buffer_type<T> realBuffer;
    buffer_type<T> overlap;
    std::shared_ptr<FourierTransformer> transformer_;
    int L;
public:
    OverlapAddFilter(
        const buffer_type<T> &b,
        FourierTransformer::Factory &factory
    ) {
        auto M = size(b);
        auto N = nearestGreaterPowerTwo(M);
        transformer_ = factory.make(N);
        L = N - gsl::narrow_cast<int>(M) + 1;
        resize(realBuffer, sizeNarrow<T>(N));
        copy<T>(b, realBuffer);
        resize(H, sizeNarrow<complex_type<T>>(N));
        resize(overlap, sizeNarrow<T>(N));
        resize(complexBuffer, sizeNarrow<complex_type<T>>(N));
        dft(realBuffer, H);
    }

    void filter(signal_type<T> x) {
        for (signal_index_type<T> j{0}; j < size(x)/L; ++j)
            filter_(x.subspan(j*L, L));
        if (auto left = size(x)%L)
            filter_(x.last(left));
    }

private:
    void dft(signal_type<T> x, complex_signal_type<T> X) {
        transformer_->dft(x, X);
    }

    void filter_(signal_type<T> x) {
        zero<T>(phase_vocoder::begin(realBuffer) + size(x), phase_vocoder::end(realBuffer));
        copy<T>(x, realBuffer);
        dft(realBuffer, complexBuffer);
        std::transform(
            phase_vocoder::begin(complexBuffer),
            phase_vocoder::end(complexBuffer),
            phase_vocoder::begin(H),
            phase_vocoder::begin(complexBuffer),
            std::multiplies<>{}
        );
        transformer_->idft(complexBuffer, realBuffer);
        addFirstToSecond<T>(realBuffer, overlap);
        copy<T>(overlap, x, size(x));
        shift<T>(overlap, size(x));
    }
};
}

#endif
