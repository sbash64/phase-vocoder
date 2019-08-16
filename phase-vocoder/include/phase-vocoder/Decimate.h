#ifndef PHASEVOCODER_DECIMATE_H
#define PHASEVOCODER_DECIMATE_H

#include <gsl/gsl>

namespace phase_vocoder {
class Decimate {
    int Q;
public:
    explicit Decimate(int Q) : Q{Q} {}

    template<typename T>
    void decimate(gsl::span<const T> x, gsl::span<T> y) {
        for (typename gsl::span<T>::index_type i{0}; i < y.size(); ++i)
            y.at(i) = x.at(i*Q);
    }
};
}

#endif