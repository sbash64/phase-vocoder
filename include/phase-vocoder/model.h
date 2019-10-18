#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_MODEL_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_MODEL_H_

#include <gsl/gsl>
#include <complex>

namespace phase_vocoder {
template<typename T>
using signal_type = gsl::span<T>;

template<typename T>
using const_signal_type = signal_type<const T>;

template<typename T>
using signal_index_type = typename signal_type<T>::index_type;

template<typename T>
using signal_iterator_type = typename signal_type<T>::iterator;

template<typename T>
using const_signal_iterator_type = typename const_signal_type<T>::iterator;

template<typename T>
using signal_size_type = typename signal_type<T>::size_type;

template<typename T>
using complex_type = std::complex<T>;
}

#endif
