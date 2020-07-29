#include "InterpolateFrames.hpp"
#include <gsl/gsl>
#include <algorithm>
#include <functional>

namespace phase_vocoder {
template <typename T>
InterpolateFrames<T>::InterpolateFrames(
    index_type P, index_type Q, index_type N)
    : previousFrame(N), currentFrame(N), accumulatedPhase(N), phaseAdvance(N),
      resampledMagnitude(N), numerator{std::min(P, Q)}, P{P}, Q{Q} {
    if (P == 1 && Q == 1) {
        preliminaryPhaseSequenceComplete = true;
        phaseSequence = {true, false};
    } else if (P == 1 && Q == 2) {
        patternPhaseSequence = {true, true, false};
        phaseSequence = {false, true, false};
    } else if (P == 2 && Q == 1) {
        patternPhaseSequence = {true, false, false};
        phaseSequence = {true, false};
    } else if (P == 1 && Q == 3) {
        preliminaryPhaseSequenceComplete = true;
        phaseSequence = {true, true, true, false};
    } else if (P == 3 && Q == 2) {
        preliminaryPhaseSequenceComplete = true;
        phaseSequence = {true, false, true, false, false};
    }
}

template <typename T>
void InterpolateFrames<T>::add(const_complex_signal_type<T> x) {
    impl::copyFirstToSecond<complex_type<T>>(currentFrame, previousFrame);
    impl::copyFirstToSecond<complex_type<T>>(x, currentFrame);
    transformFrames(phaseAdvance, &InterpolateFrames::phaseDifference);
    accumulatePhaseIfNeeded();
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
    if (phaseSequence.at(phaseSequenceHead))
        accumulatePhase();
    if (++phaseSequenceHead == phaseSequence.size()) {
        if (!preliminaryPhaseSequenceComplete) {
            preliminaryPhaseSequenceComplete = true;
            phaseSequence = patternPhaseSequence;
        }
        phaseSequenceHead = 0;
    }
}

template <typename T> void InterpolateFrames<T>::updateHasNext() {
    if (numerator > Q) {
        numerator -= Q;
        hasNext_ = false;
    }
}

template <typename T> auto phase(const complex_type<T> &x) -> T {
    return std::arg(x);
}

template <typename T>
auto InterpolateFrames<T>::phaseDifference(
    const complex_type<T> &a, const complex_type<T> &b) -> T {
    return phase(b) - phase(a);
}

template <typename T> auto magnitude(const complex_type<T> &x) -> T {
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
auto scaledMagnitude(const complex_type<T> &a, T scale) -> T {
    return magnitude(a) * scale;
}

template <typename T>
auto InterpolateFrames<T>::resampleMagnitude(
    const complex_type<T> &a, const complex_type<T> &b) -> T {
    const auto ratio{gsl::narrow_cast<T>(numerator) / gsl::narrow_cast<T>(Q)};
    return scaledMagnitude(a, 1 - ratio) + scaledMagnitude(b, ratio);
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
