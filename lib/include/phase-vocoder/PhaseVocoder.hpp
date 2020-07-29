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

namespace phase_vocoder {
constexpr auto hop(index_type N) -> index_type { return N / 4; }

template <typename T>
auto lowPassFilter(T cutoff, index_type taps) -> impl::buffer_type<T> {
    impl::buffer_type<T> coefficients(taps);
    std::generate(begin(coefficients), end(coefficients), [=, n = 0]() mutable {
        const auto something{pi<T>() * (n++ - (taps - 1) / 2)};
        return something == 0 ? T{1}
                              : std::sin(2 * cutoff * something) / something;
    });

    const auto window{hannWindow<T>(taps)};
    std::transform(begin(coefficients), end(coefficients), begin(window),
        begin(coefficients), std::multiplies<>{});

    const auto sum{
        std::accumulate(begin(coefficients), end(coefficients), T{0})};
    for (auto &x : coefficients)
        x /= sum;
    return coefficients;
}

template <typename T>
void multiplyFirstToSecond(
    const impl::buffer_type<T> &window, impl::buffer_type<T> &inputBuffer) {
    std::transform(begin(inputBuffer), end(inputBuffer), begin(window),
        begin(inputBuffer), std::multiplies<>{});
}

template <typename T> class PhaseVocoder {
    InterpolateFrames<T> interpolateFrames;
    OverlapExtract<T> overlapExtract;
    OverlapAddFilter<T> filter;
    OverlapAdd<T> overlappedOutput;
    SignalConverterImpl<T> signalConverter;
    impl::complex_buffer_type<T> nextFrame;
    impl::buffer_type<T> expanded;
    impl::buffer_type<T> decimatedBuffer;
    impl::buffer_type<T> inputBuffer;
    impl::buffer_type<T> outputBuffer;
    impl::buffer_type<T> window;
    std::shared_ptr<FourierTransformer<T>> transform;
    impl::buffer_iterator_type<T> decimatedHead;
    index_type P;
    index_type Q;
    index_type N;

  public:
    PhaseVocoder(index_type P, index_type Q, index_type N,
        typename FourierTransformer<T>::Factory &factory)
        : interpolateFrames{P, Q, N / 2 + 1}, overlapExtract{N, hop(N)},
          filter{lowPassFilter(T{0.5} / std::max(P, Q), 501), factory},
          overlappedOutput{N}, nextFrame(N / 2 + 1), expanded(hop(N) * P),
          decimatedBuffer(hop(N) * P), inputBuffer(N), outputBuffer(hop(N)),
          window{hannWindow<T>(N)}, transform{factory.make(N)},
          decimatedHead{decimatedBuffer.begin()}, P{P}, Q{Q}, N{N} {
        impl::buffer_type<T> delayedStart(N - hop(N), T{0});
        overlapExtract.add(delayedStart);
    }

    void vocode(signal_type<T> x) {
        overlapExtract.add(x);
        auto head{x.begin()};
        while (overlapExtract.hasNext()) {
            overlapExtract.next(inputBuffer);
            multiplyFirstToSecond(window, inputBuffer);
            transform->dft(inputBuffer, nextFrame);
            interpolateFrames.add(nextFrame);
            while (interpolateFrames.hasNext()) {
                interpolateFrames.next(nextFrame);
                transform->idft(nextFrame, inputBuffer);
                multiplyFirstToSecond(window, inputBuffer);
                overlappedOutput.add(inputBuffer);
                overlappedOutput.next(outputBuffer);
                signalConverter.expand(outputBuffer, expanded);
                filter.filter(expanded);
                const auto toDecimate{hop(N) * P / Q};
                signalConverter.decimate(expanded,
                    {&(*decimatedHead),
                        static_cast<typename signal_type<T>::size_type>(
                            toDecimate)});
                decimatedHead += toDecimate;
                const auto toCopy{std::min(std::distance(head, x.end()),
                    std::distance(
                        impl::begin(decimatedBuffer), decimatedHead))};
                std::copy(impl::begin(decimatedBuffer),
                    impl::begin(decimatedBuffer) + toCopy, head);
                impl::shiftLeft(decimatedBuffer, toCopy);
                head += toCopy;
                decimatedHead -= toCopy;
            }
        }
    }
};
}

#endif
