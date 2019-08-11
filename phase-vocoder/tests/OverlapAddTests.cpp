#include <gsl/gsl>
#include <vector>
#include <complex>

namespace phase_vocoder {
    class FourierTransformer {
    public:
        virtual ~FourierTransformer() = default;
        virtual void dft(gsl::span<double>, gsl::span<std::complex<double>>) = 0;
    };

    constexpr size_t nearestPowerTwo(size_t n) {
        size_t result{};
        while (n >>= 1)
            ++result;
        return 1 << (result + 1);
    }

    template<typename T>
    class OverlapAdd {
        FourierTransformer &transformer;
    public:
        OverlapAdd(FourierTransformer &transformer, std::vector<T> b) : transformer{transformer} {
            b.resize(nearestPowerTwo(b.size()));
            transformer.dft(b, {});
        };

        void filter(gsl::span<T> x) {
            transformer.dft(x.first(2), {});
        }
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

    TEST_F(OverlapAddTests, constructorTransformsTapsZeroPaddedToNearestPowerTwo) {
        FourierTransformerStub fourierTransformer;
        std::vector<double> b = { 1, 2, 3 };
        phase_vocoder::OverlapAdd<double> overlapAdd{fourierTransformer, b};
        assertEqual({1, 2, 3, 0}, fourierTransformer.dftReal());
    }

    TEST_F(OverlapAddTests, filterPassesFirstBlockLSamplesToTransform) {
        FourierTransformerStub fourierTransformer;
        std::vector<double> b = { 1, 2, 3 };
        phase_vocoder::OverlapAdd<double> overlapAdd{fourierTransformer, b};
        std::vector<double> x = { 4, 5, 6 };
        overlapAdd.filter(x);
        assertEqual({ 4, 5 }, fourierTransformer.dftReal());
    }
}