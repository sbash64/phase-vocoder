#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_INTERPOLATEFRAMES_HPP_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_INTERPOLATEFRAMES_HPP_

#include "model.hpp"
#include "utility.hpp"
#include <vector>

namespace phase_vocoder {
template <typename T> class InterpolateFrames {
  public:
    InterpolateFrames(index_type P, index_type Q, index_type N);
    void add(const_complex_signal_type<T> x);
    auto hasNext() -> bool;
    void next(complex_signal_type<T> x);

  private:
    void updatePhaseAccumulationSkip();
    void accumulatePhaseIfNeeded();
    void updateHasNext();
    auto phaseDifference(const complex_type<T> &a, const complex_type<T> &b)
        -> T;
    void transformFrames(impl::buffer_type<T> &out,
        T (InterpolateFrames::*f)(
            const complex_type<T> &, const complex_type<T> &));
    auto resampleMagnitude(const complex_type<T> &a, const complex_type<T> &b)
        -> T;
    void resampleMagnitude();
    void accumulatePhase();

    using frame_type = impl::complex_buffer_type<T>;
    frame_type previousFrame;
    frame_type currentFrame;
    impl::buffer_type<T> accumulatedPhase;
    impl::buffer_type<T> phaseAdvance;
    impl::buffer_type<T> resampledMagnitude;
    std::vector<bool> patternPhaseSequence;
    std::vector<bool> phaseSequence;
    index_type phaseSequenceHead{0};
    index_type numerator;
    index_type P;
    index_type Q;
    bool hasNext_{};
    bool preliminaryPhaseSequenceComplete{};
};

extern template class InterpolateFrames<float>;
extern template class InterpolateFrames<double>;
}

#endif
