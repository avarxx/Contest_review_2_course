#include <algorithm>
#include <climits>
#include <concepts>
#include <functional>
#include <iostream>
#include <numeric>
#include <queue>
#include <type_traits>
#include <vector>

template <typename T>
concept Numeric = std::is_arithmetic_v<T> && !std::is_same_v<T, bool>;

template <typename T>
class Graph;
template <typename T>
class FlowNetwork;
template <typename T>
class FlowNetworkBuilder;

template <typename T>
struct ResellersEdge {
  int from;
  int to;
  T capacity;
  T flow;
  int reverse_index;

  ResellersEdge(int f, int t, T c, T fl, int ri)
      : from(f), to(t), capacity(c), flow(fl), reverse_index(ri) {}
};

template <typename T>
class GraphVisitor {
 public:
  virtual void discover_vertex(int v, const Graph<T>& g) = 0;
  virtual void examine_edge(const ResellersEdge<T>& e, const Graph<T>& g) = 0;
  virtual ~GraphVisitor() = default;
};

template <typename T>
class LevelCalculationVisitor : public GraphVisitor<T> {
 private:
  std::vector<int>& level;

 public:
  LevelCalculationVisitor(std::vector<int>& l) : level(l) {}

  void discover_vertex(int v, const Graph<T>& g) override {
    for (const auto& ResellersEdge : g.edges(v)) {
      if (level[ResellersEdge.to] == -1 && ResellersEdge.flow < ResellersEdge.capacity) {
        level[ResellersEdge.to] = level[v] + 1;
      }
    }
  }

  void examine_edge(const ResellersEdge<T>& e, const Graph<T>& g) override {}
};

template <typename T>
class Graph {
 protected:
  int vertex_count;
  std::vector<std::vector<ResellersEdge<T>>> adjacency_list;

 public:
  explicit Graph(int n) : vertex_count(n), adjacency_list(n) {}

  void add_edge(int from, int to, T capacity) {
    ResellersEdge<T> forward(from, to, capacity, T{}, adjacency_list[to].size());
    ResellersEdge<T> backward(to, from, T{}, T{}, adjacency_list[from].size());

    adjacency_list[from].push_back(forward);
    adjacency_list[to].push_back(backward);
  }

  const std::vector<ResellersEdge<T>>& edges(int v) const { return adjacency_list[v]; }

  std::vector<ResellersEdge<T>>& edges(int v) { return adjacency_list[v]; }

  int size() const { return vertex_count; }
};

template <typename T>
class BFS {
 public:
  static void traverse(const Graph<T>& g, int start, GraphVisitor<T>& visitor) {
    std::vector<bool> visited(g.size(), false);
    std::queue<int> q;

    q.push(start);
    visited[start] = true;

    while (!q.empty()) {
      int v = q.front();
      q.pop();

      visitor.discover_vertex(v, g);

      for (const auto& ResellersEdge : g.edges(v)) {
        if (!visited[ResellersEdge.to] && ResellersEdge.flow < ResellersEdge.capacity) {
          visited[ResellersEdge.to] = true;
          q.push(ResellersEdge.to);
        }
      }
    }
  }
};

template <typename T>
class FlowNetwork : public Graph<T> {
 private:
  std::vector<int> level;
  std::vector<int> ptr;

  bool calculate_levels(int source, int sink) {
    std::fill(level.begin(), level.end(), -1);
    level[source] = 0;

    LevelCalculationVisitor<T> visitor(level);
    BFS<T>::traverse(*this, source, visitor);

    return level[sink] != -1;
  }

  T dfs(int vertex, int sink, T flow) {
    if (vertex == sink || flow == T{}) return flow;

    for (; ptr[vertex] < this->adjacency_list[vertex].size(); ++ptr[vertex]) {
      ResellersEdge<T>& ResellersEdge = this->adjacency_list[vertex][ptr[vertex]];

      if (level[ResellersEdge.to] == level[vertex] + 1 && ResellersEdge.flow < ResellersEdge.capacity) {
        T pushed =
            dfs(ResellersEdge.to, sink, std::min(flow, ResellersEdge.capacity - ResellersEdge.flow));

        if (pushed > T{}) {
          ResellersEdge.flow += pushed;
          this->adjacency_list[ResellersEdge.to][ResellersEdge.reverse_index].flow -= pushed;
          return pushed;
        }
      }
    }
    return T{};
  }

 public:
  explicit FlowNetwork(int n) : Graph<T>(n), level(n), ptr(n) {}

  T max_flow(int source, int sink) {
    T total_flow = T{};

    while (calculate_levels(source, sink)) {
      std::fill(ptr.begin(), ptr.end(), 0);
      while (T flow = dfs(source, sink, std::numeric_limits<T>::max())) {
        total_flow += flow;
      }
    }
    return total_flow;
  }
};

template <typename T>
class FlowNetworkBuilder {
 private:
  int vertex_count;
  std::vector<std::pair<int, int>> edges;
  std::vector<T> vertex_capacities;

 public:
  explicit FlowNetworkBuilder(int n) : vertex_count(n), vertex_capacities(n) {}

  void add_edge(int from, int to) { edges.emplace_back(from, to); }

  void set_vertex_capacity(int vertex, T capacity) {
    vertex_capacities[vertex] = capacity;
  }

  FlowNetwork<T> build() const {
    FlowNetwork<T> network(vertex_count + 2);
    int source = vertex_count;
    int sink = vertex_count + 1;

    for (int i = 0; i < vertex_count; ++i) {
      network.add_edge(source, i, vertex_capacities[i]);
    }

    for (const auto& [from, to] : edges) {
      network.add_edge(from, to, std::numeric_limits<T>::max() / 2);
    }

    return network;
  }
};

template <typename T, typename Predicate>
T binary_search(T left, T right, Predicate pred) {
  T result = right;
  while (left <= right) {
    T mid = left + (right - left) / 2;
    if (pred(mid)) {
      result = mid;
      right = mid - 1;
    } else {
      left = mid + 1;
    }
  }
  return result;
}

struct InputData {
  int vertex_count;
  int edge_count;
  std::vector<int> vertex_capacities;
  std::vector<std::pair<int, int>> edges;
};

struct MacbookDistributionProblem {
  FlowNetworkBuilder<int> builder;
  int total_capacity;

  explicit MacbookDistributionProblem(const InputData& data)
      : builder(data.vertex_count),
        total_capacity(std::accumulate(data.vertex_capacities.begin(),
                                       data.vertex_capacities.end(), 0)) {
    initialize_builder(data);
  }

 private:
  void initialize_builder(const InputData& data) {
    for (int i = 0; i < data.vertex_count; ++i) {
      builder.set_vertex_capacity(i, data.vertex_capacities[i]);
    }

    for (const auto& [from, to] : data.edges) {
      builder.add_edge(from, to);
    }
  }
};

class MacbookDistributionSolver {
 public:
  static int find_min_max_load(const MacbookDistributionProblem& MacbookDistributionProblem) {
    return binary_search(1, MacbookDistributionProblem.total_capacity,
                         [&](int mid) { return check_flow(MacbookDistributionProblem, mid); });
  }

 private:
  static bool check_flow(const MacbookDistributionProblem& MacbookDistributionProblem, int capacity) {
    auto network = MacbookDistributionProblem.builder.build();
    int n = network.size() - 2;
    int source = n;
    int sink = n + 1;

    for (int i = 0; i < n; ++i) {
      network.add_edge(i, sink, capacity);
    }

    return network.max_flow(source, sink) == MacbookDistributionProblem.total_capacity;
  }
};

class InputReader {
 public:
  static InputData read() {
    InputData data;
    std::cin >> data.vertex_count >> data.edge_count;

    read_vertex_capacities(data);
    read_edges(data);

    return data;
  }

 private:
  static void read_vertex_capacities(InputData& data) {
    data.vertex_capacities.resize(data.vertex_count);
    for (int i = 0; i < data.vertex_count; ++i) {
      std::cin >> data.vertex_capacities[i];
    }
  }

  static void read_edges(InputData& data) {
    for (int i = 0; i < data.edge_count; ++i) {
      int from, to;
      std::cin >> from >> to;
      data.edges.emplace_back(from - 1, to - 1);
    }
  }
};

int main() {
  InputData input = InputReader::read();
  MacbookDistributionProblem MacbookDistributionProblem(input);
  int result = MacbookDistributionSolver::find_min_max_load(MacbookDistributionProblem);
  std::cout << result << "\n";
  return 0;
}