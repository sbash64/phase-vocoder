#ifndef PHASEVOCODER_OVERLAPADD_H
#define PHASEVOCODER_OVERLAPADD_H

#include "common-utility.h"
#include <gsl/gsl>
#include <vector>
#include <functional>
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
        auto begin_ = buffer.begin();
        std::copy(begin_, begin_ + hop, y.begin());
        shift<T>(buffer, hop);
    }
};
}

#endif
