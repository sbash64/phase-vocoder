#include <gsl/gsl>
#include <vector>
#include <complex>

namespace phase_vocoder {
    class FourierTransformer {
    public:
        virtual ~FourierTransformer() = default;
        virtual void dft(gsl::span<double>, gsl::span<std::complex<double>>) = 0;
        virtual void idft(gsl::span<std::complex<double>>, gsl::span<double>) = 0;
    };

    constexpr size_t nearestPowerTwo(size_t n) {
        size_t result{};
        --n;
        while (n >>= 1)
            ++result;
        return 1 << (result + 1);
    }

    template<typename T>
    class OverlapAdd {
        FourierTransformer &transformer;
        std::vector<std::complex<T>> dftComplex;
        std::vector<std::complex<T>> H;
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
            H.resize(N);
            dftComplex.resize(N);
            transformer.dft(b, H);
        };

        void filter(gsl::span<T> x) {
            auto L = N - M + 1;
            for (size_t i{0}; i < L; ++i)
                dftReal.at(i) = x.at(i);
            transformer.dft(dftReal, dftComplex);
            for (size_t i{0}; i < N; ++i)
                dftComplex.at(i) *= H.at(i);
            transformer.idft(dftComplex, {});
        }
    };
}

#include "assert-utility.h"
#include <gtest/gtest.h>

namespace {
    class FourierTransformerStub : public phase_vocoder::FourierTransformer {
        std::vector<double> dftReal_;
        std::vector<double> dftComplex_;
        std::vector<std::complex<double>> idftComplex_;
    public:
        auto dftReal() const {
            return dftReal_;
        }

        auto idftComplex() const {
            return idftComplex_;
        }

        void dft(gsl::span<double> x, gsl::span<std::complex<double>> y) override {
            dftReal_.clear();
            for (auto x_ : x)
                dftReal_.push_back(x_);
            for (size_t i{0}; i < dftComplex_.size(); ++i)
                y.at(i) = dftComplex_.at(i);
        }

        void setDftComplex(std::vector<double> x) {
            dftComplex_ = std::move(x);
        }
        
        void idft(gsl::span<std::complex<double>> x, gsl::span<double>) override {
            idftComplex_.clear();
            for (auto x_ : x)
                idftComplex_.push_back(x_);
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
        
        void assertIdftComplexEquals(const std::vector<std::complex<double>> &x) {
            assertEqual(x, fourierTransformer.idftComplex());
        }

        void setTapCount(size_t n) {
            b.resize(n);
        }
    };

    TEST_F(OverlapAddTests, constructorTransformsTapsZeroPaddedToNearestPowerTwo) {
        b = { 1, 2, 3 };
        construct();
        assertDftRealEquals({ 1, 2, 3, 0 });
    }

    TEST_F(OverlapAddTests, filterPassesFirstBlockLSamplesToTransformZeroPaddedToN) {
        setTapCount(3);
        auto overlapAdd = construct();
        std::vector<double> x = { 4, 5, 6, 7, 8, 9 };
        overlapAdd.filter(x);
        assertDftRealEquals({ 4, 5, 0, 0 });
    }

    TEST_F(OverlapAddTests, filterPassesFirstTransformProductToInverseTransform) {
        setTapCount(3);
        fourierTransformer.setDftComplex({ 4, 5, 6, 7 });
        auto overlapAdd = construct();
        std::vector<double> x = { 8, 9, 10 };
        fourierTransformer.setDftComplex({ 11, 12, 13, 14 });
        overlapAdd.filter(x);
        assertIdftComplexEquals({ 4*11, 5*12, 6*13, 7*14 });
    }
}