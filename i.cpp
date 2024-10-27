#pragma GCC optimize("Ofast,no-stack-protector")
#include <iostream>
#include <string>
#include <vector>

long long pow2(long long n) { return 1 << n; }

struct InputData {
  long long n;
  std::vector<long long> in_first;
  std::vector<long long> in_second;
  std::vector<long long> bit;
};

InputData readInput() {
  InputData data;
  std::cin >> data.n;

  long long h = data.n / 2;
  data.in_first.resize(h);
  data.in_second.resize(data.n - h);
  data.bit.resize(h);

  std::string s;
  for (long long i = 0; i < data.n; ++i) {
    std::cin >> s;
    for (long long j = 0; j < data.n; ++j) {
      if (s[j] != '1') continue;

      if (i < h && j < h)
        data.in_first[i] += pow2(j);
      else if (i >= h && j >= h)
        data.in_second[i - h] += pow2(j - h);
      else if (i < h && j >= h)
        data.bit[i] += pow2(j - h);
    }
  }
  return data;
}

std::vector<bool> processFirst(const std::vector<long long>& in_first,
                               long long h) {
  std::vector<bool> first(pow2(h), true);
  long long old = -1, temp, help;

  for (long long mask = 1; mask < pow2(h); ++mask) {
    if (!(mask & (mask - 1))) {
      ++old;
      continue;
    }

    help = mask - pow2(old);
    if (!first[help]) {
      first[mask] = false;
      continue;
    }

    temp = help ^ in_first[old];
    if (temp != in_first[old] - help) {
      first[mask] = false;
    }
  }
  first[0] = true;
  return first;
}

std::vector<int> processSecond(const std::vector<long long>& in_second,
                               long long n) {
  long long h = n - n / 2;
  std::vector<int> second(pow2(h), 1);
  long long old = -1, temp, help;

  for (long long mask = 1; mask < pow2(h); ++mask) {
    if (!(mask & (mask - 1))) {
      ++old;
      continue;
    }

    help = mask - pow2(old);
    temp = help ^ in_second[old];
    if ((temp != in_second[old] - help) || (!second[help])) second[mask] = 0;
  }

  second[0] = 0;

  for (long long i = 0; i < h; ++i) {
    for (long long mask = 1; mask < pow2(h); ++mask) {
      if (!((mask >> i) % 2)) {
        second[mask + pow2(i)] += second[mask];
      }
    }
  }

  return second;
}

std::vector<long long> count(long long n,
                             const std::vector<long long>& bit) {
  std::vector<long long> all(pow2(n / 2), 0);
  all[0] = pow2(n - n / 2) - 1;
  long long old = -1;

  for (long long mask = 1; mask < pow2(n / 2); ++mask) {
    if (!(mask & (mask - 1)))
      all[mask] = bit[++old];
    else
      all[mask] = all[mask - pow2(old)] & bit[old];
  }
  return all;
}

size_t get(const std::vector<bool>& first, const std::vector<int>& second,
           const std::vector<long long>& all, long long n) {
  size_t ans = 0;
  for (long long mask = 0; mask < pow2(n / 2); ++mask) {
    if (first[mask]) ans += second[all[mask]] + 1;
  }
  return ans;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cout.tie(0);
  std::cin.tie(0);

  InputData data = readInput();

  auto first = processFirst(data.in_first, data.n / 2);
  auto second = processSecond(data.in_second, data.n);
  auto all = count(data.n, data.bit);

  std::cout << get(first, second, all, data.n) << '\n';

  return 0;
}