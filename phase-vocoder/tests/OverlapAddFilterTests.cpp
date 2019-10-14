#include "assert-utility.h"
#include <phase-vocoder/OverlapAddFilter.h>
#include <gtest/gtest.h>

namespace {
template<typename T>
void copy(gsl::span<const T> x, gsl::span<T> y) {
    std::copy(x.begin(), x.end(), y.begin());
}

template<typename T>
void resizeToMatch(std::vector<T> &x, gsl::span<const T> y) {
    x.resize(gsl::narrow_cast<size_t>(y.size()));
}

template<typename T>
class FourierTransformerStub : public phase_vocoder::FourierTransformer {
	using complex_buffer_type = std::vector<std::complex<T>>;
	using real_buffer_type = std::vector<T>;
    std::vector<std::vector<T>> dftReals_;
    complex_buffer_type dftComplex_;
    complex_buffer_type idftComplex_;
    real_buffer_type dftReal_;
    real_buffer_type idftReal_;
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

    void dft(gsl::span<T> x, gsl::span<std::complex<T>> y) override {
        resizeToMatch<T>(dftReal_, x);
        copy<T>(x, dftReal_);
        copy<std::complex<T>>(dftComplex_, y);
        dftReals_.push_back(dftReal_);
    }

    void setDftComplex(complex_buffer_type x) {
        dftComplex_ = std::move(x);
    }

    void setIdftReal(real_buffer_type x) {
        idftReal_ = std::move(x);
    }

    void idft(gsl::span<std::complex<T>> x, gsl::span<T> y) override {
        resizeToMatch<std::complex<T>>(idftComplex_, x);
        copy<std::complex<T>>(x, idftComplex_);
        copy<T>(idftReal_, y);
    }

    class FactoryStub : public Factory {
        std::shared_ptr<FourierTransformer> transform;
        int N_;
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
    std::shared_ptr<FourierTransformerStub<double>> fourierTransformer_ =
        std::make_shared<FourierTransformerStub<double>>();
    FourierTransformerStub<double>::FactoryStub factory{fourierTransformer_};
    std::vector<double> b;
    std::vector<double> signal;

    phase_vocoder::OverlapAddFilter<double> construct() {
        return {b, factory};
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
        overlapAdd.filter(signal);
    }

    void assertXEquals(const std::vector<double> &x_) {
        assertEqual(x_, signal);
    }

    void resizeX(size_t n) {
        signal.resize(n);
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
    signal = { 5, 6, 7, 8, 9, 10 };
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
    signal = { 5, 6, 7 };
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
