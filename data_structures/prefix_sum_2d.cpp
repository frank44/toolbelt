#include <bits/stdc++.h>
using namespace std;
using i64 = long long;

/*
    2D Prefix Sums
        Build: O(nm)
        Rectangle sum query: O(1)

    Indexing:
        - 0-indexed; everything is INCLUSIVE on both ends.
        - p is n x m, same shape as the grid: p[i][j] = sum over rows [0, i], cols [0, j].
        - prefix(i, j) is p[i][j] but returns 0 when i < 0 or j < 0, so query can
          subtract the row/col just before the rectangle without special-casing 0.

    Notes:
        - query returns 0 for an empty rectangle (r1 > r2 or c1 > c2).
        - Memory is n*m i64s; n = m = 3000 is ~72 MB.

    Usage:
        vector<vector<i64>> g = ...;                 // n x m
        PrefixSum2D ps(g);
        auto s = ps.query(r1, c1, r2, c2);           // sum over rows [r1, r2], cols [c1, c2]
        auto t = ps.prefix(i, j);                    // sum over rows [0, i], cols [0, j]

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

    Indexing:
        - 0-indexed, INCLUSIVE, d is n x m, same shape as the final grid.
        - add() drops the -v markers that would land past the bottom/right edge,
          so the final grid is exactly the 2D prefix sum of d and build() is
          just PrefixSum2D(d).p.

    Usage:
        RangeAdd2D ra(n, m);
        ra.add(r1, c1, r2, c2, v);                   // += v on rows [r1, r2], cols [c1, c2]
        auto grid = ra.build();                      // n x m final values

    Typical: "paint K rectangles, how many cells are covered >= 2 times".
    Stacking: to get rectangle sums of the painted grid, feed build() into PrefixSum2D.
*/


struct PrefixSum2D {
    int n, m;
    vector<vector<i64>> p;  // p[i][j] = sum over rows [0, i], cols [0, j]

    PrefixSum2D(const vector<vector<i64>>& g) : p(g) {
        n = g.size();
        m = n > 0 ? g[0].size() : 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                p[i][j] += prefix(i - 1, j) + prefix(i, j - 1) - prefix(i - 1, j - 1);
            }
        }
    }

    // sum over rows [0, i] and cols [0, j], inclusive; 0 if i < 0 or j < 0
    i64 prefix(int i, int j) const {
        if (i < 0 || j < 0) {
            return 0;
        }
        return p[i][j];
    }

    // sum over rows [r1, r2] and cols [c1, c2], inclusive; 0 if empty
    i64 query(int r1, int c1, int r2, int c2) const {
        if (r1 > r2 || c1 > c2) {
            return 0;
        }
        return prefix(r2, c2) - prefix(r1 - 1, c2) - prefix(r2, c1 - 1) + prefix(r1 - 1, c1 - 1);
    }
};


struct RangeAdd2D {
    int n, m;
    vector<vector<i64>> d;  // n x m difference array

    RangeAdd2D(int n_, int m_) : n(n_), m(m_), d(n_, vector<i64>(m_)) {}

    // d[i][j] += v; does nothing if (i, j) is past the bottom/right edge
    void mark(int i, int j, i64 v) {
        if (i < n && j < m) {
            d[i][j] += v;
        }
    }

    // += v on every cell with row in [r1, r2] and col in [c1, c2], inclusive
    void add(int r1, int c1, int r2, int c2, i64 v) {
        if (r1 > r2 || c1 > c2) {
            return;
        }
        mark(r1, c1, v);
        mark(r1, c2 + 1, -v);
        mark(r2 + 1, c1, -v);
        mark(r2 + 1, c2 + 1, v);
    }

    // n x m grid of final values, O(nm). d is left intact so more adds can follow.
    vector<vector<i64>> build() const {
        return PrefixSum2D(d).p;
    }
};
