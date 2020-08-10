#include "SignalConverter.hpp"
#include "utility.hpp"

namespace sbash64::phase_vocoder {
template <typename T>
void SignalConverterImpl<T>::expand(const_signal_type<T> x, signal_type<T> y) {
    const auto P{impl::size(y) / impl::size(x)};
    impl::zero(y);
    for (index_type i{0}; i < impl::size(x); ++i)
        impl::element(y, i * P) = impl::element(x, i);
}

template <typename T>
void SignalConverterImpl<T>::decimate(
    const_signal_type<T> x, signal_type<T> y) {
    const auto Q{impl::size(x) / impl::size(y)};
    for (index_type i{0}; i < impl::size(y); ++i)
        impl::element(y, i) = impl::element(x, i * Q);
}

template class SignalConverterImpl<double>;
template class SignalConverterImpl<float>;
}
