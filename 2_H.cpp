#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <algorithm>

constexpr double PI_VAL = 3.14159265358979323846;

class FFT {
public:
    using cd = std::complex<double>;
    
    static void transform(std::vector<cd> &data, bool invert) {
        bitReversePermute(data);
        const std::size_t n = data.size();
        for (std::size_t len = 2; len <= n; len <<= 1) {
            double angle = 2 * PI_VAL / len * (invert ? -1 : 1);
            cd wlen(std::cos(angle), std::sin(angle));
            for (std::size_t i = 0; i < n; i += len) {
                cd w(1);
                for (std::size_t j = 0; j < len / 2; ++j) {
                    cd u = data[i + j];
                    cd t = data[i + j + len/2] * w;
                    data[i + j] = u + t;
                    data[i + j + len/2] = u - t;
                    w *= wlen;
                }
            }
        }
        if (invert) {
            for (auto &x : data)
                x /= static_cast<double>(n);
        }
    }

    static std::vector<long long> multiply(const std::vector<long long> &A,
                                           const std::vector<long long> &B) {
        std::size_t nA = A.size();
        std::size_t nB = B.size();
        std::size_t n = 1;
        while (n < nA + nB)
            n <<= 1;

        std::vector<cd> fa(n), fb(n);
        for (std::size_t i = 0; i < nA; ++i) fa[i] = cd(A[i], 0);
        for (std::size_t i = 0; i < nB; ++i) fb[i] = cd(B[i], 0);

        transform(fa, false);
        transform(fb, false);
        for (std::size_t i = 0; i < n; ++i)
            fa[i] *= fb[i];
        transform(fa, true);

        std::vector<long long> result(nA + nB - 1);
        for (std::size_t i = 0; i < result.size(); ++i)
            result[i] = std::llround(fa[i].real());
        return result;
    }

private:
    static void bitReversePermute(std::vector<cd> &data) {
        std::size_t n = data.size();
        std::size_t logN = static_cast<std::size_t>(std::log2(n));
        for (std::size_t i = 0; i < n; ++i) {
            std::size_t j = 0;
            for (std::size_t b = 0; b < logN; ++b) {
                if (i & (1u << b))
                    j |= 1u << (logN - 1 - b);
            }
            if (i < j)
                std::swap(data[i], data[j]);
        }
    }
};

static std::vector<long long> operator*(const std::vector<long long> &lhs,
                                        const std::vector<long long> &rhs) {
    return FFT::multiply(lhs, rhs);
}

void readPolynomial(int &degree, std::vector<long long> &poly) {
    std::cin >> degree;
    poly.resize(degree + 1);
    for (int i = degree; i >= 0; --i)
        std::cin >> poly[i];
}

void printPolynomial(const std::vector<long long> &poly) {
    int degree = static_cast<int>(poly.size()) - 1;
    std::cout << degree;
    for (int i = degree; i >= 0; --i)
        std::cout << ' ' << poly[i];
    std::cout << '\n';
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int degA, degB;
    std::vector<long long> A, B;
    readPolynomial(degA, A);
    readPolynomial(degB, B);

    auto C = A * B;
    printPolynomial(C);

    return 0;
}
