#include <iostream>
#include <iterator>
#include <string>
#include <vector>

std::vector<int> prefixFunction(const std::string& str) {
    std::vector<int> p(str.size(), 0);
    for (size_t i = 1; i < str.size(); ++i) {
        int k = p[i - 1];
        while (k > 0 && str[i] != str[k]) {
            k = p[k - 1];
        }
        if (str[i] == str[k])
            ++k;
        p[i] = k;
    }
    return p;
}

template <typename RA>
std::vector<int> prefixFunction(RA first, RA last) {
    using D = typename std::iterator_traits<RA>::difference_type;
    D n = last - first;
    std::vector<int> p(n, 0);
    for (D i = 1; i < n; ++i) {
        int k = p[i - 1];
        while (k > 0 && first[i] != first[k]) {
            k = p[k - 1];
        }
        if (first[i] == first[k])
            ++k;
        p[i] = k;
    }
    return p;
}

template <typename PIt, typename TIt>
std::vector<typename std::iterator_traits<TIt>::difference_type> kmp(PIt p0, PIt p1, TIt t0,
                                                                     TIt t1) {
    using D = typename std::iterator_traits<TIt>::difference_type;
    D m = p1 - p0;
    if (m == 0)
        return {};
    auto pi = prefixFunction(p0, p1);
    std::vector<D> res;
    D q = 0;
    for (D i = 0; i < (t1 - t0); ++i) {
        while (q > 0 && p0[q] != t0[i]) {
            q = pi[q - 1];
        }
        if (p0[q] == t0[i])
            ++q;
        if (q == m) {
            res.push_back(i - m + 1);
            q = pi[q - 1];
        }
    }
    return res;
}

std::vector<int> kmp(const std::string& pattern, const std::string& text) {
    auto tmp = kmp(pattern.begin(), pattern.end(), text.begin(), text.end());
    return std::vector<int>(tmp.begin(), tmp.end());
}

void print(const std::vector<int>& result) {
    for (int x : result) {
        std::cout << x << "\n";
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text, pattern;
    std::cin >> text >> pattern;
    std::vector<int> result = kmp(pattern, text);
    print(result);
    return 0;
}
