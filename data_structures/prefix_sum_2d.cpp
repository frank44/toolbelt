#include <bits/stdc++.h>
using namespace std;
using i64 = long long;

/*
    2D Prefix Sums
        Build: O(nm)
        Rectangle sum query: O(1)

    Indexing:
        - Public API is 0-indexed; query bounds are INCLUSIVE on both ends.
        - Internally p is (n+1) x (m+1) with a zero row/col in front, so
          p[i][j] = sum over rows [0, i) and cols [0, j)  (half-open).
          Read p directly if you want the half-open form.

    Notes:
        - T defaults to i64; counts on a 0/1 grid fit in int (n*m <= 2e9).
        - query returns 0 for an empty rectangle (r1 > r2 or c1 > c2).
        - Memory is (n+1)(m+1) of T; for T = i64 and n = m = 3000 that is ~72 MB.

    Usage:
        vector<vector<i64>> g = ...;                 // n x m, same type as T
        PrefixSum2D<i64> ps(g);
        auto s = ps.query(r1, c1, r2, c2);           // sum over rows [r1, r2], cols [c1, c2]

        // From a cell function (grid of another type, vector<string>, etc.):
        PrefixSum2D<int> cnt(n, m, [&](int i, int j) { return g[i][j] == '#'; });
        PrefixSum2D<i64> ps2(n, m, [&](int i, int j) { return a[i][j]; });   // a is vector<vector<int>>

    ---------------------------------------------------------------------------

    2D Difference Array (the dual: rectangle add, then read every cell once)
        Each add: O(1)
        Build: O(nm)

    Usage:
        RangeAdd2D<i64> ra(n, m);
        ra.add(r1, c1, r2, c2, v);                   // += v on rows [r1, r2], cols [c1, c2]
        auto grid = ra.build();                      // n x m final values

    Typical: "paint K rectangles, how many cells are covered >= 2 times".
    Stacking: to get rectangle sums of the painted grid, feed build() into PrefixSum2D.
*/


template <typename T = i64>
struct PrefixSum2D {
    int n, m;
    vector<vector<T>> p;  // p[i][j] = sum over [0, i) x [0, j)

    // cell(i, j) -> value at row i, col j
    PrefixSum2D(int n_, int m_, function<T(int, int)> cell) : n(n_), m(m_), p(n_ + 1, vector<T>(m_ + 1, T{})) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                p[i + 1][j + 1] = cell(i, j) + p[i][j + 1] + p[i + 1][j] - p[i][j];
            }
        }
    }

    PrefixSum2D(const vector<vector<T>>& g)
        : PrefixSum2D((int)g.size(), g.empty() ? 0 : (int)g[0].size(),
                      [&](int i, int j) { return g[i][j]; }) {}

    // sum over rows [r1, r2] and cols [c1, c2], inclusive; 0 if empty
    T query(int r1, int c1, int r2, int c2) const {
        if (r1 > r2 || c1 > c2) {
            return T{};
        }
        return p[r2 + 1][c2 + 1] - p[r1][c2 + 1] - p[r2 + 1][c1] + p[r1][c1];
    }
};


template <typename T = i64>
struct RangeAdd2D {
    int n, m;
    vector<vector<T>> d;  // (n+1) x (m+1) difference array

    RangeAdd2D(int n_, int m_) : n(n_), m(m_), d(n_ + 1, vector<T>(m_ + 1, T{})) {}

    // += v on every cell with row in [r1, r2] and col in [c1, c2], inclusive
    void add(int r1, int c1, int r2, int c2, T v) {
        if (r1 > r2 || c1 > c2) {
            return;
        }
        d[r1][c1] += v;
        d[r1][c2 + 1] -= v;
        d[r2 + 1][c1] -= v;
        d[r2 + 1][c2 + 1] += v;
    }

    // n x m grid of final values, O(nm). d is left intact so more adds can follow.
    vector<vector<T>> build() const {
        vector res(n, vector<T>(m, T{}));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                T up = i > 0 ? res[i - 1][j] : T{};
                T left = j > 0 ? res[i][j - 1] : T{};
                T diag = (i > 0 && j > 0) ? res[i - 1][j - 1] : T{};
                res[i][j] = d[i][j] + up + left - diag;
            }
        }
        return res;
    }
};
