#include "OverlapAddFilter.hpp"
#include <gsl/gsl>
#include <algorithm>
#include <functional>

namespace phase_vocoder {
constexpr auto nearestGreaterPowerTwo(std::size_t n) -> int {
    int power{1};
    while ((n >>= 1) != 0U)
        ++power;
    return 1 << power;
}

template <typename T> void resize(buffer_type<T> &x, std::size_t n) {
    x.resize(n);
}

template <typename T> auto N(const buffer_type<T> &b) -> int {
    return nearestGreaterPowerTwo(size(b));
}

template <typename T>
OverlapAddFilter<T>::OverlapAddFilter(
    const buffer_type<T> &b, typename FourierTransformer<T>::Factory &factory)
    : overlap{N(b)}, complexBuffer(sizeNarrow<complex_type<T>>(N(b))),
      H(sizeNarrow<complex_type<T>>(N(b))), realBuffer(sizeNarrow<T>(N(b))),
      transformer{factory.make(N(b))}, L{N(b) - gsl::narrow_cast<int>(size(b)) +
                                           1} {
    copyFirstToSecond(b, realBuffer);
    dft(realBuffer, H);
}

template <typename T> void OverlapAddFilter<T>::filter(signal_type<T> x) {
    for (signal_index_type<T> j{0}; j < size(x) / L; ++j)
        filter_(x.subspan(j * L, L));
    if (auto left = size(x) % L)
        filter_(x.last(left));
}

template <typename T>
void OverlapAddFilter<T>::dft(signal_type<T> x, complex_signal_type<T> X) {
    transformer->dft(x, X);
}

template <typename T> void OverlapAddFilter<T>::filter_(signal_type<T> x) {
    zero<T>(begin(realBuffer) + size(x), end(realBuffer));
    copyFirstToSecond<T>(x, realBuffer);
    dft(realBuffer, complexBuffer);
    std::transform(begin(complexBuffer), end(complexBuffer), begin(H),
        begin(complexBuffer), std::multiplies<>{});
    transformer->idft(complexBuffer, realBuffer);
    overlap.add(realBuffer);
    overlap.next(x);
}

template class OverlapAddFilter<double>;
}
