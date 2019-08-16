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
public:
    PhaseVocoder(int P, int Q, int N) :
        interpolateFrames{P, Q, N},
        overlapExtract{N, N/4},
        filter{},
        overlapAdd{N, N/4},
        expand{P},
        decimate{Q},
        nextFrame{N},
        expanded{N*P},
        decimated{N*P/Q},
        inputBuffer(N)
    {}

    void vocode(gsl::span<T> x) {
        extract.add(x);
        auto head = x.begin();
        while (extract.hasNext()) {
            extract.next(inputBuffer);
            // window
            transform.dft(inputBuffer, nextFrame);
            interpolateFrames.add(nextFrame);
            while (interpolateFrames.hasNext()) {
                interpolateFrames.next(nextFrame);
                transform.idft(nextFrame, inputBuffer);
                // window
                expand.expand(inputBuffer, expanded);
                filter.filter(expanded);
                decimate.decimate(expanded, decimated);
                std::copy(decimated.begin(), decimated.end(), head);
                head += decimated.size();
            }
        }
    }
}
}