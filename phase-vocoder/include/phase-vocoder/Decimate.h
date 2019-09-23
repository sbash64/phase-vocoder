#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_DECIMATE_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_DECIMATE_H_

#include "common-utility.h"

namespace phase_vocoder {
class Decimate {
    int Q;
public:
    explicit Decimate(int Q) : Q{Q} {}

    template<typename T>
    void decimate(const_signal_type<T> x, signal_type<T> y) {
        for (typename signal_type<T>::index_type i{0}; i < size(y); ++i)
            at(y, i) = at(x, i*Q);
    }
};
}

#endif
