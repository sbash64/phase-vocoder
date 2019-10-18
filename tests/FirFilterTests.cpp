#include "assert-utility.h"
#include <phase-vocoder/OverlapAddFilter.h>
#include <fftw3.h>
#include <gtest/gtest.h>
#include <gsl/gsl>
#include <vector>

namespace phase_vocoder::test { namespace {
template<typename T>
constexpr auto sizeNarrow(int x) {
	return gsl::narrow_cast<typename std::vector<T>::size_type>(x);
}

template<typename T>
void copy(gsl::span<const T> x, gsl::span<T> y) {
	std::copy(x.begin(), x.end(), y.begin());
}

template<typename T>
T *data(std::vector<T> &x) {
	return x.data();
}

fftw_complex *to_fftw_complex(std::vector<std::complex<double>> &x) {
	return reinterpret_cast<fftw_complex *>(x.data());
}

template<typename T>
class FftwTransformer : public FourierTransformer {
	using complex_buffer_type = std::vector<std::complex<T>>;
	using real_buffer_type = std::vector<T>;
	complex_buffer_type dftComplex_;
	complex_buffer_type idftComplex_;
	real_buffer_type dftReal_;
	real_buffer_type idftReal_;
	fftw_plan dftPlan;
	fftw_plan idftPlan;
	int N;
public:
	explicit FftwTransformer(int N) :
		dftComplex_(sizeNarrow<std::complex<T>>(N)),
		idftComplex_(sizeNarrow<std::complex<T>>(N)),
		dftReal_(sizeNarrow<T>(N)),
		idftReal_(sizeNarrow<T>(N)),
		dftPlan{fftw_plan_dft_r2c_1d(
			N,
			data(dftReal_),
			to_fftw_complex(dftComplex_),
			FFTW_ESTIMATE
		)},
		idftPlan{fftw_plan_dft_c2r_1d(
			N,
			to_fftw_complex(idftComplex_),
			data(idftReal_),
			FFTW_ESTIMATE
		)},
		N{N} {}

	~FftwTransformer() {
		fftw_destroy_plan(dftPlan);
		fftw_destroy_plan(idftPlan);
	}

	void dft(signal_type<T> x, signal_type<std::complex<T>> y) override {
		copy<T>(x, dftReal_);
		fftw_execute(dftPlan);
		copy<std::complex<T>>(dftComplex_, y);
	}

	void idft(signal_type<std::complex<T>> x, signal_type<T> y) override {
		copy<std::complex<T>>(x, idftComplex_);
		fftw_execute(idftPlan);
		copy<T>(idftReal_, y);
		for (auto &y_ : y)
			y_ /= N;
	}

	class FftwFactory : public Factory {
	public:
		std::shared_ptr<FourierTransformer> make(int N_) {
			return std::make_shared<FftwTransformer>(N_);
		}
	};
};

class FirFilterTests : public ::testing::Test {
protected:
	FftwTransformer<double>::FftwFactory factory;
	std::vector<double> b{ 0 };

	void assertFilteredOutput(
		std::vector<double> x,
		const std::vector<double>& y
	) {
		auto overlapAdd = construct();
		overlapAdd.filter(x);
		assertEqual(y, x);
	}

	OverlapAddFilter<double> construct() {
		return {b, factory};
	}

	void assertFilteredOutputs(
		std::vector<std::vector<double>> x,
		const std::vector<std::vector<double>>& y
	) {
		auto overlapAdd = construct();
		for (size_t i{ 0 }; i < y.size(); ++i) {
			auto next = std::move(x.at(i));
			overlapAdd.filter(next);
			assertEqual(y.at(i), next, 1e-14);
		}
	}

	void setCoefficients(std::vector<double> b_) {
		b = std::move(b_);
	}
};

TEST_F(FirFilterTests, emptyInputYieldsEmpty) {
	assertFilteredOutput({}, {});
}

TEST_F(FirFilterTests, identityYieldsSame) {
	setCoefficients({ 1 });
	assertFilteredOutput({2, 3, 4}, {2, 3, 4});
}

TEST_F(FirFilterTests, doubleYieldsDouble) {
	setCoefficients({ 2 });
	assertFilteredOutput({ 3, 4, 5 }, { 6, 8, 10 });
}

TEST_F(FirFilterTests, simpleConvolution) {
	setCoefficients({ 1, 2, 3 });
	assertFilteredOutput({ 4, 5, 6 }, { 1*4, 1*5. + 2*4., 1*6. + 2*5. + 3*4. });
}

TEST_F(FirFilterTests, simpleConvolutionInSteps) {
	setCoefficients({ 1, 2, 3 });
	assertFilteredOutputs(
		{
			{4},
			{5},
			{6}
		},
		{
			{ 1*4 },
			{ 1*5. + 2*4. },
			{ 1*6. + 2*5. + 3*4. }
		}
	);
}

TEST_F(FirFilterTests, changingSteps) {
	setCoefficients({ 1, 2, 3, 4 });
	assertFilteredOutputs(
		{
			{10},
			{11, 12},
			{13, 14, 15}
		},
		{
			{ 1*10 },
			{
				1*11. + 2*10.,
				1*12. + 2*11. + 3*10.
			},
			{
				1*13. + 2*12. + 3*11. + 4*10.,
				1*14. + 2*13. + 3*12. + 4*11.,
				1*15. + 2*14. + 3*13. + 4*12.
			}
		}
	);
}
}}
