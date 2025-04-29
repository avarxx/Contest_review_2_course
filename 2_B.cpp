#include <iostream>
#include <vector>

void compute_sum_of_lowest_prime_divisors(int n) {
    std::vector<int> lowest_prime_divisor(n + 1, 0);

    for (int i = 2; i <= n; ++i) {
        if (lowest_prime_divisor[i] == 0) { 
            for (long long j = (long long)i * i; j <= n; j += i) {
                if (lowest_prime_divisor[j] == 0) {
                    lowest_prime_divisor[j] = i;
                }
            }
            lowest_prime_divisor[i] = i; 
        }
    }

    long long sum_of_lowest_prime_divisors = 0;
    for (int i = 3; i <= n; ++i) {
        if (lowest_prime_divisor[i] != i) { 
            sum_of_lowest_prime_divisors += lowest_prime_divisor[i];
        }
    }

    std::cout << sum_of_lowest_prime_divisors << "\n";
}

int main() {
    int n;
    std::cin >> n;
    compute_sum_of_lowest_prime_divisors(n);
    return 0;
}
