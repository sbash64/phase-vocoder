#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPADD_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPADD_H_

#include "model.h"
#include "utility.h"
#include <algorithm>

namespace phase_vocoder {
template<typename T>
class OverlapAdd {
    buffer_type<T> buffer;
    int hop;
public:
    OverlapAdd(int N, int hop) : buffer(sizeNarrow<T>(N)), hop{hop} {}

    void add(const_signal_type<T> x) {
        addFirstToSecond<T>(x, buffer);
    }

    void next(signal_type<T> y) {
        std::copy(
            phase_vocoder::begin(buffer),
            phase_vocoder::begin(buffer) + hop,
            phase_vocoder::begin(y)
        );
        shift<T>(buffer, hop);
    }
};
}

#endif
