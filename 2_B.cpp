#include <iostream>
#include <vector>
#include <cstddef> 

void compute_sum_of_lowest_prime_divisors(std::size_t n) {
    std::vector<std::size_t> lowest_prime_divisor(n + 1, 0);

    for (std::size_t i = 2; i <= n; ++i) {
        if (lowest_prime_divisor[i] == 0) {
            for (std::size_t j = i * i; j <= n; j += i) {
                if (lowest_prime_divisor[j] == 0) {
                    lowest_prime_divisor[j] = i;
                }
            }
            lowest_prime_divisor[i] = i;
        }
    }

    unsigned long long sum_of_lowest_prime_divisors = 0;
    for (std::size_t i = 3; i <= n; ++i) {
        if (lowest_prime_divisor[i] != i) {
            sum_of_lowest_prime_divisors += lowest_prime_divisor[i];
        }
    }

    std::cout << sum_of_lowest_prime_divisors << "\n";
}

int main() {
    std::size_t n;
    std::cin >> n;
    compute_sum_of_lowest_prime_divisors(n);
    return 0;
}
