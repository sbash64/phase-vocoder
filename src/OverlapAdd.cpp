#include "OverlapAdd.hpp"

namespace phase_vocoder {
template <typename T>
OverlapAdd<T>::OverlapAdd(int N) : buffer(sizeNarrow<T>(N)) {}

template <typename T> void OverlapAdd<T>::add(const_signal_type<T> x) {
    addFirstToSecond(x, buffer);
}

template <typename T> void OverlapAdd<T>::next(signal_type<T> y) {
    copyFirstToSecond(buffer, y, size(y));
    shift(buffer, size(y));
}

template class OverlapAdd<double>;
template class OverlapAdd<float>;
}
