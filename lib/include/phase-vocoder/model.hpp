#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_MODEL_HPP_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_MODEL_HPP_

#include <gsl/gsl>
#include <complex>

namespace phase_vocoder {
using index_type = typename gsl::index;

template <typename T> using signal_type = gsl::span<T>;

template <typename T> using const_signal_type = signal_type<const T>;

template <typename T>
using signal_iterator_type = typename signal_type<T>::iterator;

template <typename T>
using const_signal_iterator_type = typename const_signal_type<T>::iterator;

template <typename T>
using signal_reverse_iterator_type = typename signal_type<T>::reverse_iterator;

template <typename T> using complex_type = std::complex<T>;

template <typename T> using complex_signal_type = signal_type<complex_type<T>>;

template <typename T>
using const_complex_signal_type = const_signal_type<complex_type<T>>;

template <typename T>
using complex_signal_iterator_type = typename complex_signal_type<T>::iterator;

template <typename T>
using const_complex_signal_iterator_type =
    typename const_complex_signal_type<T>::iterator;
}

#endif
