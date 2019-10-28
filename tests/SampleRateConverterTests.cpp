#include <phase-vocoder/model.hpp>
#include <phase-vocoder/utility.hpp>
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
        int P,
        int hop,
        ISignalConverter<T> &converter,
        typename Filter<T>::Factory &factory
    ) :
        buffer(sizeNarrow<T>(P*hop)),
        filter{factory.make()},
        converter{converter} {}

    void convert(const_signal_type<T> x, signal_type<T> y) {
        converter.expand(x, buffer);
        filter->filter(buffer);
        converter.decimate(x, y);
    }
private:
    std::vector<T> buffer;
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

    auto expandInput() const {
        return expandInput_;
    }

    auto expandOutput() const {
        return expandOutput_;
    }

    auto decimateOutput() const {
        return decimateOutput_;
    }

    auto filterInput() const {
        return filterInput_;
    }

    void decimate(const_signal_type<T> x, signal_type<T> y) override {
        append(log_, "decimate");
        decimateInput_ = x;
        decimateOutput_ = y;
    }

    void expand(const_signal_type<T> x, signal_type<T> y) override {
        expandInput_ = x;
        expandOutput_ = y;
        append(log_, "expand ");
    }

    void filter(signal_type<T> x) override {
        append(log_, "filter ");
        filterInput_ = x;
    }
private:
    const_signal_type<T> decimateInput_;
    const_signal_type<T> expandInput_;
    signal_type<T> expandOutput_;
    signal_type<T> decimateOutput_;
    signal_type<T> filterInput_;
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

constexpr auto P = 3;
constexpr auto hop = 5;

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

    auto output() const {
        return y;
    }

    auto decimateInput() const {
        return shunt->decimateInput();
    }

    auto expandInput() const {
        return shunt->expandInput();
    }

    auto expandOutput() const {
        return shunt->expandOutput();
    }

    auto decimateOutput() const {
        return shunt->decimateOutput();
    }

    auto filterInput() const {
        return shunt->filterInput();
    }
    
private:
    std::vector<double> x;
    std::vector<double> y;
    std::shared_ptr<SampleRateConverterShunt<double>> shunt =
        std::make_shared<SampleRateConverterShunt<double>>();
    SampleRateConverterShuntFactory<double> factory{shunt};
    SampleRateConverter<double> converter{P, hop, *shunt, factory};
};

#define ASSERT_EQUAL(a, b)\
    EXPECT_EQ(a, b)

#define ASSERT_CONVERSION_LOG(a)\
    ASSERT_EQUAL(a, conversionLog())

#define ASSERT_INPUT_PASSED_TO_DECIMATE()\
    assertEqual(input(), decimateInput())

#define ASSERT_INPUT_PASSED_TO_EXPAND()\
    assertEqual(input(), expandInput())

#define ASSERT_OUTPUT_PASSED_TO_DECIMATE()\
    assertEqual(output(), decimateOutput())

#define ASSERT_EXPAND_OUTPUT_SIZE(a)\
    assertEqual(a, size(expandOutput()));

#define ASSERT_FILTER_INPUT_SIZE(a)\
    assertEqual(a, size(filterInput()));

TEST_F(SampleRateConverterTests, convertPerformsOperationsInOrder) {
    convert();
    ASSERT_CONVERSION_LOG("expand filter decimate");
}

TEST_F(SampleRateConverterTests, convertPassesInputToDecimate) {
    convert();
    ASSERT_INPUT_PASSED_TO_DECIMATE();
}

TEST_F(SampleRateConverterTests, convertPassesInputToExpand) {
    convert();
    ASSERT_INPUT_PASSED_TO_EXPAND();
}

TEST_F(SampleRateConverterTests, convertPassesOutputToDecimate) {
    convert();
    ASSERT_OUTPUT_PASSED_TO_DECIMATE();
}

TEST_F(SampleRateConverterTests, convertPassesBufferOfSizeHopTimesPToExpand) {
    convert();
    ASSERT_EXPAND_OUTPUT_SIZE(3 * 5l);
}

TEST_F(SampleRateConverterTests, convertPassesBufferOfSizeHopTimesPToFilter) {
    convert();
    ASSERT_FILTER_INPUT_SIZE(3 * 5l);
}
}}
