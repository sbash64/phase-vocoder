#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_DECIMATE_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_DECIMATE_H_

#include "common-utility.h"
#include <gsl/gsl>

namespace phase_vocoder {
class Decimate {
    int Q;
public:
    explicit Decimate(int Q) : Q{Q} {}

    template<typename T>
    void decimate(gsl::span<const T> x, gsl::span<T> y) {
        for (typename gsl::span<T>::index_type i{0}; i < y.size(); ++i)
            at(y, i) = at(x, i*Q);
    }
};
}

#endif
