#include "InterpolateFrames.hpp"
#include <algorithm>
#include <functional>

namespace phase_vocoder {
template <typename T>
InterpolateFrames<T>::InterpolateFrames(int P, int Q, int N)
    : previousFrame(sizeNarrow<complex_type<T>>(N)),
      currentFrame(sizeNarrow<complex_type<T>>(N)),
      accumulatedPhase(sizeNarrow<T>(N)), phaseAdvance(sizeNarrow<T>(N)),
      resampledMagnitude(sizeNarrow<T>(N)), numerator{std::min(P, Q)}, P{P},
      Q{Q} {}

template <typename T>
void InterpolateFrames<T>::add(const_complex_signal_type<T> x) {
    copy(currentFrame, previousFrame);
    copy(x, currentFrame);
    transformFrames(phaseAdvance, &InterpolateFrames::phaseDifference);
    accumulatePhaseIfNeeded();
    if (P == Q || (P > Q && !hasAdded))
        accumulatePhase();
    hasAdded = true;

    updatePhaseAccumulationSkip();

    hasNext_ = true;
    updateHasNext();
}

template <typename T> auto InterpolateFrames<T>::hasNext() -> bool {
    return hasNext_;
}

template <typename T>
void InterpolateFrames<T>::next(complex_signal_type<T> x) {
    resampleMagnitude();
    std::transform(begin(resampledMagnitude), end(resampledMagnitude),
        begin(accumulatedPhase), begin(x),
        [](T magnitude, T phase) { return std::polar(magnitude, phase); });
    accumulatePhaseIfNeeded();
    numerator += P;
    updatePhaseAccumulationSkip();
    updateHasNext();
}

template <typename T> void InterpolateFrames<T>::updatePhaseAccumulationSkip() {
    skipPhaseAccumulation = Q < numerator && numerator < Q + P;
}

template <typename T> void InterpolateFrames<T>::accumulatePhaseIfNeeded() {
    if (numerator != Q && !skipPhaseAccumulation)
        accumulatePhase();
}

template <typename T> void InterpolateFrames<T>::updateHasNext() {
    if (numerator > Q) {
        numerator -= Q;
        hasNext_ = false;
    }
}

template <typename T>
auto InterpolateFrames<T>::phaseDifference(
    const complex_type<T> &a, const complex_type<T> &b) -> T {
    return phase(b) - phase(a);
}

template <typename T>
auto InterpolateFrames<T>::phase(const complex_type<T> &x) -> T {
    return std::arg(x);
}

template <typename T>
auto InterpolateFrames<T>::magnitude(const complex_type<T> &x) -> T {
    return std::abs(x);
}

template <typename T>
void InterpolateFrames<T>::transformFrames(buffer_type<T> &out,
    T (InterpolateFrames::*f)(
        const complex_type<T> &, const complex_type<T> &)) {
    std::function<T(const complex_type<T> &a, const complex_type<T> &b)> f_ =
        [&](auto a, auto b) { return (this->*f)(a, b); };
    phase_vocoder::transform(previousFrame, currentFrame, out, f_);
}

template <typename T>
auto InterpolateFrames<T>::resampleMagnitude(
    const complex_type<T> &a, const complex_type<T> &b) -> T {
    T denominator = Q;
    auto ratio = numerator / denominator;
    return magnitude(a) * (1 - ratio) + magnitude(b) * ratio;
}

template <typename T> void InterpolateFrames<T>::resampleMagnitude() {
    transformFrames(resampledMagnitude, &InterpolateFrames::resampleMagnitude);
}

template <typename T> void InterpolateFrames<T>::accumulatePhase() {
    addFirstToSecond<T>(phaseAdvance, accumulatedPhase);
}

template class InterpolateFrames<double>;
}
