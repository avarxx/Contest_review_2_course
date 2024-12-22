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

  ResellersEdge(int source, int target, T edge_capacity, T edge_flow, int reverse_idx)
      : from(source), to(target), capacity(edge_capacity), flow(edge_flow), 
        reverse_index(reverse_idx) {}
};

template <typename T>
class GraphVisitor {
public:
  virtual void discover_vertex(int vertex, const Graph<T>& graph) = 0;
  virtual void examine_edge(const ResellersEdge<T>& edge, const Graph<T>& graph) = 0;
  virtual ~GraphVisitor() = default;
};

template <typename T>
class LevelCalculationVisitor : public GraphVisitor<T> {
private:
  std::vector<int>& level;

public:
  LevelCalculationVisitor(std::vector<int>& vertex_levels) : level(vertex_levels) {}

  void discover_vertex(int vertex, const Graph<T>& graph) override {
    for (const auto& edge : graph.edges(vertex)) {
      if (level[edge.to] == -1 && edge.flow < edge.capacity) {
        level[edge.to] = level[vertex] + 1;
      }
    }
  }

  void examine_edge(const ResellersEdge<T>& edge, const Graph<T>& graph) override {}
};

template <typename T>
class Graph {
protected:
  int vertex_count;
  std::vector<std::vector<ResellersEdge<T>>> adjacency_list;

public:
  explicit Graph(int vertices) : vertex_count(vertices), adjacency_list(vertices) {}

  void add_edge(int from, int to, T capacity) {
    ResellersEdge<T> forward(from, to, capacity, T{}, adjacency_list[to].size());
    ResellersEdge<T> backward(to, from, T{}, T{}, adjacency_list[from].size());

    adjacency_list[from].push_back(forward);
    adjacency_list[to].push_back(backward);
  }

  const std::vector<ResellersEdge<T>>& edges(int vertex) const { return adjacency_list[vertex]; }
  std::vector<ResellersEdge<T>>& edges(int vertex) { return adjacency_list[vertex]; }
  int size() const { return vertex_count; }
};

template <typename T>
class BFS {
public:
  static void traverse(const Graph<T>& graph, int start, GraphVisitor<T>& visitor) {
    std::vector<bool> visited(graph.size(), false);
    std::queue<int> vertex_queue;

    vertex_queue.push(start);
    visited[start] = true;

    while (!vertex_queue.empty()) {
      int vertex = vertex_queue.front();
      vertex_queue.pop();

      visitor.discover_vertex(vertex, graph);

      for (const auto& edge : graph.edges(vertex)) {
        if (!visited[edge.to] && edge.flow < edge.capacity) {
          visited[edge.to] = true;
          vertex_queue.push(edge.to);
        }
      }
    }
  }
};

template <typename T>
class FlowNetwork : public Graph<T> {
private:
  std::vector<int> level;
  std::vector<int> pointer;

  bool calculate_levels(int source, int sink) {
    std::fill(level.begin(), level.end(), -1);
    level[source] = 0;

    LevelCalculationVisitor<T> visitor(level);
    BFS<T>::traverse(*this, source, visitor);

    return level[sink] != -1;
  }

  T dfs(int vertex, int sink, T flow_amount) {
    if (vertex == sink || flow_amount == T{}) return flow_amount;

    for (; pointer[vertex] < this->adjacency_list[vertex].size(); ++pointer[vertex]) {
      ResellersEdge<T>& edge = this->adjacency_list[vertex][pointer[vertex]];

      if (level[edge.to] == level[vertex] + 1 && edge.flow < edge.capacity) {
        T pushed = dfs(edge.to, sink, std::min(flow_amount, edge.capacity - edge.flow));

        if (pushed > T{}) {
          edge.flow += pushed;
          this->adjacency_list[edge.to][edge.reverse_index].flow -= pushed;
          return pushed;
        }
      }
    }
    return T{};
  }

public:
  explicit FlowNetwork(int vertices) 
      : Graph<T>(vertices), level(vertices), pointer(vertices) {}

  T max_flow(int source, int sink) {
    T total_flow = T{};

    while (calculate_levels(source, sink)) {
      std::fill(pointer.begin(), pointer.end(), 0);
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
  explicit FlowNetworkBuilder(int vertices) 
      : vertex_count(vertices), vertex_capacities(vertices) {}

  void add_edge(int from, int to) { edges.emplace_back(from, to); }

  void set_vertex_capacity(int vertex, T capacity) {
    vertex_capacities[vertex] = capacity;
  }

  FlowNetwork<T> build() const {
    FlowNetwork<T> network(vertex_count + 2);
    int source = vertex_count;
    int sink = vertex_count + 1;

    for (int vertex = 0; vertex < vertex_count; ++vertex) {
      network.add_edge(source, vertex, vertex_capacities[vertex]);
    }

    for (const auto& [from, to] : edges) {
      network.add_edge(from, to, std::numeric_limits<T>::max() / 2);
    }

    return network;
  }
};

template <typename T, typename Predicate>
T binary_search(T left, T right, Predicate predicate) {
  T result = right;
  while (left <= right) {
    T mid = left + (right - left) / 2;
    if (predicate(mid)) {
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

  explicit MacbookDistributionProblem(const InputData& input)
      : builder(input.vertex_count),
        total_capacity(std::accumulate(input.vertex_capacities.begin(),
                                     input.vertex_capacities.end(), 0)) {
    initialize_builder(input);
  }

private:
  void initialize_builder(const InputData& input) {
    for (int vertex = 0; vertex < input.vertex_count; ++vertex) {
      builder.set_vertex_capacity(vertex, input.vertex_capacities[vertex]);
    }

    for (const auto& [from, to] : input.edges) {
      builder.add_edge(from, to);
    }
  }
};

class MacbookDistributionSolver {
public:
  static int find_min_max_load(const MacbookDistributionProblem& problem) {
    return binary_search(1, problem.total_capacity,
                        [&](int max_load) { return check_flow(problem, max_load); });
  }

private:
  static bool check_flow(const MacbookDistributionProblem& problem, int capacity) {
    auto network = problem.builder.build();
    int vertices = network.size() - 2;
    int source = vertices;
    int sink = vertices + 1;

    for (int vertex = 0; vertex < vertices; ++vertex) {
      network.add_edge(vertex, sink, capacity);
    }

    return network.max_flow(source, sink) == problem.total_capacity;
  }
};

class InputReader {
public:
  static InputData read() {
    InputData input;
    std::cin >> input.vertex_count >> input.edge_count;

    read_vertex_capacities(input);
    read_edges(input);

    return input;
  }

private:
  static void read_vertex_capacities(InputData& input) {
    input.vertex_capacities.resize(input.vertex_count);
    for (int vertex = 0; vertex < input.vertex_count; ++vertex) {
      std::cin >> input.vertex_capacities[vertex];
    }
  }

  static void read_edges(InputData& input) {
    for (int edge = 0; edge < input.edge_count; ++edge) {
      int from, to;
      std::cin >> from >> to;
      input.edges.emplace_back(from - 1, to - 1);
    }
  }
};

int main() {
  InputData input = InputReader::read();
  MacbookDistributionProblem problem(input);
  int result = MacbookDistributionSolver::find_min_max_load(problem);
  std::cout << result << "\n";
  return 0;
}