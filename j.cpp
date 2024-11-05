#include <array>
#include <cmath>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>


class Puzzle {
 public:
  explicit Puzzle(const std::array<int, 9>& initial)
      : initial_state(initial), goal_state{1, 2, 3, 4, 5, 6, 7, 8, 0} {}

  int solve() {
    if (!is_solvable(initial_state)) {
      std::cout << -1 << std::endl;
      return -1;
    }

    std::priority_queue<Node, std::vector<Node>, NodeComparator> open_set;
    std::unordered_map<std::string, bool> closed_set;

    Node start_node(initial_state, "", 0,
                    get_manhattan_distance(initial_state));
    open_set.push(start_node);

    while (!open_set.empty()) {
      Node current = open_set.top();
      open_set.pop();

      if (current.state == goal_state) {
        std::cout << current.cost << std::endl;
        std::cout << current.path << std::endl;
        return current.cost;
      }

      closed_set[state_to_string(current.state)] = true;

      for (const auto& neighbor : get_neighbors(current)) {
        if (!closed_set[state_to_string(neighbor.state)]) {
          open_set.push(neighbor);
        }
      }
    }

    return -1;
  }

 private:
  struct Node {
    std::array<int, 9> state;
    std::string path;
    int cost;
    int heuristic;

    Node(const std::array<int, 9>& s, const std::string& p, int c, int h)
        : state(s), path(p), cost(c), heuristic(h) {}

    int get_f_score() const { return cost + heuristic; }
  };

  struct NodeComparator {
    bool operator()(const Node& a, const Node& b) const {
      return a.get_f_score() > b.get_f_score();
    }
  };

  std::array<int, 9> initial_state;
  const std::array<int, 9> goal_state;

  bool is_solvable(const std::array<int, 9>& state) const {
    int inversions = 0;
    for (int i = 0; i < 9; ++i) {
      if (state[i] == 0) continue;
      for (int j = i + 1; j < 9; ++j) {
        if (state[j] != 0 && state[i] > state[j]) {
          ++inversions;
        }
      }
    }
    return inversions % 2 == 0;
  }

  int get_manhattan_distance(const std::array<int, 9>& state) const {
    int distance = 0;
    for (int i = 0; i < 9; ++i) {
      if (state[i] != 0) {
        int target_x = (state[i] - 1) / 3;
        int target_y = (state[i] - 1) % 3;
        int current_x = i / 3;
        int current_y = i % 3;
        distance +=
            std::abs(current_x - target_x) + std::abs(current_y - target_y);
      }
    }
    return distance;
  }

  std::string state_to_string(const std::array<int, 9>& state) const {
    std::string s;
    for (int num : state) {
      s += std::to_string(num);
    }
    return s;
  }

  std::vector<Node> get_neighbors(const Node& current) const {
    std::vector<Node> neighbors;
    static const std::array<int, 4> dx = {-1, 1, 0, 0};
    static const std::array<int, 4> dy = {0, 0, -1, 1};
    static const std::array<char, 4> directions = {'U', 'D', 'L', 'R'};

    int zero_pos = 0;
    while (current.state[zero_pos] != 0) {
      ++zero_pos;
    }
    int x = zero_pos / 3;
    int y = zero_pos % 3;

    for (int i = 0; i < 4; ++i) {
      int new_x = x + dx[i];
      int new_y = y + dy[i];
      if (new_x >= 0 && new_x < 3 && new_y >= 0 && new_y < 3) {
        std::array<int, 9> new_state = current.state;
        std::swap(new_state[zero_pos], new_state[new_x * 3 + new_y]);
        neighbors.emplace_back(new_state, current.path + directions[i],
                               current.cost + 1,
                               get_manhattan_distance(new_state));
      }
    }

    return neighbors;
  }
};

int main() {
  std::array<int, 9> initial_state;
  for (int i = 0; i < 9; ++i) {
    std::cin >> initial_state[i];
  }

  Puzzle puzzle(initial_state);
  puzzle.solve();

  return 0;
}
