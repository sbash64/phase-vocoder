#include "OverlapExtract.hpp"
#include <algorithm>

namespace phase_vocoder {
template<typename T>
OverlapExtract<T>::OverlapExtract(int N, int hop) :
	cached(sizeNarrow<T>(N)),
	head{0},
	hop{hop},
	N{N} {}

template<typename T>
void add_(
    const_signal_type<T> x,
    int N,
    signal_index_type<T> &head,
    buffer_type<T> &cached,
    const_signal_type<T> &signal
) {
    auto toFill = std::min(N - head, size(x));
    std::copy(
        begin(x),
        begin(x) + toFill,
        begin(cached) + head
    );
    signal = x.last(size(x) - toFill);
    head += toFill;
}

template<typename T>
void OverlapExtract<T>::add(const_signal_type<T> x) {
    add_(x, N, head, cached, signal);
}

template<typename T>
bool OverlapExtract<T>::hasNext() {
    return head == N;
}

template<typename T>
void OverlapExtract<T>::next(signal_type<T> out) {
    copy(cached, out);
    shift(cached, hop);
    head = N - hop;
    add_(signal, N, head, cached, signal);
}

template class OverlapExtract<int>;
}
