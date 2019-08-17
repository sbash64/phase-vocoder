#include "assert-utility.h"
#include <phase-vocoder/OverlapAddFilter.h>
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

    class FactoryStub : public Factory {
        int N_;
        std::shared_ptr<FourierTransformer> transform;
    public:
        explicit FactoryStub(std::shared_ptr<FourierTransformer> transform) :
            transform{std::move(transform)} {}

        std::shared_ptr<FourierTransformer> make(int N) override {
            N_ = N;
            return transform;
        }

        auto N() const {
            return N_;
        }
    };
};

class OverlapAddFilterTests : public ::testing::Test {
protected:
    FourierTransformerStub fourierTransformer;
    std::shared_ptr<FourierTransformerStub> fourierTransformer_ =
        std::make_shared<FourierTransformerStub>();
    FourierTransformerStub::FactoryStub factory{fourierTransformer_};
    std::vector<double> b;
    std::vector<double> x;

    phase_vocoder::OverlapAddFilter<double> construct() {
        return {fourierTransformer, b, &factory};
    }

    void assertDftRealEquals(const std::vector<double> &x) {
        assertEqual(x, fourierTransformer_->dftReal());
    }

    void assertIdftComplexEquals(const std::vector<std::complex<double>> &x) {
        assertEqual(x, fourierTransformer_->idftComplex());
    }

    void setTapCount(size_t n) {
        b.resize(n);
    }

    void setDftComplex(std::vector<std::complex<double>> x) {
        fourierTransformer_->setDftComplex(std::move(x));
    }

    void setIdftReal(std::vector<double> x) {
        fourierTransformer_->setIdftReal(std::move(x));
    }

    void filter(phase_vocoder::OverlapAddFilter<double> &overlapAdd) {
        overlapAdd.filter(x);
    }

    void assertXEquals(const std::vector<double> &x_) {
        assertEqual(x_, x);
    }

    void resizeX(size_t n) {
        x.resize(n);
    }

    void assertDftRealEquals(const std::vector<double> &x, size_t n) {
        assertEqual(x, fourierTransformer_->dftReals(n));
    }
};

TEST_F(
    OverlapAddFilterTests,
    constructorCreatesTransformWithSizeGreaterThanTapsNearestPowerTwo
) {
    b = { 1, 2, 3 };
    construct();
    assertEqual(4, factory.N());
}

TEST_F(
    OverlapAddFilterTests,
    constructorTransformsTapsZeroPaddedToNearestGreaterPowerTwo
) {
    b = { 1, 2, 3 };
    construct();
    assertDftRealEquals({ 1, 2, 3, 0 });
}

TEST_F(OverlapAddFilterTests, filterPassesEachBlockLSamplesToTransformZeroPaddedToN) {
    setTapCount(4 - 1);
    auto overlapAdd = construct();
    x = { 5, 6, 7, 8, 9, 10 };
    filter(overlapAdd);
    assertDftRealEquals({5, 6, 0, 0}, 1);
    assertDftRealEquals({7, 8, 0, 0}, 2);
    assertDftRealEquals({9, 10, 0, 0}, 3);
}

TEST_F(
    OverlapAddFilterTests,
    filterPassesEachBlockLSamplesToTransformZeroPaddedToN2
) {
    setTapCount(4 - 1);
    auto overlapAdd = construct();
    x = { 5, 6, 7 };
    filter(overlapAdd);
    assertDftRealEquals({5, 6, 0, 0}, 1);
    assertDftRealEquals({7, 0, 0, 0}, 2);
}

TEST_F(OverlapAddFilterTests, filterPassesTransformProductToInverseTransform) {
    setTapCount(4 - 1);
    setDftComplex({ 5, 6, 7, 8 });
    auto overlapAdd = construct();
    setDftComplex({ 11, 12, 13, 14 });
    resizeX(2);
    filter(overlapAdd);
    assertIdftComplexEquals({ 5*11, 6*12, 7*13, 8*14 });
}

TEST_F(OverlapAddFilterTests, filterOverlapAddsInverseTransform) {
    setTapCount(4 - 1);
    setDftComplex({ 0, 0, 0, 0 });
    auto overlapAdd = construct();
    resizeX(2);
    setIdftReal({5, 6, 7, 8});
    filter(overlapAdd);
    assertXEquals({ 5, 6 });
}

TEST_F(OverlapAddFilterTests, filterOverlapAddsInverseTransform2) {
    setTapCount(4 - 1);
    setDftComplex({ 0, 0, 0, 0 });
    auto overlapAdd = construct();
    resizeX(4);
    setIdftReal({5, 6, 7, 8});
    filter(overlapAdd);
    assertXEquals({ 5, 6, 5+7, 6+8 });
}

TEST_F(OverlapAddFilterTests, filterOverlapAddsInverseTransform3) {
    setTapCount(4 - 1);
    setDftComplex({ 0, 0, 0, 0 });
    auto overlapAdd = construct();
    resizeX(2);
    setIdftReal({5, 6, 7, 8});
    filter(overlapAdd);
    assertXEquals({ 5, 6 });
    setIdftReal({9, 10, 11, 12});
    filter(overlapAdd);
    assertXEquals({ 9+7, 10+8 });
}

TEST_F(OverlapAddFilterTests, filterOverlapAddsInverseTransform4) {
    setTapCount(4 - 1);
    setDftComplex({ 0, 0, 0, 0 });
    auto overlapAdd = construct();
    resizeX(3);
    setIdftReal({5, 6, 7, 8});
    filter(overlapAdd);
    assertXEquals({ 5, 6, 5+7 });
}

TEST_F(OverlapAddFilterTests, filterOverlapAddsInverseTransform5) {
    setTapCount(4 - 1);
    setDftComplex({ 0, 0, 0, 0 });
    auto overlapAdd = construct();
    resizeX(3);
    setIdftReal({5, 6, 7, 8});
    filter(overlapAdd);
    assertXEquals({ 5, 6, 5+7 });
    setIdftReal({9, 10, 11, 12});
    filter(overlapAdd);
    assertXEquals({ 6+8+9, 7+10, 8+11+9 });
}
}
