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
constexpr auto hop(int N) -> int { return N / 4; }

template <typename T> auto lowPassFilter(T cutoff, int taps) -> buffer_type<T> {
    buffer_type<T> coefficients(sizeNarrow<T>(taps));
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
    complex_buffer_type<T> nextFrame;
    buffer_type<T> expanded;
    buffer_type<T> decimated;
    buffer_type<T> inputBuffer;
    buffer_type<T> outputBuffer;
    buffer_type<T> window;
    std::shared_ptr<FourierTransformer<T>> transform;
    int P;
    int Q;
    int N;

  public:
    PhaseVocoder(
        int P, int Q, int N, typename FourierTransformer<T>::Factory &factory)
        : interpolateFrames{P, Q, N / 2 + 1}, overlapExtract{N, hop(N)},
          filter{lowPassFilter(T{0.5} / std::max(P, Q), 501), factory},
          overlappedOutput{N},
          nextFrame(sizeNarrow<complex_type<T>>(N / 2 + 1)),
          expanded(sizeNarrow<T>(hop(N) * P)),
          decimated(sizeNarrow<T>(hop(N) * P / Q)),
          inputBuffer(sizeNarrow<T>(N)),
          outputBuffer(sizeNarrow<T>(hop(N))), window{hannWindow<T>(N)},
          transform{factory.make(N)}, P{P}, Q{Q}, N{N} {
        buffer_type<T> delayedStart(sizeNarrow<T>(N - hop(N)), T{0});
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
