#include "OverlapAdd.hpp"

namespace sbash64 {
namespace phase_vocoder {
template <typename T> OverlapAdd<T>::OverlapAdd(index_type N) : buffer(N) {}

template <typename T> void OverlapAdd<T>::add(const_signal_type<T> x) {
    impl::addFirstToSecond<T>(x, buffer);
}

template <typename T> void OverlapAdd<T>::next(signal_type<T> y) {
    impl::copyFirstToSecond<T>(buffer, y, impl::size(y));
    impl::shiftLeft<T>(buffer, impl::size(y));
}

template class OverlapAdd<double>;
template class OverlapAdd<float>;
}
}
