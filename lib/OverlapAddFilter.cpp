#include "OverlapAddFilter.hpp"
#include <gsl/gsl>
#include <algorithm>
#include <functional>

namespace sbash64::phase_vocoder {
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
    return nearestGreaterPowerTwo(impl::size<T>(b));
}

template <typename T>
auto dftBufferLength(const impl::buffer_type<T> &b) -> index_type {
    return N(b) / 2 + 1;
}

template <typename T>
void dft(const std::shared_ptr<FourierTransformer<T>> &transformer,
    signal_type<T> x, complex_signal_type<T> X) {
    transformer->dft(x, X);
}

template <typename T>
OverlapAddFilter<T>::OverlapAddFilter(const impl::buffer_type<T> &b,
    typename FourierTransformer<T>::Factory &factory)
    : overlap{N(b)}, complexBuffer(dftBufferLength(b)), H(dftBufferLength(b)),
      realBuffer(N(b)), transformer{factory.make(N(b))}, L{N(b) -
                                                             impl::size<T>(b) +
                                                             1} {
    impl::copyFirstToSecond<T>(b, realBuffer);
    dft<T>(transformer, realBuffer, H);
}

template <typename T> void OverlapAddFilter<T>::filter(signal_type<T> x) {
    for (index_type j{0}; j < impl::size(x) / L; ++j)
        filter_(x.subspan(j * L, L));
    if (const auto left{impl::size(x) % L})
        filter_(x.last(left));
}

template <typename T> void OverlapAddFilter<T>::filter_(signal_type<T> x) {
    impl::zero<T>(begin(realBuffer) + impl::size(x), end(realBuffer));
    impl::copyFirstToSecond<T>(x, realBuffer);
    dft<T>(transformer, realBuffer, complexBuffer);
    impl::multiplyFirstToSecond<complex_type<T>>(H, complexBuffer);
    transformer->idft(complexBuffer, realBuffer);
    overlap.add(realBuffer);
    overlap.next(x);
}

template class OverlapAddFilter<double>;
template class OverlapAddFilter<float>;
}
