#include <iostream>
#include <iterator>
#include <string>
#include <vector>

template <typename ForwardIt>
std::vector<int> calculatePrefixFunction(ForwardIt first, ForwardIt last) {
    using Index = typename std::iterator_traits<ForwardIt>::difference_type;
    const Index n = std::distance(first, last);
    std::vector<int> prefix(n, 0);
    for (Index i = 1; i < n; ++i) {
        int j = prefix[i - 1];
        while (j > 0 && *(first + i) != *(first + j)) {
            j = prefix[j - 1];
        }
        if (*(first + i) == *(first + j)) {
            ++j;
        }
        prefix[i] = j;
    }
    return prefix;
}

template <typename PatternIt, typename TextIt>
std::vector<typename std::iterator_traits<TextIt>::difference_type>
findPatternOccurrences(PatternIt pat_first, PatternIt pat_last,
                       TextIt text_first, TextIt text_last) {
    using Index = typename std::iterator_traits<TextIt>::difference_type;
    const Index m = std::distance(pat_first, pat_last);
    if (m == 0) return {};

    auto prefix = calculatePrefixFunction(pat_first, pat_last);
    std::vector<Index> occurrences;
    Index matched = 0;
    for (Index i = 0; i < std::distance(text_first, text_last); ++i) {
        while (matched > 0 && *(pat_first + matched) != *(text_first + i)) {
            matched = prefix[matched - 1];
        }
        if (*(pat_first + matched) == *(text_first + i)) {
            ++matched;
        }
        if (matched == m) {
            occurrences.push_back(i - m + 1);
            matched = prefix[matched - 1];
        }
    }
    return occurrences;
}

std::vector<int> findPatternOccurrences(const std::string& pattern,
                                        const std::string& text) {
    auto raw_occurrences = findPatternOccurrences(
        pattern.begin(), pattern.end(), text.begin(), text.end());
    return std::vector<int>(raw_occurrences.begin(), raw_occurrences.end());
}

void printVector(const std::vector<int>& vec) {
    for (int idx : vec) {
        std::cout << idx << "\n";
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text, pattern;
    std::cin >> text >> pattern;
    auto result = findPatternOccurrences(pattern, text);
    printVector(result);
    return 0;
}