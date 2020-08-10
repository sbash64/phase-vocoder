#ifndef SBASH64_PHASEVOCODER_OVERLAPADD_HPP_
#define SBASH64_PHASEVOCODER_OVERLAPADD_HPP_

#include "model.hpp"
#include "utility.hpp"

namespace sbash64::phase_vocoder {
template <typename T> class OverlapAdd {
  public:
    explicit OverlapAdd(index_type N);
    void add(const_signal_type<T> x);
    void next(signal_type<T> y);

  private:
    impl::buffer_type<T> buffer;
};

extern template class OverlapAdd<float>;
extern template class OverlapAdd<double>;
}

#endif
