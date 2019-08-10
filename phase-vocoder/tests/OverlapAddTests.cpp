#include <gsl/gsl>
#include <vector>
#include <complex>

namespace phase_vocoder {
    class FourierTransformer {
    public:
        virtual ~FourierTransformer() = default;
        virtual void dft(gsl::span<double>, gsl::span<std::complex<double>>) = 0;
    };

    template<typename T>
    class OverlapAdd {
    public:
        OverlapAdd(FourierTransformer &transformer, std::vector<T> b) {
            transformer.dft(b, {});
        };
    };
}

#include "assert-utility.h"
#include <gtest/gtest.h>

namespace {
    class FourierTransformerStub : public phase_vocoder::FourierTransformer {
        std::vector<double> dftReal_;
    public:
        auto dftReal() const {
            return dftReal_;
        }

        void dft(gsl::span<double> x, gsl::span<std::complex<double>>) override {
            dftReal_.clear();
            for (auto x_ : x)
                dftReal_.push_back(x_);
        }
    };

    class OverlapAddTests : public ::testing::Test {
    };

    TEST_F(OverlapAddTests, constructorTransformsTaps) {
        FourierTransformerStub fourierTransformer;
        std::vector<double> b = { 1, 2, 3 };
        phase_vocoder::OverlapAdd<double> overlapAdd{fourierTransformer, b};
        assertEqual({1, 2, 3}, fourierTransformer.dftReal());
    }
}