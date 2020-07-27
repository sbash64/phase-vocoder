#include "InterpolateFrames.hpp"
#include <gsl/gsl>
#include <algorithm>
#include <functional>

namespace phase_vocoder {
template <typename T>
InterpolateFrames<T>::InterpolateFrames(
    index_type P, index_type Q, index_type N)
    : previousFrame(N), currentFrame(N), accumulatedPhase(N), phaseAdvance(N),
      resampledMagnitude(N), numerator{std::min(P, Q)}, P{P}, Q{Q} {}

template <typename T>
void InterpolateFrames<T>::add(const_complex_signal_type<T> x) {
    impl::copyFirstToSecond(currentFrame, previousFrame);
    impl::copyFirstToSecond(x, currentFrame);
    transformFrames(phaseAdvance, &InterpolateFrames::phaseDifference);
    if (P >= Q || hasAdded)
        accumulatePhase();
    hasAdded = true;
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
        begin(accumulatedPhase), begin(x), std::polar<T>);
    accumulatePhaseIfNeeded();
    numerator += P;
    updateHasNext();
}

template <typename T> void InterpolateFrames<T>::accumulatePhaseIfNeeded() {
    if (numerator != Q)
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
void InterpolateFrames<T>::transformFrames(impl::buffer_type<T> &out,
    T (InterpolateFrames::*f)(
        const complex_type<T> &, const complex_type<T> &)) {
    std::function<T(const complex_type<T> &a, const complex_type<T> &b)> f_ =
        [&](auto a, auto b) { return (this->*f)(a, b); };
    impl::transform(previousFrame, currentFrame, out, f_);
}

template <typename T>
auto InterpolateFrames<T>::resampleMagnitude(
    const complex_type<T> &a, const complex_type<T> &b) -> T {
    const auto ratio{gsl::narrow_cast<T>(numerator) / gsl::narrow_cast<T>(Q)};
    return magnitude(a) * (1 - ratio) + magnitude(b) * ratio;
}

template <typename T> void InterpolateFrames<T>::resampleMagnitude() {
    transformFrames(resampledMagnitude, &InterpolateFrames::resampleMagnitude);
}

template <typename T> void InterpolateFrames<T>::accumulatePhase() {
    impl::addFirstToSecond<T>(phaseAdvance, accumulatedPhase);
}

template class InterpolateFrames<double>;
template class InterpolateFrames<float>;
}
