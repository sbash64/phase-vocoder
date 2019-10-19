#include <phase-vocoder/model.h>
#include <memory>

namespace phase_vocoder {
class ISignalConverter {
public:
    virtual ~ISignalConverter() = default;
    virtual void expand(const_signal_type<double>, signal_type<double>, int) = 0;
    virtual void expand(const_signal_type<float>, signal_type<float>, int) = 0;
    virtual void decimate(const_signal_type<double>, signal_type<double>, int) = 0;
    virtual void decimate(const_signal_type<float>, signal_type<float>, int) = 0;
};

class Filter {
public:
    virtual ~Filter() = default;
    virtual void filter() = 0;

    class Factory {
    public:
        virtual ~Factory() = default;
        virtual std::shared_ptr<Filter> make() = 0;
    };
};

class SampleRateConverter {
    std::shared_ptr<Filter> filter;
    ISignalConverter &converter;
public:
    SampleRateConverter(
        ISignalConverter &converter,
        Filter::Factory &factory
    ) :
        filter{factory.make()},
        converter{converter} {}

    template<typename T>
    void convert(const_signal_type<T> x, signal_type<T> y) {
        converter.expand(x, y, {});
        filter->filter();
        converter.decimate(x, y, {});
    }
};
}

#include <gtest/gtest.h>
#include <string>

namespace phase_vocoder::test { namespace {
void append(std::string &s, const std::string &what) {
    s += what;
}

class SampleRateConverterShunt :
    public ISignalConverter,
    public Filter {
public:
    auto log() const {
        return log_;
    }

    void decimate(const_signal_type<double>, signal_type<double>, int) override {
        append(log_, "decimate");
    }

    void decimate(const_signal_type<float>, signal_type<float>, int) override {
        append(log_, "decimate");
    }

    void expand(const_signal_type<double>, signal_type<double>, int) override {
        append(log_, "expand ");
    }

    void expand(const_signal_type<float>, signal_type<float>, int) override {
        append(log_, "expand ");
    }

    void filter() override {
        append(log_, "filter ");
    }
private:
    std::string log_;
};

class SampleRateConverterShuntFactory : public Filter::Factory {
public:
    std::shared_ptr<Filter> make() override {
        return filter_;
    }

    explicit SampleRateConverterShuntFactory(std::shared_ptr<Filter> f) :
        filter_{std::move(f)} {}

private:
    std::shared_ptr<Filter> filter_;
};

class SampleRateConverterTests : public ::testing::Test {
protected:
    void convert() {
        converter.convert<double>(x, y);
    }

    auto conversionLog() const {
        return shunt->log();
    }
private:
    std::vector<double> x;
    std::vector<double> y;
    std::shared_ptr<SampleRateConverterShunt> shunt =
        std::make_shared<SampleRateConverterShunt>();
    SampleRateConverterShuntFactory factory{shunt};
    SampleRateConverter converter{*shunt, factory};
};

#define ASSERT_EQUAL(a, b)\
    EXPECT_EQ(a, b)

#define ASSERT_CONVERSION_LOG(a)\
    ASSERT_EQUAL(a, conversionLog())

TEST_F(SampleRateConverterTests, convertPerformsOperationsInOrder) {
    convert();
    ASSERT_CONVERSION_LOG("expand filter decimate");
}
}}
