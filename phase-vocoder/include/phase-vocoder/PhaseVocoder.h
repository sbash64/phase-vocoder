#include "OverlapExtract.h"
#include "OverlapAddFilter.h"
#include "InterpolateFrames.h"
#include "Expand.h"
#include "Decimate.h"
#include "OverlapAdd.h"

namespace phase_vocoder {
constexpr int hop(int N) {
    return N/4;
}

template<typename T>
std::vector<T> hannWindow(int N) {
    std::vector<T> window(N);
    auto pi = std::acos(T{-1});
    for (size_t i{0}; i < window.size(); ++i)
        window.at(i) = T{0.5} * (1 - std::cos(2*pi*i/N));
    return window;
}

template<typename T>
class PhaseVocoder {
    InterpolateFrames interpolateFrames;
    OverlapExtract overlapExtract;
    OverlapAddFilter filter;
    OverlapAdd overlappedOutput;
    Expand expand;
    Decimate decimate;
    std::vector<std::complex<T>> nextFrame;
    std::vector<T> expanded;
    std::vector<T> decimated;
    std::vector<T> inputBuffer;
    std::vector<T> outputBuffer;
    std::vector<T> window;
    std::shared_ptr<FourierTransformer> transform;
public:
    PhaseVocoder(int P, int Q, int N, FourierTransformer::Factory &factory) :
        interpolateFrames{P, Q, N},
        overlapExtract{N, hop(N)},
        filter{factory},
        overlappedOutput{N, hop(N)},
        expand{P},
        decimate{Q},
        nextFrame{N},
        expanded{hop(N)*P},
        decimated{hop(N)*P/Q},
        inputBuffer(N),
        outputBuffer(hop(N)),
        window{hannWindow(N)},
        transform{factory.make(N)}
    {
        std::vector<T> delayedStart(N - hop(N), 0);
        overlapExtract.add(delayedStart);
    }

    void vocode(gsl::span<T> x) {
        extract.add(x);
        auto head = x.begin();
        while (extract.hasNext()) {
            extract.next(inputBuffer);
            applyWindow();
            transform->dft(inputBuffer, nextFrame);
            interpolateFrames.add(nextFrame);
            while (interpolateFrames.hasNext()) {
                interpolateFrames.next(nextFrame);
                transform->idft(nextFrame, inputBuffer);
                applyWindow();
                overlappedOutput.add(inputBuffer);
                overlappedOutput.next(outputBuffer);
                expand.expand(outputBuffer, expanded);
                filter.filter(expanded);
                decimate.decimate(expanded, decimated);
                std::copy(decimated.begin(), decimated.end(), head);
                head += decimated.size();
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