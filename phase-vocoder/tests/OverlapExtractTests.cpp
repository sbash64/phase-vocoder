#include <vector>

template<typename T>
class OverlapExtract {
public:
	OverlapExtract(int N, int hop) {}

	void add(std::vector<T> x) {}

	std::vector<T> next() {
		return {};
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

TEST_F(OverlapExtractTests, tbd) {
	int N = 5;
	int hop = 2;
	OverlapExtract extract{ N, hop };
	extract.add({ 1, 2, 3, 4, 5 });
	assertEqual({ 1, 2, 3, 4, 5 }, extract.next());
}