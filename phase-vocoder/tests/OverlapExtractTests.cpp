#include <phase-vocoder/OverlapExtract.h>
#include <gtest/gtest.h>

namespace {
	template<typename T>
	void assertEqual(std::vector<T> expected, std::vector<T> actual) {
		EXPECT_EQ(expected.size(), actual.size());
		for (std::size_t i = 0; i < expected.size(); ++i)
			EXPECT_EQ(expected.at(i), actual.at(i));
	}

	constexpr auto N = 5;
	constexpr auto hop = 2;
	class OverlapExtractTests : public ::testing::Test {
	protected:
		OverlapExtract<int> extract;
		std::vector<int> buffer;

		OverlapExtractTests() : buffer(N), extract{ N, hop } {}

		auto next() {
			std::vector<int> out(N);
			extract.next(out);
		}

		auto hasNext() {
			return extract.hasNext();
		}

		void add(std::vector<int> x) {
			buffer = x;
			extract.add(buffer);
		}

		void assertNextEquals(std::vector<int> expected) {
			std::vector<int> out(N);
			extract.next(out);
			assertEqual(expected, out);
		}

		void assertHasNext() {
			EXPECT_TRUE(hasNext());
		}

		void assertDoesNotHaveNext() {
			EXPECT_FALSE(hasNext());
		}

		void add2(std::vector<int> x) {
			add(x);
		}

		void assertSegments(std::vector<int> x, std::vector<std::vector<int>> segments) {
			add(x);
			for (size_t i = 0; i < segments.size(); ++i)
				assertNextEquals(segments.at(i));
		}
	};

	TEST_F(OverlapExtractTests, returnsEachNLengthSegmentWithHopSpacing) {
		assertSegments(
			{ 1, 2, 3, 4, 5, 6, 7, 8, 9 } , 
			{ { 1, 2, 3, 4, 5 }, { 3, 4, 5, 6, 7 }, { 5, 6, 7, 8, 9 } }
		);
	}

	TEST_F(OverlapExtractTests, nextReturnsNLengthSegment) {
		assertSegments(
			{ 1, 2, 3, 4, 5 },
			{ { 1, 2, 3, 4, 5 } }
		);
	}

	TEST_F(OverlapExtractTests, nextReturnsNLengthSegmentWithLeftOver) {
		assertSegments(
			{ 1, 2, 3, 4, 5, 6 },
			{ { 1, 2, 3, 4, 5 } }
		);
	}

	TEST_F(OverlapExtractTests, nextReturnsSecondNLengthSegmentOneHopAway) {
		assertSegments(
			{ 1, 2, 3, 4, 5, 6, 7 },
			{ { 1, 2, 3, 4, 5 }, { 3, 4, 5, 6, 7 } }
		);
	}

	TEST_F(OverlapExtractTests, addAddsToExisting) {
		assertSegments(
			{ 1, 2, 3 },
			{ }
		);
		assertSegments(
			{ 4, 5 },
			{ { 1, 2, 3, 4, 5 } }
		);
	}

	TEST_F(OverlapExtractTests, hasNextIfNLengthSegmentAvailable) {
		add({ 1, 2, 3, 4, 5 });
		assertHasNext();
		next();
		assertDoesNotHaveNext();
	}

	TEST_F(OverlapExtractTests, hasNextWhenNLengthSegmentAvailable) {
		add({ 1, 2 });
		assertDoesNotHaveNext();
		add({ 3, 4 });
		assertDoesNotHaveNext();
		add({ 5 });
		assertHasNext();
	}
}