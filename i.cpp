#include <algorithm>
#include <iostream>
#include <vector>

enum class SequenceType {
  INCREASING_FIRST,  // Начинается с возрастания
  DECREASING_FIRST   // Начинается с убывания
};

struct SequenceElement {
  size_t length;
  int previous_index;
  bool is_increasing;

  SequenceElement() : length(1), previous_index(-1), is_increasing(false) {}
};

struct AlternatingSequence {
  std::vector<int> elements;
  size_t length;

  AlternatingSequence(std::vector<int> seq, size_t len)
      : elements(std::move(seq)), length(len) {}
};

std::vector<int> read_street_heights() {
  size_t street_size;
  std::cin >> street_size;

  std::vector<int> heights(street_size);
  for (auto& height : heights) {
    std::cin >> height;
  }
  return heights;
}

AlternatingSequence find_longest_alternating_sequence(
    const std::vector<int>& heights, SequenceType type) {
  const size_t size = heights.size();
  std::vector<SequenceElement> dp(size);

  size_t max_length = 1;
  size_t end_index = 0;

  for (auto& element : dp) {
    element.is_increasing = (type == SequenceType::INCREASING_FIRST);
  }

  for (size_t current = 1; current < size; ++current) {
    for (size_t previous = 0; previous < current; ++previous) {
      const bool comparison = (dp[previous].is_increasing)
                                  ? heights[current] < heights[previous]
                                  : heights[current] > heights[previous];

      if (comparison && dp[current].length < dp[previous].length + 1) {
        dp[current].length = dp[previous].length + 1;
        dp[current].previous_index = previous;
        dp[current].is_increasing = !dp[previous].is_increasing;

        if (dp[current].length > max_length) {
          max_length = dp[current].length;
          end_index = current;
        }
      }
    }
  }

  std::vector<int> sequence;
  for (size_t i = end_index; i != static_cast<size_t>(-1);
       i = dp[i].previous_index) {
    sequence.push_back(heights[i]);
  }
  std::reverse(sequence.begin(), sequence.end());

  return AlternatingSequence(sequence, max_length);
}

AlternatingSequence find_best_alternating_sequence(
    const std::vector<int>& heights) {
  auto increasing_first = find_longest_alternating_sequence(
      heights, SequenceType::INCREASING_FIRST);
  auto decreasing_first = find_longest_alternating_sequence(
      heights, SequenceType::DECREASING_FIRST);

  return (increasing_first.length >= decreasing_first.length)
             ? increasing_first
             : decreasing_first;
}

void print_sequence(const AlternatingSequence& sequence) {
  std::cout << sequence.length << '\n';
  for (int height : sequence.elements) {
    std::cout << height << ' ';
  }
  std::cout << '\n';
}

int main() {
  auto heights = read_street_heights();
  auto best_sequence = find_best_alternating_sequence(heights);
  print_sequence(best_sequence);
  return 0;
}