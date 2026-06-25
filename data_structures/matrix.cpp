#include <bits/stdc++.h>
#include <cassert>
using namespace std;

/*
    Vanilla Matrix Struct
        > Tested on: https://leetcode.com/problems/number-of-zigzag-arrays-ii

    Supported Operations:
    A * B,  A *= B      matrix product, O(n^3)
    A * s,  A *= s      scalar multiply, O(n^2)
    A + B               element-wise sum, O(n^2)
    A ^ e               A^e via binary exponentiation, O(n^3 * log2(e))
*/
using i64 = long long;

struct Matrix {
    int n, m;
    i64 MOD; // pass in zero to disable this feature
    vector<vector<i64>> A;

    Matrix(int n, int m, i64 MOD) : n(n), m(m), MOD(MOD) {
        A = vector(n, vector<i64>(m));
    }

    Matrix(int n, int m, const vector<vector<i64>>& A, i64 MOD) : n(n), m(m), MOD(MOD), A(A) {}

    Matrix operator*(const Matrix& rhs) const {
        assert(m == rhs.n);
        Matrix res(n, rhs.m, MOD);
        for (int i = 0; i < n; i++) { // ijk -> ikj for cache locality
            const auto& Ai = A[i];
            auto& Ri = res.A[i];
            for (int k = 0; k < m; k++) {
                i64 aik = Ai[k];
                if (aik == 0) {
                    continue;
                }
                const auto& Bk = rhs.A[k];
                for (int j = 0; j < rhs.m; j++) {
                    Ri[j] += aik * Bk[j];
                    if (MOD != 0) {
                        Ri[j] %= MOD;
                    }
                }
            }
        }
        return res;
    }

    Matrix& operator*=(const Matrix& rhs) {
        return *this = *this * rhs;
    }

    Matrix operator*(i64 scalar) const {
        Matrix res(n, m, MOD);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                res.A[i][j] = A[i][j] * scalar;
                if (MOD != 0) {
                    res.A[i][j] %= MOD;
                }
            }
        }
        return res;
    }

    Matrix& operator*=(i64 scalar) {
        return *this = *this * scalar;
    }

    Matrix operator+(const Matrix& rhs) const {
        assert(n == rhs.n && m == rhs.m);
        Matrix res(n, m, MOD);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                res.A[i][j] = A[i][j] + rhs.A[i][j];
                if (MOD != 0) {
                    res.A[i][j] %= MOD;
                }
            }
        }
        return res;
    }

    Matrix operator^(i64 pow) const {
        assert(n == m);
        Matrix ans(n, m, MOD);
        for (int i = 0; i < n; i++) {
            ans.A[i][i] = 1;
        }
        Matrix a = *this;
        while (pow > 0) {
            if (pow & 1) {
                ans *= a;
            }
            pow >>= 1;
            if (pow) { // skip the final useless squaring
                a *= a;
            }
        }
        return ans;
    }
};