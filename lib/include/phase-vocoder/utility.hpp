#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_UTILITY_HPP_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_UTILITY_HPP_

#include "model.hpp"
#include <gsl/gsl>
#include <algorithm>
#include <functional>
#include <vector>

namespace phase_vocoder {
namespace impl {
template <typename T> using buffer_type = std::vector<T>;

template <typename T>
using buffer_iterator_type = typename buffer_type<T>::iterator;

template <typename T>
using buffer_reverse_iterator_type = typename buffer_type<T>::reverse_iterator;

template <typename T> using complex_buffer_type = buffer_type<complex_type<T>>;

template <typename T>
using complex_buffer_iterator_type = typename complex_buffer_type<T>::iterator;

template <typename T>
using const_buffer_iterator_type = typename buffer_type<T>::const_iterator;

template <typename T> auto size(const buffer_type<T> &x) -> index_type {
    return x.size();
}

template <typename T> auto size(const signal_type<T> &x) -> index_type {
    return x.size();
}

template <typename T> auto size(const const_signal_type<T> &x) -> index_type {
    return x.size();
}

template <typename T>
auto element(const signal_type<T> &x, index_type i) -> T & {
    return gsl::at(x, i);
}

template <typename T>
auto element(const const_signal_type<T> &x, index_type i) -> const T & {
    return gsl::at(x, i);
}

template <typename T> auto element(buffer_type<T> &x, index_type i) -> T & {
    return x.at(i);
}

template <typename T>
auto element(const buffer_type<T> &x, index_type i) -> const T & {
    return x.at(i);
}

template <typename T> auto rbegin(buffer_type<T> &x) { return x.rbegin(); }

template <typename T>
void zero(
    signal_reverse_iterator_type<T> b, signal_reverse_iterator_type<T> e) {
    std::fill(b, e, T{0});
}

template <typename T>
void zero(signal_iterator_type<T> b, signal_iterator_type<T> e) {
    std::fill(b, e, T{0});
}

template <typename T>
void zero(buffer_iterator_type<T> b, buffer_iterator_type<T> e) {
    std::fill(b, e, T{0});
}

template <typename T>
void zero(
    buffer_reverse_iterator_type<T> b, buffer_reverse_iterator_type<T> e) {
    std::fill(b, e, T{0});
}

template <typename T> void shiftLeft(buffer_type<T> &x, index_type n) {
    for (index_type i{0}; i < gsl::narrow<index_type>(size(x)) - n; ++i)
        element(x, i) = element(x, i + n);
    zero<T>(rbegin(x), rbegin(x) + n);
}

template <typename T>
void addFirstToSecond(const_signal_type<T> x, signal_type<T> y) {
    std::transform(begin(y), end(y), begin(x), begin(y), std::plus<>{});
}

template <typename T>
void addFirstToSecond(const_signal_type<T> x, buffer_type<T> &y) {
    std::transform(begin(y), end(y), begin(x), begin(y), std::plus<>{});
}

template <typename T>
void addFirstToSecond(const buffer_type<T> &x, buffer_type<T> &y) {
    std::transform(begin(y), end(y), begin(x), begin(y), std::plus<>{});
}

template <typename T>
void copyFirstToSecond(const_signal_iterator_type<T> sourceBegin,
    const_signal_iterator_type<T> sourceEnd,
    signal_iterator_type<T> destination) {
    std::copy(sourceBegin, sourceEnd, destination);
}

template <typename T>
void copyFirstToSecond(const_buffer_iterator_type<T> sourceBegin,
    const_buffer_iterator_type<T> sourceEnd,
    signal_iterator_type<T> destination) {
    std::copy(sourceBegin, sourceEnd, destination);
}

template <typename T>
void copyFirstToSecond(const_buffer_iterator_type<T> sourceBegin,
    const_buffer_iterator_type<T> sourceEnd,
    buffer_iterator_type<T> destination) {
    std::copy(sourceBegin, sourceEnd, destination);
}

template <typename T>
void copyFirstToSecond(const_complex_signal_iterator_type<T> sourceBegin,
    const_complex_signal_iterator_type<T> sourceEnd,
    complex_buffer_iterator_type<T> destination) {
    std::copy(sourceBegin, sourceEnd, destination);
}

template <typename T>
void copyFirstToSecond(
    const_signal_type<T> source, signal_type<T> destination) {
    copyFirstToSecond<T>(begin(source), end(source), begin(destination));
}

template <typename T>
void copyFirstToSecond(
    const buffer_type<T> &source, signal_type<T> destination) {
    copyFirstToSecond<T>(begin(source), end(source), begin(destination));
}

template <typename T>
void copyFirstToSecond(
    const buffer_type<T> &source, buffer_type<T> &destination) {
    copyFirstToSecond<T>(begin(source), end(source), begin(destination));
}

template <typename T>
void copyFirstToSecond(
    const_complex_signal_type<T> source, complex_buffer_type<T> &destination) {
    copyFirstToSecond<T>(begin(source), end(source), begin(destination));
}

template <typename T>
void copyFirstToSecond(
    const_signal_type<T> source, signal_type<T> destination, index_type n) {
    copyFirstToSecond(begin(source), begin(source) + n, begin(destination));
}

template <typename T>
void copyFirstToSecond(
    const buffer_type<T> &source, signal_type<T> destination, index_type n) {
    copyFirstToSecond<T>(begin(source), begin(source) + n, begin(destination));
}

template <typename T>
void transform(const complex_buffer_type<T> &first,
    const complex_buffer_type<T> &second, buffer_type<T> &out,
    std::function<T(const complex_type<T> &, const complex_type<T> &)> f) {
    std::transform(begin(first), end(first), begin(second), begin(out), f);
}
}
}

#endif
