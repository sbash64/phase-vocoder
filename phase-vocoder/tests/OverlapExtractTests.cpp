#include <vector>

template<typename T>
class OverlapExtract {
	std::vector<T> signal;
	int N;
public:
	OverlapExtract(int N, int ) : N{ N } {}

	void add(std::vector<T> x) {
		signal = x;
	}

	std::vector<T> next() {
		return { signal.begin(), signal.begin() + N };
	}
};

#include <gtest/gtest.h>

template<typename T>
void assertEqual(std::vector<T> actual, std::vector<T> expected) {
	EXPECT_EQ(actual.size(), expected.size());
	for (std::size_t i = 0; i < actual.size(); ++i)
		EXPECT_EQ(actual.at(i), expected.at(i));
}

class OverlapExtractTests : public ::testing::Test {

};

TEST_F(OverlapExtractTests, nextReturnsNLengthSegment) {
	int N = 5;
	int hop = 2;
	OverlapExtract<int> extract{ N, hop };
	extract.add({ 1, 2, 3, 4, 5 });
	assertEqual({ 1, 2, 3, 4, 5 }, extract.next());
}

TEST_F(OverlapExtractTests, nextReturnsNLengthSegmentWithLeftOver) {
	int N = 5;
	int hop = 2;
	OverlapExtract<int> extract{ N, hop };
	extract.add({ 1, 2, 3, 4, 5, 6 });
	assertEqual({ 1, 2, 3, 4, 5 }, extract.next());
}

TEST_F(OverlapExtractTests, nextReturnsSecondNLengthSegmentOneHopAway) {
	int N = 5;
	int hop = 2;
	OverlapExtract<int> extract{ N, hop };
	extract.add({ 1, 2, 3, 4, 5, 6, 7 });
	extract.next();
	assertEqual({ 3, 4, 5, 6, 7 }, extract.next());
}