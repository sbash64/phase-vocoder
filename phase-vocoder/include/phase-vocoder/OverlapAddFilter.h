#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPADDFILTER_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPADDFILTER_H_

#include "common-utility.h"
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
    virtual void dft(gsl::span<double>, gsl::span<std::complex<double>>) = 0;
    virtual void idft(gsl::span<std::complex<double>>, gsl::span<double>) = 0;

    class Factory {
    public:
        virtual ~Factory() = default;
        virtual std::shared_ptr<FourierTransformer> make(int N) = 0;
    };
};

constexpr size_t nearestGreaterPowerTwo(size_t n) {
    size_t power{1};
    while (n >>= 1)
        ++power;
    return 1 << power;
}

template<typename T>
void resize(std::vector<T> &x, size_t n) {
    x.resize(n);
}

template<typename T>
class OverlapAddFilter {
    std::vector<std::complex<T>> complexBuffer;
    std::vector<std::complex<T>> H;
    std::vector<T> realBuffer;
    std::vector<T> overlap;
    std::shared_ptr<FourierTransformer> transformer_;
    size_t N;
    size_t M;
    size_t L;
public:
    OverlapAddFilter(
        const std::vector<T> &b,
        FourierTransformer::Factory &factory
    ) :
        N{nearestGreaterPowerTwo(size(b))},
        M{size(b)}
    {
        transformer_ = factory.make(N);
        L = N - M + 1;
        resize(realBuffer, N);
        copy<T>(b, realBuffer);
        resize(H, N);
        resize(overlap, N);
        resize(complexBuffer, N);
        dft(realBuffer, H);
    }

    void filter(gsl::span<T> x) {
        for (size_t j{0}; j < x.size()/L; ++j)
            filter_(x.subspan(j*L, L));
        if (auto left = x.size()%L)
            filter_(x.last(left));
    }

private:
    void dft(gsl::span<T> x, gsl::span<std::complex<T>> X) {
        transformer_->dft(x, X);
    }

    void filter_(gsl::span<T> x) {
        std::fill(begin(realBuffer) + size(x), end(realBuffer), T{0});
        copy<T>(x, realBuffer);
        dft(realBuffer, complexBuffer);
        std::transform(
            begin(complexBuffer),
            end(complexBuffer),
            begin(H),
            begin(complexBuffer),
            std::multiplies<>{}
        );
        transformer_->idft(complexBuffer, realBuffer);
        addFirstToSecond<T>(realBuffer, overlap);
        std::copy(begin(overlap), begin(overlap) + size(x), begin(x));
        shift<T>(overlap, size(x));
    }
};
}

#endif
