#include "OverlapAddFilter.h"
#include <gsl/gsl>
#include <algorithm>
#include <functional>

namespace phase_vocoder {
constexpr int nearestGreaterPowerTwo(std::size_t n) {
    int power{1};
    while (n >>= 1)
        ++power;
    return 1 << power;
}

template<typename T>
void resize(buffer_type<T> &x, std::size_t n) {
    x.resize(n);
}

template<typename T>
OverlapAddFilter<T>::OverlapAddFilter(
    const buffer_type<T> &b,
    typename FourierTransformer<T>::Factory &factory
) :
    overlap{nearestGreaterPowerTwo(size(b))}
{
    auto M = size(b);
    auto N = nearestGreaterPowerTwo(M);
    transformer = factory.make(N);
    L = N - gsl::narrow_cast<int>(M) + 1;
    resize(realBuffer, sizeNarrow<T>(N));
    copy(b, realBuffer);
    resize(H, sizeNarrow<complex_type<T>>(N));
    resize(complexBuffer, sizeNarrow<complex_type<T>>(N));
    dft(realBuffer, H);
}

template<typename T>
void OverlapAddFilter<T>::filter(signal_type<T> x) {
    for (signal_index_type<T> j{0}; j < size(x)/L; ++j)
        filter_(x.subspan(j*L, L));
    if (auto left = size(x)%L)
        filter_(x.last(left));
}

template<typename T>
void OverlapAddFilter<T>::dft(signal_type<T> x, complex_signal_type<T> X) {
    transformer->dft(x, X);
}

template<typename T>
void OverlapAddFilter<T>::filter_(signal_type<T> x) {
    zero<T>(
        begin(realBuffer) + size(x),
        end(realBuffer)
    );
    copy<T>(x, realBuffer);
    dft(realBuffer, complexBuffer);
    std::transform(
        begin(complexBuffer),
        end(complexBuffer),
        begin(H),
        begin(complexBuffer),
        std::multiplies<>{}
    );
    transformer->idft(complexBuffer, realBuffer);
    overlap.add(realBuffer);
    overlap.next(x);
}

template class OverlapAddFilter<double>;
}
