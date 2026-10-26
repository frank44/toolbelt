#include <bits/stdc++.h>
using namespace std;
using i64 = long long;

/*
    2D Prefix Sums
        Build: O(nm)
        Rectangle sum query: O(1)

    Indexing:
        - 0-indexed; query bounds are INCLUSIVE on both ends.
        - p is (n+1) x (m+1) with a zero row/col in front, so
          p[i][j] = sum over rows [0, i) and cols [0, j)  (half-open).
          Read p directly if you want the half-open form.

    Notes:
        - query returns 0 for an empty rectangle (r1 > r2 or c1 > c2).
        - Memory is (n+1)(m+1) i64s; n = m = 3000 is ~72 MB.

    Usage:
        vector<vector<i64>> g = ...;                 // n x m
        PrefixSum2D ps(g);
        auto s = ps.query(r1, c1, r2, c2);           // sum over rows [r1, r2], cols [c1, c2]

        // Counting '#' cells in a vector<string> grid: build the i64 grid first
        vector g(n, vector<i64>(m));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                g[i][j] = s[i][j] == '#';
            }
        }

    ---------------------------------------------------------------------------

    2D Difference Array (the dual: rectangle add, then read every cell once)
        Each add: O(1)
        Build: O(nm)

    Usage:
        RangeAdd2D ra(n, m);
        ra.add(r1, c1, r2, c2, v);                   // += v on rows [r1, r2], cols [c1, c2]
        auto grid = ra.build();                      // n x m final values

    Typical: "paint K rectangles, how many cells are covered >= 2 times".
    Stacking: to get rectangle sums of the painted grid, feed build() into PrefixSum2D.
*/


struct PrefixSum2D {
    int n, m;
    vector<vector<i64>> p;  // p[i][j] = sum over [0, i) x [0, j)

    PrefixSum2D(const vector<vector<i64>>& g) {
        n = g.size();
        m = n > 0 ? g[0].size() : 0;
        p = vector(n + 1, vector<i64>(m + 1));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                p[i + 1][j + 1] = g[i][j] + p[i][j + 1] + p[i + 1][j] - p[i][j];
            }
        }
    }

    // sum over rows [r1, r2] and cols [c1, c2], inclusive; 0 if empty
    i64 query(int r1, int c1, int r2, int c2) const {
        if (r1 > r2 || c1 > c2) {
            return 0;
        }
        return p[r2 + 1][c2 + 1] - p[r1][c2 + 1] - p[r2 + 1][c1] + p[r1][c1];
    }
};


struct RangeAdd2D {
    int n, m;
    vector<vector<i64>> d;  // (n+1) x (m+1) difference array

    RangeAdd2D(int n_, int m_) : n(n_), m(m_), d(n_ + 1, vector<i64>(m_ + 1)) {}

    // += v on every cell with row in [r1, r2] and col in [c1, c2], inclusive
    void add(int r1, int c1, int r2, int c2, i64 v) {
        if (r1 > r2 || c1 > c2) {
            return;
        }
        d[r1][c1] += v;
        d[r1][c2 + 1] -= v;
        d[r2 + 1][c1] -= v;
        d[r2 + 1][c2 + 1] += v;
    }

    // n x m grid of final values, O(nm). d is left intact so more adds can follow.
    vector<vector<i64>> build() const {
        vector res(n, vector<i64>(m));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                i64 up = i > 0 ? res[i - 1][j] : 0;
                i64 left = j > 0 ? res[i][j - 1] : 0;
                i64 diag = (i > 0 && j > 0) ? res[i - 1][j - 1] : 0;
                res[i][j] = d[i][j] + up + left - diag;
            }
        }
        return res;
    }
};
