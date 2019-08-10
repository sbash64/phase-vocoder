namespace phase_vocoder {
    class FourierTransformer {
    public:
        virtual ~FourierTransformer() = default;
    };

    template<typename T>
    class OverlapAdd {
    public:
        OverlapAdd(FourierTransformer &) {};
    };
}

#include <gtest/gtest.h>

namespace {
    class FourierTransformerStub : public phase_vocoder::FourierTransformer {};

    class OverlapAddTests : public ::testing::Test {
        FourierTransformerStub fourierTransformer;
        phase_vocoder::OverlapAdd<double> overlapAdd{fourierTransformer};
    };

    TEST_F(OverlapAddTests, tbd) {
        
    }
}