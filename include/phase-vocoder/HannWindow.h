#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_HANNWINDOW_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_HANNWINDOW_H_

#include "utility.h"
#include <vector>
#include <algorithm>
#include <cmath>

namespace phase_vocoder {
template<typename T>
T pi() {
    return std::acos(T{-1});
}

template<typename T>
std::vector<T> hannWindow(int N) {
    std::vector<T> window(sizeNarrow<T>(N+1));
    std::generate(
        begin(window),
        end(window),
        [=, n = 0]() mutable {
            return T{0.5} * (1 - std::cos(2 * pi<T>()*n++/N));
    });
    return window;
}
}

#endif