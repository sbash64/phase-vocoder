#ifndef PHASEVOCODER_COMMON_UTILITY_H
#define PHASEVOCODER_COMMON_UTILITY_H

#include <gsl/gsl>
#include <algorithm>
#include <functional>

namespace phase_vocoder {
template<typename T>
void shift(gsl::span<T> x, int n) {
    for (typename gsl::span<T>::index_type i{0}; i < x.size() - n; ++i)
        x.at(i) = x.at(i+n);
    std::fill(x.rbegin(), x.rbegin() + n, T{0});
}

template<typename T>
void addFirstToSecond(gsl::span<const T> x, gsl::span<T> y) {
    auto begin_ = y.begin();
    std::transform(
        begin_,
        y.end(),
        x.begin(),
        begin_,
        std::plus<>{}
    );
}

template<typename T>
void copy(gsl::span<const T> source, gsl::span<T> destination) {
    std::copy(source.begin(), source.end(), destination.begin());
}
}

#endif
