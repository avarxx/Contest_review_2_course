#pragma GCC optimize("Ofast,no-stack-protector")

#include <iostream>
#include <vector>

long long pow2(long long n) { return 1 << n; }

std::vector<long long> count(long long n, const std::vector<long long>& bit) {
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

size_t get(std::vector<bool>& first, std::vector<int>& second,
           std::vector<long long>& all, long long n) {
  size_t ans = 0;

  for (long long mask = 0; mask < (pow2(n / 2)); ++mask) {
    if (first[mask]) ans += second[all[mask]] + 1;
  }

  return ans;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cout.tie(0);
  std::cin.tie(0);

  long long n;

  std::cin >> n;
  std::vector<long long> in_first(n / 2), in_second(n - n / 2), bit(n / 2);

  std::string s;

  long long h = n / 2;

  for (long long i = 0; i < n; ++i) {
    std::cin >> s;

    for (long long j = 0; j < n; ++j) {
      if (i < h && s[j] == '1' && j < h)
        in_first[i] += pow2(j);

      else if (i >= h && s[j] == '1' && j >= h)
        in_second[i - h] += pow2(j - h);

      else if (i < h && s[j] == '1' && j >= h)
        bit[i] += pow2(j - h);
    }
  }

  std::vector<bool> first(pow2(h), true);
  std::vector<int> second(pow2(n - h), 1);

  long long temp, old = -1, help;

  for (long long mask = 1; mask < pow2(h); ++mask) {
    if (!(mask & (mask - 1)))
      ++old;

    else {
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
  }

  old = -1;

  for (long long mask = 1; mask < pow2(n - n / 2); ++mask) {
    if (!(mask & (mask - 1)))
      ++old;

    else {
      help = mask - pow2(old);
      temp = help ^ in_second[old];

      if ((temp != in_second[old] - help) || (!second[help])) second[mask] = 0;
    }
  }

  first[0] = true;
  second[0] = 0;

  for (long long i = 0; i < n - h; ++i) {
    for (long long mask = 1; mask < pow2(n - h); ++mask) {
      if (!((mask >> i) % 2)) {
        second[mask + pow2(i)] += second[mask];
      }
    }
  }

  auto all = count(n, bit);

  std::cout << get(first, second, all, n) << '\n';
}