#include <vector>

template<typename T>
class OverlapExtract {
	std::vector<T> signal;
	int head;
	int hop;
	int N;
public:
	OverlapExtract(int N, int hop) : N{ N }, hop{ hop }, head{ 0 } {}

	void add(std::vector<T> x) {
		signal = x;
	}

	std::vector<T> next() {
		auto head_ = signal.begin() + head;
		head += hop;
		return { head_, head_ + N };
	}
};

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