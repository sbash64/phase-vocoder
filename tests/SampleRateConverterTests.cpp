#include <phase-vocoder/model.hpp>
#include <memory>

namespace phase_vocoder {
template<typename T>
class ISignalConverter {
public:
    virtual ~ISignalConverter() = default;
    virtual void expand(const_signal_type<T>, signal_type<T>) = 0;
    virtual void decimate(const_signal_type<T>, signal_type<T>) = 0;
};

template<typename T>
class Filter {
public:
    virtual ~Filter() = default;
    virtual void filter(signal_type<T>) = 0;

    class Factory {
    public:
        virtual ~Factory() = default;
        virtual std::shared_ptr<Filter> make() = 0;
    };
};

template<typename T>
class SampleRateConverter {
public:
    SampleRateConverter(
        ISignalConverter<T> &converter,
        typename Filter<T>::Factory &factory
    ) :
        filter{factory.make()},
        converter{converter} {}

    void convert(const_signal_type<T> x, signal_type<T> y) {
        converter.expand(x, y);
        filter->filter(y);
        converter.decimate(x, y);
    }
private:
    std::shared_ptr<Filter<T>> filter;
    ISignalConverter<T> &converter;
};
}

#include "assert-utility.h"
#include <gtest/gtest.h>
#include <string>

namespace phase_vocoder::test { namespace {
void append(std::string &s, const std::string &what) {
    s += what;
}

template<typename T>
class SampleRateConverterShunt :
    public ISignalConverter<T>,
    public Filter<T> {
public:
    auto log() const {
        return log_;
    }

    auto decimateInput() const {
        return decimateInput_;
    }

    void decimate(const_signal_type<T> x, signal_type<T>) override {
        append(log_, "decimate");
        decimateInput_ = x;
    }

    void expand(const_signal_type<T>, signal_type<T>) override {
        append(log_, "expand ");
    }

    void filter(signal_type<T>) override {
        append(log_, "filter ");
    }
private:
    const_signal_type<T> decimateInput_;
    std::string log_;
};

template<typename T>
class SampleRateConverterShuntFactory : public Filter<T>::Factory {
public:
    std::shared_ptr<Filter<T>> make() override {
        return filter_;
    }

    explicit SampleRateConverterShuntFactory(std::shared_ptr<Filter<T>> f) :
        filter_{std::move(f)} {}

private:
    std::shared_ptr<Filter<T>> filter_;
};

class SampleRateConverterTests : public ::testing::Test {
protected:
    void convert() {
        converter.convert(x, y);
    }

    auto conversionLog() const {
        return shunt->log();
    }

    auto input() const {
        return x;
    }

    auto decimateInput() const {
        return shunt->decimateInput();
    }
private:
    std::vector<double> x;
    std::vector<double> y;
    std::shared_ptr<SampleRateConverterShunt<double>> shunt =
        std::make_shared<SampleRateConverterShunt<double>>();
    SampleRateConverterShuntFactory<double> factory{shunt};
    SampleRateConverter<double> converter{*shunt, factory};
};

#define ASSERT_EQUAL(a, b)\
    EXPECT_EQ(a, b)

#define ASSERT_CONVERSION_LOG(a)\
    ASSERT_EQUAL(a, conversionLog())

#define ASSERT_INPUT_PASSED_TO_DECIMATE()\
    assertEqual(input(), decimateInput())

TEST_F(SampleRateConverterTests, convertPerformsOperationsInOrder) {
    convert();
    ASSERT_CONVERSION_LOG("expand filter decimate");
}

TEST_F(SampleRateConverterTests, convertPassesInputToDecimate) {
    convert();
    ASSERT_INPUT_PASSED_TO_DECIMATE();
}
}}
