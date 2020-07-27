#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_PHASEVOCODER_HPP_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_PHASEVOCODER_HPP_

#include "model.hpp"
#include "utility.hpp"
#include "OverlapExtract.hpp"
#include "OverlapAddFilter.hpp"
#include "InterpolateFrames.hpp"
#include "SampleRateConverter.hpp"
#include "SignalConverter.hpp"
#include "OverlapAdd.hpp"
#include "HannWindow.hpp"
#include <memory>
#include <functional>
#include <algorithm>
#include <vector>
#include <iostream>

namespace phase_vocoder {
constexpr auto hop(index_type N) -> index_type { return N / 4; }

template <typename T>
auto lowPassFilter(T cutoff, index_type taps) -> impl::buffer_type<T> {
    impl::buffer_type<T> coefficients(taps);
    std::generate(phase_vocoder::begin(coefficients),
        phase_vocoder::end(coefficients), [=, n = 0]() mutable {
            auto something = (pi<T>() * (n++ - (taps - 1) / 2));
            return something == 0
                ? T{1}
                : std::sin(2 * cutoff * something) / something;
        });

    auto window = hannWindow<T>(taps);
    std::transform(phase_vocoder::begin(coefficients),
        phase_vocoder::end(coefficients), phase_vocoder::begin(window),
        phase_vocoder::begin(coefficients), std::multiplies<>{});

    auto sum = std::accumulate(phase_vocoder::begin(coefficients),
        phase_vocoder::end(coefficients), T{0});
    for (auto &x : coefficients)
        x /= sum;
    return coefficients;
}

template <typename T> class PhaseVocoder {
    InterpolateFrames<T> interpolateFrames;
    OverlapExtract<T> overlapExtract;
    OverlapAddFilter<T> filter;
    OverlapAdd<T> overlappedOutput;
    SignalConverterImpl<T> signalConverter;
    impl::complex_buffer_type<T> nextFrame;
    impl::buffer_type<T> expanded;
    impl::buffer_type<T> decimated;
    impl::buffer_type<T> inputBuffer;
    impl::buffer_type<T> outputBuffer;
    impl::buffer_type<T> window;
    std::shared_ptr<FourierTransformer<T>> transform;
    index_type P;
    index_type Q;
    index_type N;

  public:
    PhaseVocoder(index_type P, index_type Q, index_type N,
        typename FourierTransformer<T>::Factory &factory)
        : interpolateFrames{P, Q, N / 2 + 1}, overlapExtract{N, hop(N)},
          filter{lowPassFilter(T{0.5} / std::max(P, Q), 501), factory},
          overlappedOutput{N}, nextFrame(N / 2 + 1), expanded(hop(N) * P),
          decimated(hop(N) * P / Q), inputBuffer(N),
          outputBuffer(hop(N)), window{hannWindow<T>(N)},
          transform{factory.make(N)}, P{P}, Q{Q}, N{N} {
        impl::buffer_type<T> delayedStart(N - hop(N), T{0});
        overlapExtract.add(delayedStart);
    }

    void vocode(signal_type<T> x) {
        overlapExtract.add(x);
        auto head = x.begin();
        while (overlapExtract.hasNext()) {
            overlapExtract.next(inputBuffer);
            applyWindow();
            transform->dft(inputBuffer, nextFrame);
            interpolateFrames.add(nextFrame);
            while (interpolateFrames.hasNext()) {
                interpolateFrames.next(nextFrame);
                transform->idft(nextFrame, inputBuffer);
                applyWindow();
                overlappedOutput.add(inputBuffer);
                overlappedOutput.next(outputBuffer);
                signalConverter.expand(outputBuffer, expanded);
                filter.filter(expanded);
                signalConverter.decimate(expanded, decimated);
                std::copy(decimated.begin(), decimated.end(), head);
                head += hop(N) * P / Q;
            }
        }
    }

    void applyWindow() {
        std::transform(phase_vocoder::begin(inputBuffer),
            phase_vocoder::end(inputBuffer), phase_vocoder::begin(window),
            phase_vocoder::begin(inputBuffer), std::multiplies<>{});
    }
};
}

#endif
