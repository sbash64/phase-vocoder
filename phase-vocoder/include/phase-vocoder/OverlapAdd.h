#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPADD_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_OVERLAPADD_H_

#include "common-utility.h"
#include <gsl/gsl>
#include <vector>
#include <algorithm>

namespace phase_vocoder {
template<typename T>
class OverlapAdd {
    std::vector<T> buffer;
    int hop;
public:
    OverlapAdd(int N, int hop) : buffer(N), hop{hop} {}

    void add(gsl::span<const T> x) {
        addFirstToSecond<T>(x, buffer);
    }

    void next(gsl::span<T> y) {
        std::copy(begin(buffer), begin(buffer) + hop, begin(y));
        shift<T>(buffer, hop);
    }
};
}

#endif
