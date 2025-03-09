#include <gtest/gtest.h>

#include <algorithm>
#include <numeric>
#include <vector>

struct ThreadData {
  std::vector<double> array;
  double min;
  double max;
  double average;
};

void findMinMax(ThreadData* data) {
  data->min = *std::min_element(data->array.begin(), data->array.end());
  data->max = *std::max_element(data->array.begin(), data->array.end());
}

void calculateAverage(ThreadData* data) {
  double sum = std::accumulate(data->array.begin(), data->array.end(), 0.0);
  data->average = sum / data->array.size();
}

TEST(ThreadTests, FindMinMax) {
  ThreadData data;
  data.array = {10.0, 5.0, 20.0, 3.0, 8.0};

  findMinMax(&data);

  EXPECT_EQ(data.min, 3.0);
  EXPECT_EQ(data.max, 20.0);
}

TEST(ThreadTests, CalculateAverage) {
  ThreadData data;
  data.array = {10.0, 5.0, 20.0, 3.0, 8.0};

  calculateAverage(&data);

  EXPECT_NEAR(data.average, 9.2, 0.01);
}

TEST(ThreadTests, ReplaceMinMaxWithAverage) {
  ThreadData data;
  data.array = {10.0, 5.0, 20.0, 3.0, 8.0};

  findMinMax(&data);
  calculateAverage(&data);

  std::replace(data.array.begin(), data.array.end(), data.min, data.average);
  std::replace(data.array.begin(), data.array.end(), data.max, data.average);

  std::vector<double> expected = {9.2, 9.2, 9.2, 9.2, 9.2};
  for (size_t i = 0; i < data.array.size(); ++i) {
    EXPECT_NEAR(data.array[i], expected[i], 0.01);
  }
}
