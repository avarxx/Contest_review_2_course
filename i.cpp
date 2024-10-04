#include <algorithm>
#include <iostream>
#include <vector>

void scanf_data(int& streetSize, std::vector<int>& city) {
  std::cin >> streetSize;
  city.resize(streetSize);
  for (int i = 0; i < streetSize; ++i) {
    std::cin >> city[i];
  }
}

void print_data(int maxSequenceLength, const std::vector<int>& sequence) {
  std::cout << maxSequenceLength << std::endl;
  for (int elem : sequence) {
    std::cout << elem << " ";
  }
  std::cout << std::endl;
}

int findLongestAlternatingSequence(std::vector<int>& prev,
                                   const std::vector<int>& city,
                                   int& maxLength) {
  int streetSize = city.size();
  int maxStreet = 0;
  std::vector<int> dp(streetSize, 1);
  std::vector<bool> isIncreasing(streetSize, false);

  for (int current = 1; current < streetSize; ++current) {
    for (int previous = 0; previous < current; ++previous) {
      if ((isIncreasing[previous] && city[current] < city[previous]) ||
          (!isIncreasing[previous] && city[current] > city[previous])) {
        if (dp[current] < dp[previous] + 1) {
          dp[current] = dp[previous] + 1;
          prev[current] = previous; 
          isIncreasing[current] = !isIncreasing[previous];
          if (dp[current] > maxLength) {
            maxLength = dp[current];
            maxStreet = current;
          }
        }
      }
    }
  }
  return maxStreet;
}

std::vector<int> reconstructSequence(int currentIndex,
                                     const std::vector<int>& city,
                                     const std::vector<int>& prev) {
  std::vector<int> sequence;
  while (currentIndex != -1) {
    sequence.push_back(city[currentIndex]);
    currentIndex = prev[currentIndex];
  }
  std::reverse(sequence.begin(), sequence.end());
  return sequence;
}

int main() {
  int streetSize = 0;
  std::vector<int> city;
  scanf_data(streetSize, city);

  int maxLength1 = 1, maxLength2 = 1;
  std::vector<int> prev1(streetSize, -1), prev2(streetSize, -1);

  int endIndex1 = findLongestAlternatingSequence(prev1, city, maxLength1);

  std::vector<int> reversedCity = city;
  for (int& value : reversedCity) {
    value *= -1;
  }
  int endIndex2 =
      findLongestAlternatingSequence(prev2, reversedCity, maxLength2);

  int maxLength = std::max(maxLength1, maxLength2);
  std::vector<int> result = (maxLength1 > maxLength2)
                                ? reconstructSequence(endIndex1, city, prev1)
                                : reconstructSequence(endIndex2, city, prev2);

  print_data(maxLength, result);

  return 0;
}