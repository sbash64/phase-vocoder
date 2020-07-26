#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_INTERPOLATEFRAMES_HPP_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_INTERPOLATEFRAMES_HPP_

#include "model.hpp"
#include "utility.hpp"

namespace phase_vocoder {
template <typename T> class InterpolateFrames {
  public:
    InterpolateFrames(int P, int Q, int N);
    void add(const_complex_signal_type<T> x);
    auto hasNext() -> bool;
    void next(complex_signal_type<T> x);

  private:
    void updatePhaseAccumulationSkip();
    void accumulatePhaseIfNeeded();
    void updateHasNext();
    auto phaseDifference(const complex_type<T> &a, const complex_type<T> &b)
        -> T;
    auto phase(const complex_type<T> &x) -> T;
    auto magnitude(const complex_type<T> &x) -> T;
    void transformFrames(buffer_type<T> &out,
        T (InterpolateFrames::*f)(
            const complex_type<T> &, const complex_type<T> &));
    auto resampleMagnitude(const complex_type<T> &a, const complex_type<T> &b)
        -> T;
    void resampleMagnitude();
    void accumulatePhase();

    using frame_type = complex_buffer_type<T>;
    frame_type previousFrame;
    frame_type currentFrame;
    buffer_type<T> accumulatedPhase;
    buffer_type<T> phaseAdvance;
    buffer_type<T> resampledMagnitude;
    int numerator;
    int P;
    int Q;
    bool hasNext_{true};
    bool hasAdded{};
    bool skipPhaseAccumulation{};
};
}

#endif
