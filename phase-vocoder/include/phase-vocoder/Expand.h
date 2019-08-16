#ifndef PHASEVOCODER_EXPAND_H
#define PHASEVOCODER_EXPAND_H

#include <gsl/gsl>

namespace phase_vocoder {
class Expand {
    int P;
public:
    explicit Expand(int P) : P{P} {}

    template<typename T>
    void expand(gsl::span<const T> x, gsl::span<T> y) {
        std::fill(y.begin(), y.end(), 0);
        for (typename gsl::span<T>::index_type i{0}; i < x.size(); ++i)
            y.at(i*P) = x.at(i);
    }
};
}

#endif
