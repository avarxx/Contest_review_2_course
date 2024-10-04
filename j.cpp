#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

std::pair<std::string, std::string> scanf_data() {
  std::string first_str, second_str;
  std::cin >> first_str >> second_str;
  return {first_str, second_str};
}

void print_answer(const std::vector<int>& first_index_vec, const std::vector<int>& second_index_vec) {
  std::cout << first_index_vec.size() << "\n";
  for (int tmp = first_index_vec.size() - 1; tmp >= 0; --tmp) {
    std::cout << first_index_vec[tmp] << " ";
  }
  std::cout << "\n";
  for (int tmp = second_index_vec.size() - 1; tmp >= 0; --tmp) {
    std::cout << second_index_vec[tmp] << " ";
  }
  std::cout << "\n";
}

std::pair<std::vector<int>, std::vector<int>> solve_lcs(const std::string& first_str, const std::string& second_str) {
  int first_str_size = first_str.size();
  int second_str_size = second_str.size();
  std::vector<std::vector<int>> dp(first_str_size + 1, std::vector<int>(second_str_size + 1, 0));
  for (int tmp = 1; tmp <= first_str_size; ++tmp) {
    for (int sec_ind = 1; sec_ind <= second_str_size; ++sec_ind) {
      if (first_str[tmp - 1] == second_str[sec_ind - 1]) {
        dp[tmp][sec_ind] = dp[tmp - 1][sec_ind - 1] + 1;
      } else {
        dp[tmp][sec_ind] = std::max(dp[tmp - 1][sec_ind], dp[tmp][sec_ind - 1]);
      }
    }
  }

  std::vector<int> first_index_vec, second_index_vec;
  int tmp = first_str_size;
  int sec_ind = second_str_size;
  while (tmp > 0 && sec_ind > 0) {
    if (first_str[tmp - 1] == second_str[sec_ind - 1]) {
      first_index_vec.push_back(tmp);
      second_index_vec.push_back(sec_ind);
      --tmp;
      --sec_ind;
    } else if (dp[tmp - 1][sec_ind] > dp[tmp][sec_ind - 1]) {
      --tmp;
    } else {
      --sec_ind;
    }
  }

  return {first_index_vec, second_index_vec};
}

int main() {
  auto [first_str, second_str] = scanf_data();
  auto [first_index_vec, second_index_vec] = solve_lcs(first_str, second_str);
  print_answer(first_index_vec, second_index_vec);
  return 0;
}
