#include <array>
#include <iostream>

template <unsigned long long MOD>
class ModInt {
  unsigned long long value;

 public:
  ModInt(unsigned long long val = 0) : value(val % MOD) {}

  ModInt& operator+=(const ModInt& other) {
    value = (value + other.value) % MOD;
    return *this;
  }

  ModInt& operator*=(const ModInt& other) {
    value = (value * other.value) % MOD;
    return *this;
  }

  ModInt operator+(const ModInt& other) const { return ModInt(*this) += other; }

  ModInt operator*(const ModInt& other) const { return ModInt(*this) *= other; }

  operator unsigned long long() const { return value; }
};

template <typename T, size_t N>
class Matrix {
  std::array<std::array<T, N>, N> data;

 public:
  Matrix() {
    for (size_t i = 0; i < N; i++) {
      for (size_t j = 0; j < N; j++) {
        data[i][j] = (i == j) ? T(1) : T(0);
      }
    }
  }

  Matrix(const std::initializer_list<std::initializer_list<T>>& init) {
    size_t i = 0;
    for (const auto& row : init) {
      size_t j = 0;
      for (const auto& val : row) {
        data[i][j] = val;
        j++;
      }
      i++;
    }
  }

  Matrix operator*(const Matrix& other) const {
    Matrix result;
    for (size_t i = 0; i < N; i++) {
      for (size_t j = 0; j < N; j++) {
        result.data[i][j] = T(0);
        for (size_t k = 0; k < N; k++) {
          result.data[i][j] += data[i][k] * other.data[k][j];
        }
      }
    }
    return result;
  }

  const std::array<T, N>& operator[](size_t i) const { return data[i]; }
};

template <typename T, size_t N>
Matrix<T, N> fast_power(Matrix<T, N> base, unsigned long long exp) {
  Matrix<T, N> result;

  while (exp > 0) {
    if (exp % 2 == 1) {
      result = result * base;
    }
    base = base * base;
    exp /= 2;
  }
  return result;
}

unsigned long long count_hops(unsigned long long n) {
  constexpr unsigned long long MOD = 1000003;
  constexpr size_t MATRIX_SIZE = 5;
  using ModMatrix = Matrix<ModInt<MOD>, MATRIX_SIZE>;

  std::array<ModInt<MOD>, MATRIX_SIZE> f = {1, 1, 2, 4, 8};

  if (n <= 0) return 0;
  if (n <= MATRIX_SIZE) return f[n - 1];

  ModMatrix transition_matrix = {{{1, 1, 1, 1, 1},
                                  {1, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 0},
                                  {0, 0, 0, 1, 0}}};

  ModMatrix result = fast_power(transition_matrix, n - MATRIX_SIZE);

  ModInt<MOD> fn = 0;
  for (size_t i = 0; i < MATRIX_SIZE; i++) {
    fn += result[0][i] * f[MATRIX_SIZE - 1 - i];
  }

  return fn;
}

int main() {
  unsigned long long n = 0;
  std::cin >> n;
  std::cout << count_hops(n) << "\n";
  return 0;
}