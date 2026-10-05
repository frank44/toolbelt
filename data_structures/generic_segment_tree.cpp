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
 *   string toString()    -> leaves + tree drawing, one node per line; debug(st) prints it
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