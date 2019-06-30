#include <phase-vocoder/OverlapExtract.h>
#include <gtest/gtest.h>

template<typename T>
void assertEqual(std::vector<T> expected, std::vector<T> actual) {
	EXPECT_EQ(expected.size(), actual.size());
	for (std::size_t i = 0; i < expected.size(); ++i)
		EXPECT_EQ(expected.at(i), actual.at(i));
}

class OverlapExtractTests : public ::testing::Test {
protected:
	int N = 5;
	int hop = 2;
	OverlapExtract<int> extract{ N, hop };

	auto next() {
		return extract.next();
	}

	auto hasNext() {
		return extract.hasNext();
	}

	void add(std::vector<int> x) {
		extract.add(x);
	}

	void assertNextEquals(std::vector<int> expected) {
		assertEqual(expected, next());
	}
};

TEST_F(OverlapExtractTests, nextReturnsNLengthSegment) {
	add({ 1, 2, 3, 4, 5 });
	assertNextEquals({ 1, 2, 3, 4, 5 });
}

TEST_F(OverlapExtractTests, nextReturnsNLengthSegmentWithLeftOver) {
	add({ 1, 2, 3, 4, 5, 6 });
	assertNextEquals({ 1, 2, 3, 4, 5 });
}

TEST_F(OverlapExtractTests, nextReturnsSecondNLengthSegmentOneHopAway) {
	add({ 1, 2, 3, 4, 5, 6, 7 });
	next();
	assertNextEquals({ 3, 4, 5, 6, 7 });
}

TEST_F(OverlapExtractTests, addAddsToExisting) {
	add({ 1, 2, 3 });
	add({ 4, 5 });
	assertNextEquals({ 1, 2, 3, 4, 5 });
}

TEST_F(OverlapExtractTests, hasNextIfNLengthSegmentAvailable) {
	add({ 1, 2, 3, 4, 5 });
	EXPECT_TRUE(hasNext());
	next();
	EXPECT_FALSE(hasNext());
}

TEST_F(OverlapExtractTests, hasNextWhenNLengthSegmentAvailable) {
	add({ 1, 2 });
	EXPECT_FALSE(hasNext());
	add({ 3, 4 });
	EXPECT_FALSE(hasNext());
	add({ 5 });
	EXPECT_TRUE(hasNext());
}