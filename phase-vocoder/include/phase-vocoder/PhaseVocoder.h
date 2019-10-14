#ifndef PHASE_VOCODER_INCLUDE_PHASE_VOCODER_PHASEVOCODER_H_
#define PHASE_VOCODER_INCLUDE_PHASE_VOCODER_PHASEVOCODER_H_

#include "OverlapExtract.h"
#include "OverlapAddFilter.h"
#include "InterpolateFrames.h"
#include "Expand.h"
#include "Decimate.h"
#include "OverlapAdd.h"
#include <memory>
#include <functional>
#include <algorithm>
#include <vector>

namespace phase_vocoder {
constexpr int hop(int N) {
    return N/4;
}

size_t size(int N) {
    return gsl::narrow<size_t>(N);
}

template<typename T>
std::vector<T> hannWindow(int N) {
    std::vector<T> window(size(N));
    auto half = (N-1)/2;
    auto pi = std::acos(T{-1});
    std::generate(
        window.begin() + half,
        window.end(),
        [=, n = 0]() mutable {
            return T{0.5} * (1 + std::cos(pi*n/half));
    });
    std::copy(window.rbegin(), window.rbegin()+half, window.begin());
    return window;
}

template<typename T>
std::vector<T> lowPassFilter(T cutoff, int taps) {
    auto size = gsl::narrow<size_t>(taps);
    std::vector<T> coefficients(size);
    auto pi = std::acos(T{-1});
    for (size_t i{0}; i < size; ++i) {
        auto something = (pi * (i - (size - 1)/2));
        coefficients.at(i) = something == 0
            ? T{1}
            : std::sin(2 * cutoff * something)/something;
    }

    auto window = hannWindow<T>(taps);
    std::transform(
        coefficients.begin(),
        coefficients.end(),
        window.begin(),
        coefficients.begin(),
        std::multiplies<>{}
    );

    auto sum = std::accumulate(coefficients.begin(), coefficients.end(), T{0});
    for (auto &x : coefficients)
        x /= sum;
    return coefficients;
}

template<typename T>
class PhaseVocoder {
    InterpolateFrames<T> interpolateFrames;
    OverlapExtract<T> overlapExtract;
    OverlapAddFilter<T> filter;
    OverlapAdd<T> overlappedOutput;
    Expand expand;
    Decimate decimate;
    std::vector<std::complex<T>> nextFrame;
    std::vector<T> expanded;
    std::vector<T> decimated;
    std::vector<T> inputBuffer;
    std::vector<T> outputBuffer;
    std::vector<T> window;
    std::shared_ptr<FourierTransformer> transform;
    int P;
    int Q;
    int N;
public:
    PhaseVocoder(int P, int Q, int N, FourierTransformer::Factory &factory) :
        interpolateFrames{P, Q, N},
        overlapExtract{N, hop(N)},
        filter{lowPassFilter(T{0.5}/std::max(P, Q), 501), factory},
        overlappedOutput{N, hop(N)},
        expand{P},
        decimate{Q},
        nextFrame(size(N)),
        expanded(size(hop(N)*P)),
        decimated(size(hop(N)*P/Q)),
        inputBuffer(size(N)),
        outputBuffer(size(hop(N))),
        window{hannWindow<T>(N+1)},
        transform{factory.make(N)},
        P{P},
        Q{Q},
        N{N}
    {
        std::vector<T> delayedStart(size(N - hop(N)), 0);
        overlapExtract.add(delayedStart);
    }

    void vocode(gsl::span<T> x) {
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
                expand.expand<T>(outputBuffer, expanded);
                filter.filter(expanded);
                decimate.decimate<T>(expanded, decimated);
                std::copy(decimated.begin(), decimated.end(), head);
                head += hop(N)*P/Q;
            }
        }
    }

    void applyWindow() {
        std::transform(
            inputBuffer.begin(),
            inputBuffer.end(),
            window.begin(),
            inputBuffer.begin(),
            std::multiplies<>{}
        );
    }
};
}

#endif
