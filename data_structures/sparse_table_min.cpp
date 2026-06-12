/*
    Standard Sparse Table implementation
        O(nlogn) to build
        O(1) minimum-range queries over [L, R)

    Usage: 
        SparseTableMin<i64> t(vec);
        t.queryRange(0, n); // note the half open query interval
*/

template <typename T>
struct SparseTableMin {
    int n, k;
    vector<vector<T>> t;  // t[j][i] = min over [i, i + 2^j)

    SparseTableMin(const vector<T>& arr) {
        n = (int)arr.size();
        k = __lg(n) + 1;  // n must be > 0
        t.assign(k, vector<T>(n));

        t[0] = arr;
        for (int j = 1; j < k; j++) {
            for (int i = 0; i + (1 << j) <= n; i++) {
                t[j][i] = min(t[j-1][i], t[j-1][i + (1 << (j-1))]);
            }
        }
    }

    // min over [L, R)
    T queryRange(int L, int R) const {
        assert(0 <= L && L < R && R <= n);
        int lg = __lg(R - L);
        return min(t[lg][L], t[lg][R - (1 << lg)]);
    }
};