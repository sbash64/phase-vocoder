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
        std::vector<T> dftReal;
        size_t N;
        size_t M;
    public:
        OverlapAdd(FourierTransformer &transformer, std::vector<T> b) : 
            transformer{transformer},
            N{nearestPowerTwo(b.size())},
            M{b.size()}
        {
            b.resize(N);
            dftReal.resize(N);
            transformer.dft(b, {});
        };

        void filter(gsl::span<T> x) {
            auto L = N - M + 1;
            for (size_t i{0}; i < L; ++i)
                dftReal.at(i) = x.at(i);
            transformer.dft(dftReal, {});
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
    protected:
        FourierTransformerStub fourierTransformer;
        std::vector<double> b;


        phase_vocoder::OverlapAdd<double> construct() {
            return {fourierTransformer, b};
        }

        void assertDftRealEquals(const std::vector<double> &x) {
            assertEqual(x, fourierTransformer.dftReal());
        }
    };

    TEST_F(OverlapAddTests, constructorTransformsTapsZeroPaddedToNearestPowerTwo) {
        b = { 1, 2, 3 };
        construct();
        assertDftRealEquals({ 1, 2, 3, 0 });
    }

    TEST_F(OverlapAddTests, filterPassesFirstBlockLSamplesToTransformZeroPaddedToN) {
        b = { 1, 2, 3 };
        auto overlapAdd = construct();
        std::vector<double> x = { 4, 5, 6 };
        overlapAdd.filter(x);
        assertDftRealEquals({ 4, 5, 0, 0 });
    }
}