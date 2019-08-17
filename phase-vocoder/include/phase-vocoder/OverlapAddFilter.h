#ifndef PHASEVOCODER_OVERLAPADDFILTER_H
#define PHASEVOCODER_OVERLAPADDFILTER_H

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
        std::vector<T> b,
        FourierTransformer::Factory &factory
    ) :
        N{nearestGreaterPowerTwo(b.size())},
        M{b.size()}
    {
        transformer_ = factory.make(N);
        L = N - M + 1;
        b.resize(N);
        realBuffer.resize(N);
        H.resize(N);
        overlap.resize(N);
        complexBuffer.resize(N);
        dft(b, H);
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
        std::fill(realBuffer.begin() + x.size(), realBuffer.end(), 0);
        std::copy(x.begin(), x.end(), realBuffer.begin());
        dft(realBuffer, complexBuffer);
        std::transform(
            complexBuffer.begin(),
            complexBuffer.end(),
            H.begin(),
            complexBuffer.begin(),
            std::multiplies<>{}
        );
        transformer_->idft(complexBuffer, realBuffer);
        std::transform(
            overlap.begin(),
            overlap.end(),
            realBuffer.begin(),
            overlap.begin(),
            std::plus<>{}
        );
        std::copy(overlap.begin(), overlap.begin() + x.size(), x.begin());
        for (size_t i{0}; i < N - x.size(); ++i)
            overlap.at(i) = overlap.at(i+x.size());
        std::fill(overlap.rbegin(), overlap.rbegin() + x.size(), 0);
    }
};
}

#endif
