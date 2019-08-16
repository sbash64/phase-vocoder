#include "assert-utility.h"
#include <phase-vocoder/OverlapExtract.h>
#include <gtest/gtest.h>

namespace {
constexpr auto N = 5;
constexpr auto hop = 2;
class OverlapExtractTests : public ::testing::Test {
protected:
	phase_vocoder::OverlapExtract<int> extract;
	std::vector<int> buffer;

	OverlapExtractTests() : extract{ N, hop }, buffer(N)  {}

	void assertSegments(
		std::vector<int> x,
		std::vector<std::vector<int>> segments
	) {
		add(std::move(x));
		for (auto segment : segments) {
			assertHasNext();
			assertNextEquals(std::move(segment));
		}
		assertDoesNotHaveNext();
	}

	void assertHasNext() {
		EXPECT_TRUE(hasNext());
	}

	void assertDoesNotHaveNext() {
		EXPECT_FALSE(hasNext());
	}

	bool hasNext() {
		return extract.hasNext();
	}

	void add(std::vector<int> x) {
		buffer = std::move(x);
		extract.add(buffer);
	}

	void assertNextEquals(std::vector<int> expected) {
		std::vector<int> out(N);
		extract.next(out);
		assertEqual(std::move(expected), std::move(out));
	}
};

TEST_F(OverlapExtractTests, returnsEachNLengthSegmentWithHopSpacing) {
	assertSegments(
		{ 1, 2, 3, 4, 5, 6, 7, 8, 9 } ,
		{ { 1, 2, 3, 4, 5 }, { 3, 4, 5, 6, 7 }, { 5, 6, 7, 8, 9 } }
	);
}

TEST_F(OverlapExtractTests, returnsOneNLengthSegment) {
	assertSegments(
		{ 1, 2, 3, 4, 5 },
		{ { 1, 2, 3, 4, 5 } }
	);
}

TEST_F(OverlapExtractTests, returnsOneNLengthSegmentDespiteLeftOver) {
	assertSegments(
		{ 1, 2, 3, 4, 5, 6 },
		{ { 1, 2, 3, 4, 5 } }
	);
}

TEST_F(OverlapExtractTests, returnsTwoNLengthSegments) {
	assertSegments(
		{ 1, 2, 3, 4, 5, 6, 7 },
		{ { 1, 2, 3, 4, 5 }, { 3, 4, 5, 6, 7 } }
	);
}

TEST_F(OverlapExtractTests, addsToExisting) {
	assertSegments(
		{ 1, 2, 3 },
		{ }
	);
	assertSegments(
		{ 4, 5 },
		{ { 1, 2, 3, 4, 5 } }
	);
}
}
