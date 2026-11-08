#include <bits/stdc++.h>
using namespace std;
using i64 = long long;

/*
    Merge-Sort Tree over a STATIC array (no updates): count elements of an index range by value.

    Build: O(n log n) time and memory (each level of the tree is a sorted copy of the array)
    Query: O(log^2 n) per count, O(log^3 n) per kth
    0-based, inclusive on both index ranges and value ranges.

    Usage:
        vector<i64> a = {...};
        MergeSortTree mst(a);                     // T deduced from the vector
        int c = mst.countLE(l, r, x);             // # of i in [l, r] with a[i] <= x
        int c = mst.countLT(l, r, x);             // # of i in [l, r] with a[i] <  x
        int c = mst.countBetween(l, r, x, y);     // # of i in [l, r] with x <= a[i] <= y
        i64 v = mst.kth(l, r, k);                 // k-th smallest in [l, r], k is 1-based
*/

template <typename T = i64>
struct MergeSortTree {
    int n;
    vector<vector<T>> tree;   // tree[node] = sorted copy of its segment; tree[0] = whole array sorted

    MergeSortTree(const vector<T>& a) : n((int)a.size()), tree(4 * a.size()) {
        if (n > 0) {
            build(0, 0, n - 1, a);
        }
    }

    // # of i in [l, r] with a[i] < x
    int countLT(int l, int r, T x) const { return count(0, 0, n - 1, l, r, x, true); }

    // # of i in [l, r] with a[i] <= x
    int countLE(int l, int r, T x) const { return count(0, 0, n - 1, l, r, x, false); }

    // # of i in [l, r] with x <= a[i] <= y
    int countBetween(int l, int r, T x, T y) const { return countLE(l, r, y) - countLT(l, r, x); }

    // k-th smallest value in [l, r], 1-based (k = 1 is the min). Requires 1 <= k <= r - l + 1.
    T kth(int l, int r, int k) const {
        const auto& all = tree[0];
        int lo = 0, hi = n - 1;   // smallest index into all[] whose value has >= k elements <= it
        while (lo < hi) {
            int mid = lo + (hi - lo) / 2;
            if (countLE(l, r, all[mid]) >= k) {
                hi = mid;
            } else {
                lo = mid + 1;
            }
        }
        return all[lo];
    }

  private:
    void build(int inx, int L, int R, const vector<T>& a) {
        if (L == R) {
            tree[inx] = {a[L]};
            return;
        }
        int mid = L + (R - L) / 2;
        build(2 * inx + 1, L, mid, a);
        build(2 * inx + 2, mid + 1, R, a);
        tree[inx].reserve(R - L + 1);
        const auto& lt = tree[2 * inx + 1];
        const auto& rt = tree[2 * inx + 2];
        merge(lt.begin(), lt.end(), rt.begin(), rt.end(), back_inserter(tree[inx]));
    }

    // strict: count a[i] < x, else a[i] <= x
    int count(int inx, int L, int R, int l, int r, T x, bool strict) const {
        if (R < l || r < L) {
            return 0;
        }
        if (l <= L && R <= r) {
            const auto& v = tree[inx];
            auto it = strict ? lower_bound(v.begin(), v.end(), x) : upper_bound(v.begin(), v.end(), x);
            return (int)(it - v.begin());
        }
        int mid = L + (R - L) / 2;
        return count(2 * inx + 1, L, mid, l, r, x, strict) +
               count(2 * inx + 2, mid + 1, R, l, r, x, strict);
    }
};
