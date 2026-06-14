#include <bits/stdc++.h>
using namespace std;

/*
    Union-Find (Disjoint Set Union) with path compression + union by size
        Construct: O(n)
        find / unite / same: O(alpha(n)) amortized (effectively O(1))
        getComponents: O(n)

    Notes:
        - parent[i] == i only for official representatives; always use find(i).
        - size[r] and numSets are accurate only for representatives r / globally.

    Usage:
        UnionFind uf(n);
        uf.unite(a, b);            // returns false if already in the same set
        bool together = uf.same(a, b);
        int comp = uf.numSets;     // current number of disjoint sets
        int sz = uf.size[uf.find(a)];
        auto cc = uf.getComponents();   // representative -> members

    Tested on:
        https://judge.yosupo.jp/problem/unionfind
*/

struct UnionFind {
    int n, numSets;
    vector<int> parent;  // parent[i] == i for representatives; use find(i) otherwise
    vector<int> size;    // size[r] valid only when r is a representative

    UnionFind(int n_) : n(n_), numSets(n_), parent(n_), size(n_, 1) {
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }
    }

    int find(int i) {
        int root = i;
        while (parent[root] != root) {
            root = parent[root];
        }
        while (parent[i] != root) {  // 2-pass path compression
            int next = parent[i];
            parent[i] = root;
            i = next;
        }
        return root;
    }

    bool same(int i, int j) { return find(i) == find(j); }

    bool unite(int i, int j) {
        int pi = find(i), pj = find(j);
        if (pi == pj) {
            return false;
        }
        numSets--;  // merged two sets
        if (size[pi] < size[pj]) {  // attach smaller under larger
            swap(pi, pj);
        }
        parent[pj] = pi;
        size[pi] += size[pj];
        return true;
    }

    map<int, vector<int>> getComponents() {
        map<int, vector<int>> cc;
        for (int i = 0; i < n; i++) {
            cc[find(i)].push_back(i);
        }
        return cc;
    }
};
