#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <algorithm>

constexpr double PI_VAL = 3.14159265358979323846;

static void bitReversePermute(std::vector<std::complex<double>> &arr) {
    int n = arr.size();
    int logN = std::log2(n);
    for (int i = 0; i < n; ++i) {
        int j = 0;
        for (int b = 0; b < logN; ++b)
            if (i & (1 << b))
                j |= 1 << (logN - 1 - b);
        if (i < j) std::swap(arr[i], arr[j]);
    }
}

static void fftTransform(std::vector<std::complex<double>> &v, bool invert) {
    bitReversePermute(v);
    int n = v.size();
    for (int len = 2; len <= n; len <<= 1) {
        double ang = 2 * PI_VAL / len * (invert ? -1 : 1);
        std::complex<double> wlen(std::cos(ang), std::sin(ang));
        for (int i = 0; i < n; i += len) {
            std::complex<double> w(1, 0);
            for (int j = 0; j < len / 2; ++j) {
                auto u = v[i + j];
                auto t = v[i + j + len/2] * w;
                v[i + j] = u + t;
                v[i + j + len/2] = u - t;
                w *= wlen;
            }
        }
    }
    if (invert) {
        for (auto &x : v) x /= n;
    }
}

static std::vector<long long> multiplyFTT(const std::vector<long long> &A,
                                          const std::vector<long long> &B) {
    int nA = A.size(), nB = B.size(), n = 1;
    while (n < nA + nB) n <<= 1;
    std::vector<std::complex<double>> fa(n), fb(n);
    for (int i = 0; i < nA; ++i) fa[i] = {double(A[i]), 0.0};
    for (int i = 0; i < nB; ++i) fb[i] = {double(B[i]), 0.0};
    fftTransform(fa, false);
    fftTransform(fb, false);
    for (int i = 0; i < n; ++i) fa[i] *= fb[i];
    fftTransform(fa, true);
    std::vector<long long> C(nA + nB - 1);
    for (int i = 0; i < (int)C.size(); ++i)
        C[i] = std::llround(fa[i].real());
    return C;
}

void readPolynomial(int &deg, std::vector<long long> &poly) {
    std::cin >> deg;
    poly.resize(deg + 1);
    for (int i = deg; i >= 0; --i) {
        std::cin >> poly[i];
    }
}

void printPolynomial(const std::vector<long long> &poly) {
    int deg = int(poly.size()) - 1;
    std::cout << deg;
    for (int i = deg; i >= 0; --i) {
        std::cout << ' ' << poly[i];
    }
    std::cout << '\n';
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int degA, degB;
    std::vector<long long> A, B;
    readPolynomial(degA, A);
    readPolynomial(degB, B);

    auto C = multiplyFTT(A, B);
    printPolynomial(C);

    return 0;
}
