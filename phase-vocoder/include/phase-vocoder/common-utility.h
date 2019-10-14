#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_COMMON_UTILITY_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_COMMON_UTILITY_H_

#include <gsl/gsl>
#include <algorithm>
#include <functional>

namespace phase_vocoder {
template<typename T>
using signal_type = gsl::span<T>;

template<typename T>
using const_signal_type = signal_type<const T>;

template<typename T>
using signal_index_type = typename signal_type<T>::index_type;

template<typename T>
auto size(const signal_type<T> &x) {
    return x.size();
}

template<typename T>
auto at(const signal_type<T> &x, size_t i) {
    return x.at(i);
}

template<typename T>
auto rbegin(const signal_type<T> &x) {
    return x.rbegin();
}

template<typename T>
void shift(signal_type<T> x, signal_index_type<T> n) {
    for (signal_index_type<T> i{0}; i < size(x) - n; ++i)
        at(x, i) = at(x, i+n);
    std::fill(rbegin(x), rbegin(x) + n, T{0});
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
using const_signal_iterator_type = typename const_signal_type<T>::iterator;

template<typename T>
using signal_iterator_type = typename signal_type<T>::iterator;

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
void copy(const_signal_type<T> source, signal_type<T> destination, typename signal_type<T>::size_type n) {
    copy<T>(begin(source), begin(source) + n, begin(destination));
}
}

#endif
