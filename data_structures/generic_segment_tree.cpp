#include <bits/stdc++.h>
using namespace std;
using i64 = long long;
 
/**
 * Generic Segment Tree (point update, range query, any associative op).
 *   - 0-based, inclusive ranges
 *
 * Construct:
 *   SegTree st(n, identity, merge);       // all leaves = identity
 *   SegTree st(vec, identity, merge);     // built from vector<T> in O(n)
 *
 * Methods:
 *   T set(i, v)          -> data[i] = v, returns v
 *   T update(i, v)       -> data[i] = merge(data[i], v), returns new data[i]
 *   T queryRange(l, r)   -> aggregate over [l, r]
 *
 * Examples:
 *   SegTree st(n, INF,  [](i64 a, i64 b) { return min(a, b); });
 *   SegTree st(n, -INF, [](i64 a, i64 b) { return max(a, b); });
 *   SegTree st(n, 0LL,  [](i64 a, i64 b) { return a + b; });
 *   SegTree st(n, 0LL,  [](i64 a, i64 b) { return gcd(a, b); });
 *   SegTree st(n, 0LL,  [](i64 a, i64 b) { return (a + b) % MOD; });
 *
 * Notes:
 *   - identity must be the identity element of merge
 *   - T can be a struct ({sum, max}, matrix, ...) as long as merge is associative
 *   - literal types must match T exactly: 0LL not 0 when T = i64 (CTAD deduces T from identity)
 */

template <class T, class F>
struct SegTree {
    int n;
    vector<T> tree;
    F merge;
    T identity;

    SegTree(int size, T id, F op)
        : n(size), tree(4 * size, id), merge(op), identity(id) {}

    SegTree(const vector<T>& leafs, T id, F op)
        : n((int)leafs.size()), tree(4 * leafs.size(), id), merge(op), identity(id) {
        build(0, 0, n - 1, leafs);
    }

    T set(int i, T v) { return update(0, 0, n - 1, i, v, true); }
    T update(int i, T v) { return update(0, 0, n - 1, i, v, false); }
    T queryRange(int l, int r) { return query(0, 0, n - 1, l, r); }

  private:
    void build(int inx, int L, int R, const vector<T>& data) {
        if (L == R) {
            tree[inx] = data[L];
            return;
        }
        int mid = L + (R - L) / 2;
        build(2 * inx + 1, L, mid, data);
        build(2 * inx + 2, mid + 1, R, data);
        tree[inx] = merge(tree[2 * inx + 1], tree[2 * inx + 2]);
    }

    T update(int inx, int L, int R, int target, T v, bool destructive) {
        if (L == R) {
            tree[inx] = destructive ? v : merge(tree[inx], v);
            return tree[inx];
        }
        int mid = L + (R - L) / 2;
        T leafVal = (target <= mid) ? update(2 * inx + 1, L, mid, target, v, destructive)
                                    : update(2 * inx + 2, mid + 1, R, target, v, destructive);
        tree[inx] = merge(tree[2 * inx + 1], tree[2 * inx + 2]);
        return leafVal;
    }

    T query(int inx, int L, int R, int tl, int tr) {
        if (R < tl || tr < L) {
            return identity;
        }
        if (tl <= L && R <= tr) {
            return tree[inx];
        }
        int mid = L + (R - L) / 2;
        return merge(query(2 * inx + 1, L, mid, tl, tr),
                     query(2 * inx + 2, mid + 1, R, tl, tr));
    }
};