#include "OverlapAddFilter.hpp"
#include <gsl/gsl>
#include <algorithm>
#include <functional>

namespace phase_vocoder {
constexpr auto nearestGreaterPowerTwo(index_type n) -> index_type {
    int power{1};
    while ((n >>= 1) != 0U)
        ++power;
    return 1 << power;
}

template <typename T> void resize(impl::buffer_type<T> &x, index_type n) {
    x.resize(n);
}

template <typename T> auto N(const impl::buffer_type<T> &b) -> index_type {
    return nearestGreaterPowerTwo(impl::size(b));
}

template <typename T>
auto dftBufferLength(const impl::buffer_type<T> &b) -> index_type {
    return N(b) / 2 + 1;
}

template <typename T>
void multiplyFirstToSecond(
    const impl::complex_buffer_type<T> &a, impl::complex_buffer_type<T> &b) {
    std::transform(begin(b), end(b), begin(a), begin(b), std::multiplies<>{});
}

template <typename T>
OverlapAddFilter<T>::OverlapAddFilter(const impl::buffer_type<T> &b,
    typename FourierTransformer<T>::Factory &factory)
    : overlap{N(b)}, complexBuffer(dftBufferLength(b)), H(dftBufferLength(b)),
      realBuffer(N(b)), transformer{factory.make(N(b))}, L{N(b) -
                                                             impl::size(b) +
                                                             1} {
    impl::copyFirstToSecond(b, realBuffer);
    dft(realBuffer, H);
}

template <typename T> void OverlapAddFilter<T>::filter(signal_type<T> x) {
    for (index_type j{0}; j < impl::size(x) / L; ++j)
        filter_(x.subspan(j * L, L));
    if (auto left{impl::size(x) % L})
        filter_(x.last(left));
}

template <typename T>
void OverlapAddFilter<T>::dft(signal_type<T> x, complex_signal_type<T> X) {
    transformer->dft(x, X);
}

template <typename T> void OverlapAddFilter<T>::filter_(signal_type<T> x) {
    impl::zero<T>(begin(realBuffer) + impl::size(x), end(realBuffer));
    impl::copyFirstToSecond<T>(x, realBuffer);
    dft(realBuffer, complexBuffer);
    multiplyFirstToSecond(H, complexBuffer);
    transformer->idft(complexBuffer, realBuffer);
    overlap.add(realBuffer);
    overlap.next(x);
}

template class OverlapAddFilter<double>;
template class OverlapAddFilter<float>;
}
