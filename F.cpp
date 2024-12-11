#include <iostream>
#include <vector>

void GetDFSed(size_t node, std::vector<std::vector<size_t>>& matrix,
              size_t& the_v, std::vector<bool>& visited,
              std::vector<std::vector<size_t>>& ans) {
  visited[node] = true;
  for (size_t xtmp = 0; xtmp < the_v; ++xtmp) {
    if (matrix[node][xtmp] != static_cast<size_t>(0) && !visited[xtmp]) {
      GetDFSed(xtmp, matrix, the_v, visited, ans);
    }
  }
}

std::vector<std::vector<size_t>> inputMatrix() {
  size_t the_v;
  std::cin >> the_v;
  std::vector<std::vector<size_t>> matrix(the_v, std::vector<size_t>(the_v));

  for (size_t row = 0; row < the_v; ++row) {
    for (size_t col = 0; col < the_v; ++col) {
      std::cin >> matrix[row][col];
    }
  }

  return matrix;
}

std::vector<std::vector<size_t>> computeReachabilityMatrix(
    const std::vector<std::vector<size_t>>& matrix) {
  size_t the_v = matrix.size();
  std::vector<std::vector<size_t>> ans(the_v, std::vector<size_t>(the_v, 0));

  for (size_t start_vertex = 0; start_vertex < the_v; ++start_vertex) {
    std::vector<bool> visited(the_v, false);
    GetDFSed(start_vertex,
             const_cast<std::vector<std::vector<size_t>>&>(matrix), the_v,
             visited, ans);

    for (size_t dest_vertex = 0; dest_vertex < the_v; ++dest_vertex) {
      if (visited[dest_vertex]) {
        ans[start_vertex][dest_vertex] = 1;
      }
    }
  }

  return ans;
}

void outputMatrix(const std::vector<std::vector<size_t>>& matrix) {
  for (const auto& row : matrix) {
    for (size_t val : row) {
      std::cout << val << " ";
    }
    std::cout << '\n';
  }
}

int main() {
  auto matrix = inputMatrix();

  auto reachabilityMatrix = computeReachabilityMatrix(matrix);

  outputMatrix(reachabilityMatrix);

  return 0;
}