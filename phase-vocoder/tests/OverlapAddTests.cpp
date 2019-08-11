#include <gsl/gsl>
#include <vector>
#include <complex>
#include <algorithm>

namespace phase_vocoder {
    class FourierTransformer {
    public:
        virtual ~FourierTransformer() = default;
        virtual void dft(gsl::span<double>, gsl::span<std::complex<double>>) = 0;
        virtual void idft(gsl::span<std::complex<double>>, gsl::span<double>) = 0;
    };

    constexpr size_t nearestGreaterPowerTwo(size_t n) {
        size_t power{1};
        while (n >>= 1)
            ++power;
        return 1 << power;
    }

    template<typename T>
    class OverlapAdd {
        FourierTransformer &transformer;
        std::vector<std::complex<T>> complexBuffer;
        std::vector<std::complex<T>> H;
        std::vector<T> realBuffer;
        std::vector<T> overlap;
        size_t N;
        size_t M;
    public:
        OverlapAdd(FourierTransformer &transformer, std::vector<T> b) : 
            transformer{transformer},
            N{nearestGreaterPowerTwo(b.size())},
            M{b.size()}
        {
            b.resize(N);
            realBuffer.resize(N);
            H.resize(N);
            overlap.resize(N);
            complexBuffer.resize(N);
            dft(b, H);
        };

        void filter(gsl::span<T> x) {
            auto L = N - M + 1;
            for (size_t j{0}; j < x.size()/L; ++j)
                filterCompleteBlock(x.subspan(j*L, L));
        }

    private:
        void dft(gsl::span<T> x, gsl::span<std::complex<T>> X) {
            transformer.dft(x, X);
        }

        void filterCompleteBlock(gsl::span<T> x) {
            auto L = N - M + 1;
            std::copy(x.begin(), x.end(), realBuffer.begin());
            dft(realBuffer, complexBuffer);
            std::transform(
                complexBuffer.begin(),
                complexBuffer.end(),
                H.begin(),
                complexBuffer.begin(),
                std::multiplies<>{}
            );
            transformer.idft(complexBuffer, realBuffer);
            std::transform(
                overlap.begin(),
                overlap.end(),
                realBuffer.begin(),
                overlap.begin(),
                std::plus<>{}
            );
            std::copy(overlap.begin(), overlap.begin() + L, x.begin());
            for (size_t i{0}; i < N - L; ++i)
                overlap.at(i) = overlap.at(i+L);
        }
    };
}

#include "assert-utility.h"
#include <gtest/gtest.h>

namespace {
    class FourierTransformerStub : public phase_vocoder::FourierTransformer {
        std::vector<double> dftReal_;
        std::vector<std::vector<double>> dftReals_;
        std::vector<double> idftReal_;
        std::vector<std::complex<double>> dftComplex_;
        std::vector<std::complex<double>> idftComplex_;
    public:
        auto dftReal() const {
            return dftReal_;
        }

        auto dftReals(size_t n) const {
            return dftReals_.at(n);
        }

        auto idftComplex() const {
            return idftComplex_;
        }

        template<typename T>
        void copy(gsl::span<T> x, std::vector<T> &y) {
            y.clear();
            for (auto x_ : x)
                y.push_back(x_);
        }

        template<typename T>
        void copy(const std::vector<T> &x, gsl::span<T> y) {
            std::copy(x.begin(), x.end(), y.begin());
        }

        void dft(gsl::span<double> x, gsl::span<std::complex<double>> y) override {
            copy(x, dftReal_);
            copy(dftComplex_, y);
            dftReals_.push_back(dftReal_);
        }

        void setDftComplex(std::vector<std::complex<double>> x) {
            dftComplex_ = std::move(x);
        }

        void setIdftReal(std::vector<double> x) {
            idftReal_ = std::move(x);
        }
        
        void idft(gsl::span<std::complex<double>> x, gsl::span<double> y) override {
            copy(x, idftComplex_);
            copy(idftReal_, y);
        }
    };

    class OverlapAddTests : public ::testing::Test {
    protected:
        FourierTransformerStub fourierTransformer;
        std::vector<double> b;
        std::vector<double> x;

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

        void setDftComplex(std::vector<std::complex<double>> x) {
            fourierTransformer.setDftComplex(std::move(x));
        }

        void setIdftReal(std::vector<double> x) {
            fourierTransformer.setIdftReal(std::move(x));
        }

        void filter(phase_vocoder::OverlapAdd<double> &overlapAdd) {
            overlapAdd.filter(x);
        }

        void assertXEquals(const std::vector<double> &x_) {
            assertEqual(x_, x);
        }

        void resizeX(size_t n) {
            x.resize(n);
        }

        void assertDftRealEquals(const std::vector<double> &x, size_t n) {
            assertEqual(x, fourierTransformer.dftReals(n));
        }
    };

    TEST_F(OverlapAddTests, constructorTransformsTapsZeroPaddedToNearestGreaterPowerTwo) {
        b = { 1, 2, 3 };
        construct();
        assertDftRealEquals({ 1, 2, 3, 0 });
    }

    TEST_F(OverlapAddTests, filterPassesEachBlockLSamplesToTransformZeroPaddedToN) {
        setTapCount(3);
        auto overlapAdd = construct();
        x = { 4, 5, 6, 7, 8, 9 };
        filter(overlapAdd);
        assertDftRealEquals({4, 5, 0, 0}, 1);
        assertDftRealEquals({6, 7, 0, 0}, 2);
        assertDftRealEquals({8, 9, 0, 0}, 3);
    }

    TEST_F(OverlapAddTests, filterPassesFirstTransformProductToInverseTransform) {
        setTapCount(3);
        setDftComplex({ 4, 5, 6, 7 });
        auto overlapAdd = construct();
        setDftComplex({ 11, 12, 13, 14 });
        resizeX(2);
        filter(overlapAdd);
        assertIdftComplexEquals({ 4*11, 5*12, 6*13, 7*14 });
    }

    TEST_F(OverlapAddTests, filterOverlapAddsInverseTransform) {
        setTapCount(3);
        setDftComplex({ 0, 0, 0, 0 });
        auto overlapAdd = construct();
        resizeX(2);
        setIdftReal({4, 5, 6, 7});
        filter(overlapAdd);
        assertXEquals({ 4, 5 });
    }

    TEST_F(OverlapAddTests, filterOverlapAddsInverseTransform2) {
        setTapCount(3);
        setDftComplex({ 0, 0, 0, 0 });
        auto overlapAdd = construct();
        resizeX(4);
        setIdftReal({5, 6, 7, 8});
        filter(overlapAdd);
        assertXEquals({ 5, 6, 5+7, 6+8 });
    }
}