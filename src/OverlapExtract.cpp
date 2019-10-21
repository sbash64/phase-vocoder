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
void OverlapExtract<T>::add(const_signal_type<T> x) {
    add_(x);
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
    add_(signal);
}

template<typename T>
void OverlapExtract<T>::add_(const_signal_type<T> x) {
    auto toFill = leftToFill(x);
    copyToCached(x, toFill);
    assignRemainingSignal(x, toFill);
    addToHead(toFill);
}

template<typename T>
signal_size_type<T> OverlapExtract<T>::leftToFill(const_signal_type<T> x) {
    return std::min(N - head, size(x));
}

template<typename T>
void OverlapExtract<T>::copyToCached(const_signal_type<T> x, signal_size_type<T> n) {
    std::copy(
        begin(x),
        begin(x) + n,
        begin(cached) + head
    );
}

template<typename T>
void OverlapExtract<T>::assignRemainingSignal(const_signal_type<T> x, signal_size_type<T> used) {
    signal = x.last(size(x) - used);
}

template<typename T>
void OverlapExtract<T>::addToHead(signal_size_type<T> n) {
    head += n;
}

template class OverlapExtract<int>;
}
