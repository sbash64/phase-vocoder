namespace phase_vocoder {
class Decimator {
public:
    virtual ~Decimator() = default;
    virtual void decimate() = 0;
};

class Expander {
public:
    virtual ~Expander() = default;
    virtual void expand() = 0;
};

class Filter {
public:
    virtual ~Filter() = default;
    virtual void filter() = 0;
};

class SampleRateConverter {
    Decimator &decimator;
    Expander &expander;
    Filter &filter;
public:
    SampleRateConverter(
        Decimator &decimator,
        Expander &expander,
        Filter &filter
    ) :
        decimator{decimator},
        expander{expander},
        filter{filter} {}
    
    void convert() {
        expander.expand();
        filter.filter();
        decimator.decimate();
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
    public Decimator,
    public Expander,
    public Filter
{
public:
    auto log() const {
        return log_;
    }

    void decimate() override {
        append(log_, "decimate");
    }

    void expand() override {
        append(log_, "expand ");
    }

    void filter() override {
        append(log_, "filter ");
    }
private:
    std::string log_;
};

class SampleRateConverterTests : public ::testing::Test {
protected:
    void convert() {
        converter.convert();
    }

    auto conversionLog() const {
        return shunt.log();
    }
private:
    SampleRateConverterShunt shunt;
    SampleRateConverter converter{shunt, shunt, shunt};
};

#define ASSERT_EQUAL(a, b)\
    EXPECT_EQ(a, b)

#define ASSERT_CONVERSION_LOG(a)\
    ASSERT_EQUAL(a, conversionLog())

TEST_F(SampleRateConverterTests, tbd) {
    convert();
    ASSERT_CONVERSION_LOG("expand filter decimate");
}
}}
