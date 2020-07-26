#include "OverlapExtract.hpp"
#include <algorithm>

namespace phase_vocoder {
template <typename T>
OverlapExtract<T>::OverlapExtract(int N, int hop)
    : buffer(sizeNarrow<T>(N)), head{0}, hop{hop}, N{N} {}

template <typename T>
void add_(const_signal_type<T> x, int N, signal_index_type<T> &head,
    buffer_type<T> &buffer, const_signal_type<T> &onDeck) {
    auto toFill = std::min(N - head, size(x));
    std::copy(begin(x), begin(x) + toFill, begin(buffer) + head);
    onDeck = x.last(size(x) - toFill);
    head += toFill;
}

template <typename T> void OverlapExtract<T>::add(const_signal_type<T> x) {
    add_(x, N, head, buffer, onDeck);
}

template <typename T> auto OverlapExtract<T>::hasNext() -> bool {
    return head == N;
}

template <typename T> void OverlapExtract<T>::next(signal_type<T> out) {
    copyFirstToSecond(buffer, out);
    shiftLeft(buffer, hop);
    head = N - hop;
    add_(onDeck, N, head, buffer, onDeck);
}

template class OverlapExtract<int>;
template class OverlapExtract<float>;
}
