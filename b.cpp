#include <iostream>
#include <vector>

const unsigned long long mod = 1000003;

std::vector<std::vector<unsigned long long>> matrix_multiply(
    const std::vector<std::vector<unsigned long long>>& matrix1,
    const std::vector<std::vector<unsigned long long>>& matrix2) {
  std::vector<std::vector<unsigned long long>> result(
      5, std::vector<unsigned long long>(5, 0));
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      for (int n = 0; n < 5; n++) {
        result[i][j] =
            (result[i][j] + (matrix1[i][n] * matrix2[n][j]) % mod) % mod;
      }
    }
  }
  return result;
}

std::vector<std::vector<unsigned long long>> fast_deg(
    std::vector<std::vector<unsigned long long>>& matrix,
    unsigned unsigned long long deg) {
  std::vector<std::vector<unsigned long long>> results(
      5, std::vector<unsigned long long>(5, 0));
  for (int i = 0; i < 5; i++) {
    results[i][i] = 1;
  }

  while (deg > 0) {
    if (deg % 2) {
      results = matrix_multiply(results, matrix);
    }
    matrix = matrix_multiply(matrix, matrix);
    deg /= 2;
  }
  return results;
}

unsigned unsigned long long count_hops(unsigned long long n) {
  unsigned long long f5 = 8, f4 = 4, f3 = 2, f2 = 1, f1 = 1;

  std::vector<std::vector<unsigned long long>> perexod_matrix = {
      {1, 1, 1, 1, 1},
      {1, 0, 0, 0, 0},
      {0, 1, 0, 0, 0},
      {0, 0, 1, 0, 0},
      {0, 0, 0, 1, 0}};
  if (n <= 0) return 0;
  switch (n) {
    case 1:
      return f1;
    case 2:
      return f2;
    case 3:
      return f3;
    case 4:
      return f4;
    case 5:
      return f5;
    default:
      break;
  }

  std::vector<std::vector<unsigned long long>> matrix =
      fast_deg(perexod_matrix, n - 5);

  unsigned unsigned long long fn =
      (matrix[0][0] * f5 % mod + matrix[0][1] * f4 % mod +
       matrix[0][2] * f3 % mod + matrix[0][3] * f2 % mod +
       matrix[0][4] * f1 % mod) %
      mod;
  return fn;
}

int main() {
  unsigned unsigned long long n = 0;
  std::cin >> n;
  std::cout << count_hops(n) << "\n";
  return 0;
}
