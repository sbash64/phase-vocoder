#include "OverlapExtract.hpp"
#include <gsl/gsl>
#include <algorithm>

namespace phase_vocoder {
template <typename T>
OverlapExtract<T>::OverlapExtract(index_type N, index_type hop)
    : buffer(N), head{0}, hop{hop}, N{N} {}

template <typename T>
void add(const_signal_type<T> x, index_type N, index_type &head,
    impl::buffer_type<T> &buffer, const_signal_type<T> &onDeck) {
    const auto toFill{std::min(N - head, impl::size(x))};
    std::copy(begin(x), begin(x) + toFill, begin(buffer) + head);
    onDeck = x.last(impl::size(x) - toFill);
    head += toFill;
}

template <typename T> void OverlapExtract<T>::add(const_signal_type<T> x) {
    phase_vocoder::add(x, N, head, buffer, onDeck);
}

template <typename T> auto OverlapExtract<T>::hasNext() -> bool {
    return head == N;
}

template <typename T> void OverlapExtract<T>::next(signal_type<T> out) {
    impl::copyFirstToSecond(buffer, out);
    impl::shiftLeft(buffer, hop);
    head = N - hop;
    phase_vocoder::add(onDeck, N, head, buffer, onDeck);
}

template class OverlapExtract<index_type>;
template class OverlapExtract<float>;
template class OverlapExtract<double>;
}
