#include "OverlapAddFilter.hpp"
#include <gsl/gsl>
#include <algorithm>
#include <functional>

namespace phase_vocoder {
constexpr auto nearestGreaterPowerTwo(std::size_t n) -> index_type {
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
void multiplyFirstToSecond(
    const complex_buffer_type<T> &a, complex_buffer_type<T> &b) {
    std::transform(begin(b), end(b), begin(a), begin(b), std::multiplies<>{});
}

template <typename T>
OverlapAddFilter<T>::OverlapAddFilter(
    const buffer_type<T> &b, typename FourierTransformer<T>::Factory &factory)
    : overlap{N(b)}, complexBuffer(N(b) / 2 + 1), H(N(b) / 2 + 1),
      realBuffer(N(b)), transformer{factory.make(N(b))}, L{N(b) - size(b) + 1} {
    copyFirstToSecond(b, realBuffer);
    dft(realBuffer, H);
}

template <typename T> void OverlapAddFilter<T>::filter(signal_type<T> x) {
    for (index_type j{0}; j < size(x) / L; ++j)
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
    multiplyFirstToSecond(H, complexBuffer);
    transformer->idft(complexBuffer, realBuffer);
    overlap.add(realBuffer);
    overlap.next(x);
}

template class OverlapAddFilter<double>;
template class OverlapAddFilter<float>;
}
