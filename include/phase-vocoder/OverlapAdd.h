#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPADD_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPADD_H_

#include "model.h"
#include "utility.h"

namespace phase_vocoder {
template<typename T>
class OverlapAdd {
    buffer_type<T> buffer;
public:
    explicit OverlapAdd(int N) : buffer(sizeNarrow<T>(N)) {}

    void add(const_signal_type<T> x) {
        addFirstToSecond(x, buffer);
    }

    void next(signal_type<T> y) {
        copy(buffer, y, size(y));
        shift(buffer, size(y));
    }
};
}

#endif
