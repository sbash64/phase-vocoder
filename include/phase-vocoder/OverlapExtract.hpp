#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPEXTRACT_HPP_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPEXTRACT_HPP_

#include "model.hpp"
#include "utility.hpp"

namespace phase_vocoder {
template <typename T> class OverlapExtract {
  public:
    OverlapExtract(index_type N, index_type hop);
    void add(const_signal_type<T> x);
    auto hasNext() -> bool;
    void next(signal_type<T> out);

  private:
    impl::buffer_type<T> buffer;
    const_signal_type<T> onDeck;
    index_type head;
    index_type hop;
    index_type N;
};
}

#endif
