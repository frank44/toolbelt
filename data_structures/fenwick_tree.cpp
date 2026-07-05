#include <bits/stdc++.h>
using namespace std;
using i64 = long long;

/*
    Generic Fenwick Tree (Binary Indexed Tree) for point-update / prefix-sum

    Indexing:
        - The public API is fully 0-indexed: valid positions are [0, n).
        - Internally a 1-indexed tree of size n+1 is used (the +1 is hidden).

    Notes:
        - lowerBound assumes all stored values are NON-NEGATIVE (prefix sums must
          be monotonic non-decreasing). It is the Fenwick-descent generalization
          of "k-th smallest" on a frequency table.
        - pointGet(i) recovers a single value in O(log n), ~2x faster than
          prefixSum(i) - prefixSum(i - 1).
        - scale multiplies every value by c; watch for overflow.

    Usage:
        FenwickTree<i64> ft(n);   // n zeros
        ft.update(i, delta);             // arr[i] += delta
        ft.set(i, value);                // arr[i] = value
        auto s  = ft.rangeSum(a, b);     // sum over [a, b] inclusive
        auto p  = ft.prefixSum(i);       // sum over [0, i] inclusive
        auto sf = ft.suffixSum(i);       // sum over [i, n-1] inclusive
        auto v  = ft.pointGet(i);        // value at index i
        int  k  = ft.lowerBound(target); // smallest i with prefixSum(i) >= target, else n

        vector<long long> a = {...};
        FenwickTree<long long> ft2(a);  // build directly from values in O(n)
*/


template <typename T = i64>
struct FenwickTree {
    int n;             // number of logical positions (0-indexed)
    vector<T> tree;
    T totalSum;        // running sum of all values, for O(1) total() / suffixSum

    FenwickTree(int n_) : n(n_), tree(n_ + 1, T{}), totalSum(T{}) {}

    FenwickTree(const vector<T>& a) : n((int)a.size()), tree(a.size() + 1, T{}), totalSum(T{}) {
        for (int i = 0; i < n; i++) {
            tree[i + 1] += a[i];
            totalSum += a[i];
            int parent = (i + 1) + ((i + 1) & -(i + 1));
            if (parent <= n) {
                tree[parent] += tree[i + 1];
            }
        }
    }

    // arr[i] += delta
    void update(int i, T delta) {
        totalSum += delta;
        for (int x = i + 1; x <= n; x += x & -x) {
            tree[x] += delta;
        }
    }

    // arr[i] = value
    void set(int i, T value) {
        update(i, value - pointGet(i));
    }

    // sum over [0, i] inclusive; returns 0 for i < 0
    T prefixSum(int i) const {
        T sum = T{};
        for (int x = i + 1; x > 0; x -= x & -x) {
            sum += tree[x];
        }
        return sum;
    }

    // sum over [a, b] inclusive
    T rangeSum(int a, int b) const {
        if (a > b) {
            return T{};
        }
        return prefixSum(b) - prefixSum(a - 1);
    }

    // sum over [i, n-1] inclusive
    T suffixSum(int i) const {
        return totalSum - prefixSum(i - 1);
    }

    // value at index i, in O(log n)
    T pointGet(int i) const {
        int x = i + 1;
        T sum = tree[x];
        int stop = x - (x & -x);
        for (x--; x != stop; x -= x & -x) {
            sum -= tree[x];
        }
        return sum;
    }

    // Smallest index i in [0, n) with prefixSum(i) >= target, or n if none exists.
    // Requires all stored values to be non-negative.
    int lowerBound(T target) const {
        if (target <= T{}) {
            return 0;
        }
        int pos = 0;       // largest 1-indexed node whose prefix is still < target
        T cur = T{};
        for (int pw = 1 << __lg(max(1, n)); pw > 0; pw >>= 1) {
            int next = pos + pw;
            if (next <= n && cur + tree[next] < target) {
                pos = next;
                cur += tree[next];
            }
        }
        return pos;        // 0-indexed answer = (pos + 1) - 1
    }

    // Multiply every stored value by c. Beware of overflow.
    void scale(T c) {
        totalSum *= c;
        for (int i = 1; i <= n; i++) {
            tree[i] *= c;
        }
    }

    // Reset all values to zero.
    void clear() {
        totalSum = T{};
        fill(tree.begin(), tree.end(), T{});
    }
};
