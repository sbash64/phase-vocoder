#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_COMMON_UTILITY_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_COMMON_UTILITY_H_

#include <gsl/gsl>
#include <algorithm>
#include <functional>

namespace phase_vocoder {
template<typename T>
auto size(const gsl::span<T> &x) {
    return x.size();
}

template<typename T>
auto at(const gsl::span<T> &x, size_t i) {
    return x.at(i);
}

template<typename T>
auto rbegin(const gsl::span<T> &x) {
    return x.rbegin();
}

template<typename T>
void shift(gsl::span<T> x, int n) {
    for (typename gsl::span<T>::index_type i{0}; i < size(x) - n; ++i)
        at(x, i) = at(x, i+n);
    std::fill(rbegin(x), rbegin(x) + n, T{0});
}

template<typename T>
auto begin(const gsl::span<T> &x) {
    return x.begin();
}

template<typename T>
auto end(const gsl::span<T> &x) {
    return x.end();
}

template<typename T>
void addFirstToSecond(gsl::span<const T> x, gsl::span<T> y) {
    std::transform(
        begin(y),
        end(y),
        begin(x),
        begin(y),
        std::plus<>{}
    );
}

template<typename T>
void copy(gsl::span<const T> source, gsl::span<T> destination) {
    std::copy(begin(source), end(source), begin(destination));
}
}

#endif
