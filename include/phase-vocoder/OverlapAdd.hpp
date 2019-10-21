#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPADD_HPP_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPADD_HPP_

#include "model.hpp"
#include "utility.hpp"

namespace phase_vocoder {
template<typename T>
class OverlapAdd {
public:
    explicit OverlapAdd(int N);
    void add(const_signal_type<T> x);
    void next(signal_type<T> y);
private:
    buffer_type<T> buffer;
};
}

#endif
