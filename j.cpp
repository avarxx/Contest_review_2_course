#include <array>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <algorithm>

class EightPuzzleSolver {
 public:
  explicit EightPuzzleSolver(const std::array<int32_t, 9>& initial)
      : initial_board(initial), target_board{1, 2, 3, 4, 5, 6, 7, 8, 0} {}

  int32_t solve() {
    if (!can_solve_puzzle(initial_board)) {
      std::cout << -1 << '\n';
      return -1;
    }

    std::priority_queue<BoardState, std::vector<BoardState>, BoardStateComparator> open_queue;
    std::unordered_map<std::string, bool> visited_states;

    BoardState start(initial_board, "", 0, calculate_manhattan_distance(initial_board));
    open_queue.push(start);

    while (!open_queue.empty()) {
      BoardState current = open_queue.top();
      open_queue.pop();

      if (current.state == target_board) {
        std::cout << current.cost << ' ' << current.path << '\n';
        return current.cost;
      }

      visited_states[board_to_string(current.state)] = true;

      for (const auto& next_state : generate_next_states(current)) {
        if (!visited_states[board_to_string(next_state.state)]) {
          open_queue.push(next_state);
        }
      }
    }

    return -1;
  }

 private:
  struct BoardState {
    std::array<int32_t, 9> state;
    std::string path;
    int32_t cost;
    int32_t heuristic;

    BoardState(const std::array<int32_t, 9>& s, const std::string& p, int32_t c, int32_t h)
        : state(s), path(p), cost(c), heuristic(h) {}

    int32_t get_total_cost() const { return cost + heuristic; }
  };

  struct BoardStateComparator {
    bool operator()(const BoardState& a, const BoardState& b) const {
      return a.get_total_cost() > b.get_total_cost();
    }
  };

  std::array<int32_t, 9> initial_board;
  const std::array<int32_t, 9> target_board;

  bool can_solve_puzzle(const std::array<int32_t, 9>& board) const {
    int32_t inversions = 0;
    for (size_t i = 0; i < 9; ++i) {
      if (board[i] == 0) continue;
      for (size_t j = i + 1; j < 9; ++j) {
        if (board[j] != 0 && board[i] > board[j]) {
          ++inversions;
        }
      }
    }
    return inversions % 2 == 0;
  }

  int32_t calculate_manhattan_distance(const std::array<int32_t, 9>& board) const {
    int32_t distance = 0;
    for (size_t i = 0; i < 9; ++i) {
      if (board[i] != 0) {
        int32_t target_x = (board[i] - 1) / 3;
        int32_t target_y = (board[i] - 1) % 3;
        int32_t current_x = static_cast<int32_t>(i) / 3;
        int32_t current_y = static_cast<int32_t>(i) % 3;
        distance += std::abs(current_x - target_x) + std::abs(current_y - target_y);
      }
    }
    return distance;
  }

  std::string board_to_string(const std::array<int32_t, 9>& board) const {
    std::string result;
    for (int32_t num : board) {
      result += std::to_string(num);
    }
    return result;
  }

  std::vector<BoardState> generate_next_states(const BoardState& current) const {
    std::vector<BoardState> next_states;
    static const std::array<int32_t, 4> dx = {-1, 1, 0, 0};
    static const std::array<int32_t, 4> dy = {0, 0, -1, 1};
    static const std::array<char, 4> directions = {'U', 'D', 'L', 'R'};

    size_t zero_pos = 0;
    while (current.state[zero_pos] != 0) {
      ++zero_pos;
    }
    int32_t x = static_cast<int32_t>(zero_pos) / 3;
    int32_t y = static_cast<int32_t>(zero_pos) % 3;

    for (size_t i = 0; i < 4; ++i) {
      int32_t new_x = x + dx[i];
      int32_t new_y = y + dy[i];
      if (new_x >= 0 && new_x < 3 && new_y >= 0 && new_y < 3) {
        std::array<int32_t, 9> new_board = current.state;
        std::swap(new_board[zero_pos], new_board[static_cast<size_t>(new_x * 3 + new_y)]);
        next_states.emplace_back(
            new_board, 
            current.path + directions[i], 
            current.cost + 1, 
            calculate_manhattan_distance(new_board)
        );
      }
    }

    return next_states;
  }
};

int main() {
  std::array<int32_t, 9> initial_board;
  for (size_t i = 0; i < 9; ++i) {
    std::cin >> initial_board[i];
  }

  EightPuzzleSolver puzzle_solver(initial_board);
  puzzle_solver.solve();

  return 0;
}