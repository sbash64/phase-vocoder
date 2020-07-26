#include "SignalConverter.hpp"
#include "utility.hpp"

namespace phase_vocoder {
template <typename T>
void SignalConverterImpl<T>::expand(const_signal_type<T> x, signal_type<T> y) {
    auto P = y.size() / x.size();
    zero<T>(begin(y), end(y));
    for (signal_index_type<T> i{0}; i < size(x); ++i)
        element(y, i * P) = element(x, i);
}

template <typename T>
void SignalConverterImpl<T>::decimate(
    const_signal_type<T> x, signal_type<T> y) {
    auto Q = x.size() / y.size();
    for (signal_index_type<T> i{0}; i < size(y); ++i)
        element(y, i) = element(x, i * Q);
}

template class SignalConverterImpl<double>;
template class SignalConverterImpl<float>;
}
