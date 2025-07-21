#include <algorithm>
#include <iostream>
#include <vector>

constexpr int MOD = 7340033;
constexpr int ROOT = 5;

class NTT {
public:
    static int modPow(int base, int exp) {
        long long result = 1, curr = base;
        while (exp) {
            if (exp & 1) result = result * curr % MOD;
            curr = curr * curr % MOD;
            exp >>= 1;
        }
        return static_cast<int>(result);
    }

    static void transform(std::vector<int> &data, bool invert) {
        int n = data.size();
        int logN = __builtin_ctz(n);
        static std::vector<int> rev;
        static std::vector<int> roots;
        rev.assign(n, 0);
        roots.assign(n, 0);

        for (int i = 0; i < n; ++i)
            rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (logN - 1));

        roots[1] = 1;
        for (int k = 1; (1 << k) < n; ++k) {
            int root = modPow(ROOT, (MOD - 1) >> (k + 1));
            for (int i = 1 << (k - 1); i < (1 << k); ++i) {
                roots[2 * i] = roots[i];
                roots[2 * i + 1] = static_cast<int>(1LL * roots[i] * root % MOD);
            }
        }

        for (int i = 0; i < n; ++i)
            if (i < rev[i]) std::swap(data[i], data[rev[i]]);

        for (int len = 1; len < n; len <<= 1) {
            for (int i = 0; i < n; i += 2 * len) {
                for (int j = 0; j < len; ++j) {
                    int u = data[i + j];
                    int v = static_cast<int>(1LL * data[i + j + len] * roots[len + j] % MOD);
                    data[i + j] = u + v < MOD ? u + v : u + v - MOD;
                    data[i + j + len] = u - v >= 0 ? u - v : u - v + MOD;
                }
            }
        }

        if (invert) {
            std::reverse(data.begin() + 1, data.end());
            int invN = modPow(n, MOD - 2);
            for (int &x : data)
                x = static_cast<int>(1LL * x * invN % MOD);
        }
    }

    static std::vector<int> multiply(const std::vector<int> &A,
                                     const std::vector<int> &B,
                                     int keep) {
        int need = A.size() + B.size() - 1;
        int n = 1;
        while (n < keep) n <<= 1;
        while (n < need) n <<= 1;

        std::vector<int> FA = A;
        std::vector<int> FB = B;
        FA.resize(n);
        FB.resize(n);

        transform(FA, false);
        transform(FB, false);
        for (int i = 0; i < n; ++i)
            FA[i] = static_cast<int>(1LL * FA[i] * FB[i] % MOD);
        transform(FA, true);

        FA.resize(keep);
        return FA;
    }
};

class Polynomial {
    std::vector<int> coeffs;
public:
    Polynomial() = default;
    explicit Polynomial(std::vector<int> c) : coeffs(std::move(c)) {}

    int operator[](size_t idx) const { return coeffs[idx]; }

    friend std::istream& operator>>(std::istream &is, Polynomial &P) {
        int degree;
        is >> degree;
        P.coeffs.assign(degree + 1, 0);
        for (int i = 0; i <= degree; ++i)
            is >> P.coeffs[i];
        return is;
    }

    friend std::ostream& operator<<(std::ostream &os, const Polynomial &P) {
        for (size_t i = 0; i < P.coeffs.size(); ++i) {
            if (i) os << ' ';
            os << P.coeffs[i];
        }
        return os << '\n';
    }

    Polynomial invertSeries(int m) const {
        std::vector<int> Q{NTT::modPow(coeffs[0], MOD - 2)};
        for (int len = 1; len < m; len <<= 1) {
            int lim = len << 1;
            std::vector<int> cut(std::min((int)coeffs.size(), lim));
            std::copy(coeffs.begin(), coeffs.begin() + cut.size(), cut.begin());
            auto PQ = NTT::multiply(cut, Q, lim);

            std::vector<int> twoMinusPQ(lim);
            twoMinusPQ[0] = (2 - PQ[0] + MOD) % MOD;
            for (int i = 1; i < lim; ++i)
                twoMinusPQ[i] = (MOD - PQ[i]) % MOD;

            Q = NTT::multiply(Q, twoMinusPQ, lim);
        }
        Q.resize(m);
        return Polynomial(Q);
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int m;
    Polynomial P;
    std::cin >> m >> P;
    if (P[0] == 0) {
        std::cout << "The ears of a dead donkey\n";
        return 0;
    }

    Polynomial Q = P.invertSeries(m);
    std::cout << Q;
    return 0;
}
