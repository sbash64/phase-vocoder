#include "OverlapExtract.h"
#include "OverlapAddFilter.h"
#include "InterpolateFrames.h"
#include "Expand.h"
#include "Decimate.h"
#include "OverlapAdd.h"

namespace phase_vocoder {
template<typename T>
class PhaseVocoder {
    InterpolateFrames interpolateFrames;
    OverlapExtract overlapExtract;
    OverlapAddFilter filter;
    OverlapAdd overlapAdd;
    Expand expand;
    Decimate decimate;
    std::vector<std::complex<T>> nextFrame;
    std::vector<T> expanded;
    std::vector<T> decimated;
    std::vector<T> inputBuffer;
    std::shared_ptr<FourierTransformer> transform;
public:
    PhaseVocoder(int P, int Q, int N, FourierTransformer::Factory &factory) :
        interpolateFrames{P, Q, N},
        overlapExtract{N, N/4},
        filter{factory},
        overlapAdd{N, N/4},
        expand{P},
        decimate{Q},
        nextFrame{N},
        expanded{hop*P},
        decimated{hop*P/Q},
        inputBuffer(N),
        transform{factory.make(N)}
    {}

    void vocode(gsl::span<T> x) {
        extract.add(x);
        auto head = x.begin();
        while (extract.hasNext()) {
            extract.next(inputBuffer);
            // window
            transform->dft(inputBuffer, nextFrame);
            interpolateFrames.add(nextFrame);
            while (interpolateFrames.hasNext()) {
                interpolateFrames.next(nextFrame);
                transform->idft(nextFrame, inputBuffer);
                // window
                overlapAdd.add(inputBuffer);
                overlapAdd.next(inputBuffer);
                expand.expand({inputBuffer.begin(), hop}, expanded);
                filter.filter(expanded);
                decimate.decimate(expanded, decimated);
                std::copy(decimated.begin(), decimated.end(), head);
                head += decimated.size();
            }
        }
    }
}
}