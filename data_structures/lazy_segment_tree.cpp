#include <bits/stdc++.h>
using namespace std;
using i64 = long long;

/**
 * Generic Segment Tree that uses lazy propagation to support range updates (on top of the simpler version)
 *   - 0-based, inclusive ranges
 *
 * Construct:
 *   SegTree st(n, identity, merge);                       // all leaves = identity
 *   SegTree st(vec, identity, merge);                     // built from vector<T> in O(n)
 *   SegTree st(n, identity, merge, apply, compose);       // with range updates
 *   SegTree st(vec, identity, merge, apply, compose);     // with range updates
 *
 * Methods:
 *   T set(i, v)             -> data[i] = v, returns v
 *   T update(i, v)          -> data[i] = merge(data[i], v), returns new data[i]
 *   T setRange(l, r, v)     -> data[i] = v for i in [l, r], returns aggregate over [l, r]
 *   T updateRange(l, r, v)  -> data[i] = merge(data[i], v) for i in [l, r], returns aggregate over [l, r]
 *   T queryRange(l, r)      -> aggregate over [l, r]
 *   string toString()       -> leaves + tree drawing, one node per line; debug(st) prints it
 *
 * Range updates require an `apply` and a `compose`:
 *   apply(T node, T v, int len)  -> node value after applying tag v to the whole segment (len = segment length)
 *   compose(T newV, T oldV)      -> pending tag when newV is applied on top of oldV
 *
 * Examples:
 *   // point update, range min
 *   SegTree st(n, INF, [](i64 a, i64 b) { return min(a, b); });
 *
 *   // range add, range sum
 *   SegTree st(n, 0LL,
 *       [](i64 a, i64 b) { return a + b; },
 *       [](i64 node, i64 v, int len) { return node + v * len; },
 *       [](i64 nv, i64 ov) { return nv + ov; });
 *
 *   // range assign, range sum
 *   SegTree st(n, 0LL,
 *       [](i64 a, i64 b) { return a + b; },
 *       [](i64 node, i64 v, int len) { return v * len; },
 *       [](i64 nv, i64 ov) { return nv; });
 *
 * Notes:
 *   - identity must be the identity element of merge
 *   - literal types must match T exactly: 0LL not 0 when T = i64 (CTAD deduces T from identity)
 *   - T can be a struct ({sum, max}, matrix, ...) as long as merge is associative
 *   - toString needs a printable T: under LOCAL anything debug() can print (pair, tuple, vector, ...),
 *     otherwise anything with operator<<. A struct T needs its own operator<< either way:
 *
 *       struct Node {
 *           i64 sum, mx;
 *           friend ostream& operator<<(ostream& os, const Node& x) {
 *               return os << "{sum=" << x.sum << " mx=" << x.mx << "}";   // keep it on one line
 *           }
 *       };
 *
 * toString() output for SegTree st(vector<i64>{1, 2, 3, 4, 5}, 0LL, sum):
 *   SegTree n=5 leaves={1, 2, 3, 4, 5}
 *   [0, 4] = 15
 *   ├─ [0, 2] = 6
 *   │  ├─ [0, 1] = 3
 *   │  │  ├─ [0] = 1
 *   │  │  └─ [1] = 2
 *   │  └─ [2] = 3
 *   └─ [3, 4] = 9
 *      ├─ [3] = 4
 *      └─ [4] = 5
 * (the leaves={...} summary is only printed when T is a plain number)
 */

template <class T, class F, class A = std::nullptr_t, class C = std::nullptr_t>
struct SegTree {
    int n;
    vector<T> tree;
    vector<T> lazy;
    vector<bool> hasLazy;
    F merge;
    T identity;
    A apply;
    C compose;

    SegTree(int size, T id, F op)
        : n(size), tree(4 * size, id), lazy(4 * size, id), hasLazy(4 * size, false),
          merge(op), identity(id), apply(nullptr), compose(nullptr) {}

    SegTree(const vector<T>& leafs, T id, F op)
        : n((int)leafs.size()), tree(4 * leafs.size(), id), lazy(4 * leafs.size(), id),
          hasLazy(4 * leafs.size(), false), merge(op), identity(id),
          apply(nullptr), compose(nullptr) {
        build(0, 0, n - 1, leafs);
    }

    SegTree(int size, T id, F op, A ap, C cp)
        : n(size), tree(4 * size, id), lazy(4 * size, id), hasLazy(4 * size, false),
          merge(op), identity(id), apply(ap), compose(cp) {}

    SegTree(const vector<T>& leafs, T id, F op, A ap, C cp)
        : n((int)leafs.size()), tree(4 * leafs.size(), id), lazy(4 * leafs.size(), id),
          hasLazy(4 * leafs.size(), false), merge(op), identity(id),
          apply(ap), compose(cp) {
        build(0, 0, n - 1, leafs);
    }

    T set(int i, T v) { return update(0, 0, n - 1, i, v, true); }
    T update(int i, T v) { return update(0, 0, n - 1, i, v, false); }

    T setRange(int l, int r, T v) { return rangeUpdate(0, 0, n - 1, l, r, v, true); }
    T updateRange(int l, int r, T v) { return rangeUpdate(0, 0, n - 1, l, r, v, false); }

    T queryRange(int l, int r) { return query(0, 0, n - 1, l, r); }

    // Leaves on the header line (numeric T only), then the tree, one node per line. No trailing newline.
    string toString() const {
        string out = "SegTree n=" + to_string(n);
        if (n == 0) {
            return out;
        }
        string leaves, body;
        dump(body, leaves, 0, 0, n - 1, "", "");
        if (is_arithmetic_v<T>) {
            out += " leaves={" + leaves + "}";
        }
        return out + body;
    }

    friend ostream& operator<<(ostream& os, const SegTree& st) {
        return os << st.toString();
    }

  private:
    static string show(const T& v) {
        ostringstream o;
#ifdef LOCAL
        auto* old = cout.rdbuf(o.rdbuf());  // dbg() only writes to cout, so borrow its buffer
        dbg(v);
        cout.rdbuf(old);
#else
        o << v;
#endif
        return o.str();
    }

    // head = prefix for this node's own line, tail = prefix its children build on
    void dump(string& body, string& leaves, int inx, int L, int R, const string& head, const string& tail) const {
        string val = show(tree[inx]);
        body += "\n" + head + "[" + to_string(L);
        if (L < R) {
            body += ", " + to_string(R);
        }
        body += "] = " + val;
        if (hasLazy[inx]) {
            body += " (lazy=" + show(lazy[inx]) + ")";
        }
        if (L == R) {
            leaves += (leaves.empty() ? "" : ", ") + val;
            return;
        }
        int mid = L + (R - L) / 2;
        dump(body, leaves, 2 * inx + 1, L, mid, tail + "\u251c\u2500 ", tail + "\u2502  ");
        dump(body, leaves, 2 * inx + 2, mid + 1, R, tail + "\u2514\u2500 ", tail + "   ");
    }

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

    // Push node inx's pending tag (if any) down to its children.
    void push(int inx, int L, int R) {
        if (!hasLazy[inx] || L == R) {
            return;
        }
        int mid = L + (R - L) / 2;
        int lc = 2 * inx + 1, rc = 2 * inx + 2;
        tree[lc] = apply(tree[lc], lazy[inx], mid - L + 1);
        if (hasLazy[lc]) lazy[lc] = compose(lazy[inx], lazy[lc]);
        else { lazy[lc] = lazy[inx]; hasLazy[lc] = true; }
        tree[rc] = apply(tree[rc], lazy[inx], R - mid);
        if (hasLazy[rc]) lazy[rc] = compose(lazy[inx], lazy[rc]);
        else { lazy[rc] = lazy[inx]; hasLazy[rc] = true; }
        hasLazy[inx] = false;
    }

    T update(int inx, int L, int R, int target, T v, bool destructive) {
        if (L == R) {
            tree[inx] = destructive ? v : merge(tree[inx], v);
            return tree[inx];
        }
        push(inx, L, R);
        int mid = L + (R - L) / 2;
        T leafVal = (target <= mid) ? update(2 * inx + 1, L, mid, target, v, destructive)
                                    : update(2 * inx + 2, mid + 1, R, target, v, destructive);
        tree[inx] = merge(tree[2 * inx + 1], tree[2 * inx + 2]);
        return leafVal;
    }

    T rangeUpdate(int inx, int L, int R, int tl, int tr, T v, bool destructive) {
        if (R < tl || tr < L) {
            return identity;
        }
        if (tl <= L && R <= tr) {
            tree[inx] = apply(tree[inx], v, R - L + 1);
            if (hasLazy[inx]) lazy[inx] = compose(v, lazy[inx]);
            else { lazy[inx] = v; hasLazy[inx] = true; }
            return tree[inx];
        }
        push(inx, L, R);
        int mid = L + (R - L) / 2;
        T res;
        if (tr <= mid) {
            res = rangeUpdate(2 * inx + 1, L, mid, tl, tr, v, destructive);
        } else if (tl > mid) {
            res = rangeUpdate(2 * inx + 2, mid + 1, R, tl, tr, v, destructive);
        } else {
            T a = rangeUpdate(2 * inx + 1, L, mid, tl, tr, v, destructive);
            T b = rangeUpdate(2 * inx + 2, mid + 1, R, tl, tr, v, destructive);
            res = merge(a, b);
        }
        tree[inx] = merge(tree[2 * inx + 1], tree[2 * inx + 2]);
        return res;
    }

    T query(int inx, int L, int R, int tl, int tr) {
        if (R < tl || tr < L) {
            return identity;
        }
        if (tl <= L && R <= tr) {
            return tree[inx];
        }
        push(inx, L, R);
        int mid = L + (R - L) / 2;
        return merge(query(2 * inx + 1, L, mid, tl, tr),
                     query(2 * inx + 2, mid + 1, R, tl, tr));
    }
};