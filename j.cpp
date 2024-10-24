#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

void print_matching_positions(
    const std::vector<std::pair<size_t, size_t>>& positions) {
  std::cout << positions.size() << "\n";
  for (const auto& [first_pos, second_pos] : positions) {
    std::cout << first_pos << " ";
  }
  std::cout << "\n";
  for (const auto& [first_pos, second_pos] : positions) {
    std::cout << second_pos << " ";
  }
  std::cout << "\n";
}

std::vector<std::pair<size_t, size_t>> solve_lcs(
    const std::string& first_str, const std::string& second_str) {
  const size_t first_length = first_str.size();
  const size_t second_length = second_str.size();

  std::vector<std::vector<size_t>> lcs_lengths(
      first_length + 1, std::vector<size_t>(second_length + 1, 0));

  for (size_t i = 1; i <= first_length; ++i) {
    for (size_t j = 1; j <= second_length; ++j) {
      if (first_str[i - 1] == second_str[j - 1]) {
        lcs_lengths[i][j] = lcs_lengths[i - 1][j - 1] + 1;
      } else {
        lcs_lengths[i][j] =
            std::max(lcs_lengths[i - 1][j], lcs_lengths[i][j - 1]);
      }
    }
  }

  std::vector<std::pair<size_t, size_t>> matching_positions;
  size_t i = first_length;
  size_t j = second_length;

  while (i > 0 && j > 0) {
    if (first_str[i - 1] == second_str[j - 1]) {
      matching_positions.push_back({i, j});
      --i;
      --j;
    } else if (lcs_lengths[i - 1][j] > lcs_lengths[i][j - 1]) {
      --i;
    } else {
      --j;
    }
  }

  std::reverse(matching_positions.begin(), matching_positions.end());
  return matching_positions;
}

int main() {
  std::string first_str, second_str;
  std::cin >> first_str >> second_str;

  auto matching_positions = solve_lcs(first_str, second_str);
  print_matching_positions(matching_positions);

  return 0;
}