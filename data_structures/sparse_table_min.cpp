/*
    Standard Sparse Table implementation
        Build: O(nlogn)
        Minimum Range Query: O(1) - returns the min value as well as the corresponding index
            > Ties are broken by LEFTMOST index

    Usage: 
        SparseTableMin t(vec);
        auto [val, inx] = t.query(0, n); // note - half open query interval

    Tested on: https://leetcode.com/problems/number-of-ways-to-assign-edge-weights-ii/description
*/

template <typename T>
struct SparseTableMin {
    int n, k;
    vector<vector<pair<T, int>>> t;  // t[j][i] = {min value, its index} over [i, i + 2^j)

    SparseTableMin(const vector<T>& arr) {
        n = (int)arr.size();
        k = __lg(n) + 1;
        t.assign(k, vector<pair<T, int>>(n));

        for (int i = 0; i < n; i++) {
            t[0][i] = {arr[i], i};
        }
        for (int j = 1; j < k; j++) {
            for (int i = 0; i + (1 << j) <= n; i++) {
                t[j][i] = min(t[j-1][i], t[j-1][i + (1 << (j-1))]);
            }
        }
    }

    // {min value, index of leftmost min} over [L, R)
    pair<T, int> query(int L, int R) const {
        assert(0 <= L && L < R && R <= n);
        int lg = __lg(R - L);
        return min(t[lg][L], t[lg][R - (1 << lg)]);
    }
};