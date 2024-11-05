#include <iostream>
#include <vector>
#include <algorithm>

class Graph {
 public:
  explicit Graph(int n) : list_(n + 1), reversed_list_(n + 1), tout_sort_(n), used_(n + 1, false) {
    tout_.resize(n + 1);
    component_.resize(n + 1, 0);
    for (int i = 0; i < n; ++i) {
      tout_sort_[i] = i + 1;
    }
  }

  void Add(int first, int second) {
    list_[first].push_back(second);
    reversed_list_[second].push_back(first);
  }

  void FindComponents() {
    int timer = 0;
    int number = 0;
    for (int i = 1; i < static_cast<int>(list_.size()); ++i) {
      if (!used_[i]) {
        DFS(i, timer);
      }
    }
    std::sort(tout_sort_.begin(), tout_sort_.end(),
              [&](int first, int second) { return tout_[first] > tout_[second]; });
    for (auto vertex : tout_sort_) {
      if (!component_[vertex]) {
        ++number;
        ReversedDFS(vertex, number);
      }
    }
    num_components_ = number;
  }

  std::vector<int> GetComponents() const {
    return component_;
  }

  size_t NumComponents() const {
    return num_components_;
  }

 private:
  std::vector<std::vector<int>> list_;
  std::vector<std::vector<int>> reversed_list_;
  std::vector<int> tout_sort_;
  std::vector<bool> used_;
  std::vector<int> tout_;
  std::vector<int> component_;
  size_t num_components_;

  void DFS(int vertex, int& timer) {
    used_[vertex] = true;
    for (auto to : list_[vertex]) {
      if (!used_[to]) {
        DFS(to, timer);
      }
    }
    tout_[vertex] = timer++;
  }

  void ReversedDFS(int vertex, int number) {
    component_[vertex] = number;
    for (auto to : reversed_list_[vertex]) {
      if (!component_[to]) {
        ReversedDFS(to, number);
      }
    }
  }
};

int main() {
  int n;
  int m;
  std::cin >> n >> m;
  Graph graph(n);
  int first;
  int second;
  while (m--) {
    std::cin >> first >> second;
    graph.Add(first, second);
  }
  graph.FindComponents();
  std::vector<int> components = graph.GetComponents();
  std::cout << graph.NumComponents() << "\n";
  for (size_t i = 1; i < components.size(); ++i) {
    std::cout << components[i] << " ";
  }
}