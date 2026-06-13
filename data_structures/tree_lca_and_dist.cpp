#include <bits/stdc++.h>
using namespace std;

/*
    Utility to find LCA and distance between arbitrary tree nodes for static graphs.
    
    Build: O(nlogn) - it builds a Euler path and a range-minimum-query data structure (sparse table) to compute the LCA
    Query: O(1)

    Usage:
        TreeUtil tu(numNodes, adjList, root); 

        int w = tu.getLCA(u, v);
        int dist = tu.getDist(u, v); // assumes edges all weigh 1
*/

template <typename T>
struct SparseTableMin {
    int n, k;
    vector<vector<pair<T, int>>> t;  // t[j][i] = {min value, its index} over [i, i + 2^j)

    SparseTableMin() {}

    SparseTableMin(const vector<T>& arr) {
        build(arr);
    }

    void build(const vector<T>& arr) {
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


struct TreeUtil {
    vector<int> path; // euler path
    vector<int> depth; // depth of euler path nodes
    vector<int> dist; // distance from root
    vector<int> first; // index of first occurance within euler path 
    SparseTableMin<int> st;

    TreeUtil(int n, const vector<vector<int>>& g, int root) {
        assert(n > 0);
        dist.assign(n, -1);
        first.assign(n, -1);
        tour(g, root, -1, 0);

        for (int i=0; i<(int)path.size(); i++) {
            int u = path[i];
            if (first[u] == -1) {
                first[u] = i;
            }
        }
        st.build(depth);
    }

    void tour(const vector<vector<int>>& g, int cur, int prev, int d) { // canonical euler tour / dfs
        path.push_back(cur);
        depth.push_back(d);
        dist[cur] = d;

        for (int next : g[cur]) {
            if (next == prev) {
                continue;
            }
            tour(g, next, cur, d+1);
            path.push_back(cur);
            depth.push_back(d);
        }
    }

    int getLCA(int u, int v) const {
        int L = first[u];
        int R = first[v];
        if (L > R) {
            swap(L, R);
        }
        return path[st.query(L, R+1).second];
    }

    // assumes all edges are of weight 1
    int getDist(int u, int v) const {
        int lca = getLCA(u, v);
        return dist[u] + dist[v] - 2*dist[lca];
    }
};