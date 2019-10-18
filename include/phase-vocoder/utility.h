#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_UTILITY_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_UTILITY_H_

#include "model.h"
#include <gsl/gsl>
#include <algorithm>
#include <functional>
#include <vector>

namespace phase_vocoder {
template<typename T>
auto size(const signal_type<T> &x) {
    return x.size();
}

template<typename T>
auto &at(const signal_type<T> &x, signal_index_type<T> i) {
    return x.at(i);
}

template<typename T>
auto rbegin(const signal_type<T> &x) {
    return x.rbegin();
}

template<typename T>
void zero(
    signal_reverse_iterator_type<T> b,
    signal_reverse_iterator_type<T> e
) {
    std::fill(b, e, T{0});
}

template<typename T>
void zero(
    signal_iterator_type<T> b,
    signal_iterator_type<T> e
) {
    std::fill(b, e, T{0});
}

template<typename T>
void shift(signal_type<T> x, signal_index_type<T> n) {
    for (signal_index_type<T> i{0}; i < size(x) - n; ++i)
        // gsl namespace has function called "at".
        // explicit name resolves ambiguous call.
        phase_vocoder::at(x, i) = phase_vocoder::at(x, i+n);
    zero<T>(rbegin(x), rbegin(x) + n);
}

template<typename T>
auto begin(const signal_type<T> &x) {
    return x.begin();
}

template<typename T>
auto end(const signal_type<T> &x) {
    return x.end();
}

template<typename T>
void addFirstToSecond(const_signal_type<T> x, signal_type<T> y) {
    std::transform(
        begin(y),
        end(y),
        begin(x),
        begin(y),
        std::plus<>{}
    );
}

template<typename T>
void copy(
    const_signal_iterator_type<T> sourceBegin,
    const_signal_iterator_type<T> sourceEnd,
    signal_iterator_type<T> destination
) {
    std::copy(sourceBegin, sourceEnd, destination);
}

template<typename T>
void copy(const_signal_type<T> source, signal_type<T> destination) {
    copy<T>(begin(source), end(source), begin(destination));
}

template<typename T>
void copy(
    const_signal_type<T> source,
    signal_type<T> destination,
    signal_size_type<T> n
) {
    copy<T>(begin(source), begin(source) + n, begin(destination));
}

template<typename T>
constexpr auto sizeNarrow(int x) {
	return gsl::narrow_cast<typename std::vector<T>::size_type>(x);
}
}

#endif
