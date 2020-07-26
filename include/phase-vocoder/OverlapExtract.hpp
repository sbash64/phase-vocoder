#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPEXTRACT_HPP_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPEXTRACT_HPP_

#include "model.hpp"
#include "utility.hpp"

namespace phase_vocoder {
template <typename T> class OverlapExtract {
  public:
    OverlapExtract(int N, int hop);
    void add(const_signal_type<T> x);
    auto hasNext() -> bool;
    void next(signal_type<T> out);

  private:
    buffer_type<T> buffer;
    const_signal_type<T> onDeck;
    signal_index_type<T> head;
    int hop;
    int N;
};
}

#endif
