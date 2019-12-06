#include "assert-utility.h"
#include <phase-vocoder/OverlapExtract.hpp>
#include <gtest/gtest.h>

namespace phase_vocoder::test {
namespace {
constexpr auto N = 5;
constexpr auto hop = 2;
class OverlapExtractTests : public ::testing::Test {
  protected:
    OverlapExtract<int> extract{N, hop};
    std::vector<int> buffer{std::vector<int>(N)};

    void assertSegments(
        std::vector<int> x, const std::vector<std::vector<int>>& segments) {
        add(std::move(x));
        for (auto &segment : segments) {
            assertHasNext();
            assertNextEquals(segment);
        }
        assertDoesNotHaveNext();
    }

    void assertHasNext() { EXPECT_TRUE(hasNext()); }

    void assertDoesNotHaveNext() { EXPECT_FALSE(hasNext()); }

    auto hasNext() -> bool { return extract.hasNext(); }

    void add(std::vector<int> x) {
        buffer = std::move(x);
        extract.add(buffer);
    }

    void assertNextEquals(const std::vector<int>& expected) {
        std::vector<int> out(N);
        extract.next(out);
        assertEqual(expected, out);
    }
};

// clang-format off

#define OVERLAP_EXTRACT_TEST(a)\
    TEST_F(OverlapExtractTests, a)

OVERLAP_EXTRACT_TEST(returnsEachNLengthSegmentWithHopSpacing) {
	assertSegments(
		{ 1, 2, 3, 4, 5, 6, 7, 8, 9 } ,
		{ { 1, 2, 3, 4, 5 }, { 3, 4, 5, 6, 7 }, { 5, 6, 7, 8, 9 } }
	);
}

OVERLAP_EXTRACT_TEST(returnsOneNLengthSegment) {
	assertSegments(
		{ 1, 2, 3, 4, 5 },
		{ { 1, 2, 3, 4, 5 } }
	);
}

OVERLAP_EXTRACT_TEST(returnsOneNLengthSegmentDespiteLeftOver) {
	assertSegments(
		{ 1, 2, 3, 4, 5, 6 },
		{ { 1, 2, 3, 4, 5 } }
	);
}

OVERLAP_EXTRACT_TEST(returnsTwoNLengthSegments) {
	assertSegments(
		{ 1, 2, 3, 4, 5, 6, 7 },
		{ { 1, 2, 3, 4, 5 }, { 3, 4, 5, 6, 7 } }
	);
}

OVERLAP_EXTRACT_TEST(addsToExisting) {
	assertSegments(
		{ 1, 2, 3 },
		{ }
	);
	assertSegments(
		{ 4, 5 },
		{ { 1, 2, 3, 4, 5 } }
	);
}

// clang-format on

}
}
