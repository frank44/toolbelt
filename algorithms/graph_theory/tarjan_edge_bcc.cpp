#include <bits/stdc++.h>
using namespace std;

/*
Tarjan: bridges + 2-edge-connected components + bridge tree.

Undirected graphs only. O(N + M)

Tested with:
  https://leetcode.com/problems/critical-connections-in-a-network/

Usage:
  EdgeBCC eb(n);
  for (int i = 0; i < m; i++) {
      int u, v; cin >> u >> v;
      eb.addEdge(u, v);
  }
  eb.build();

After build():
  eb.isBridge[i]   // 1 iff input edge i is a bridge (size == #addEdge calls)
  eb.compId[u]     // 2-edge-connected component id of u
  eb.compCnt       // number of 2ECCs
  eb.bridgeTree()  // adjacency of the bridge tree (nodes 0..compCnt-1)

The bridge tree: contract each 2ECC to a node; bridges become the tree edges.
Always a tree. Common uses:
  - Count leaves L; minimum edges to make the graph 2-edge-connected is
    (L + 1) / 2. (CF 1000E)
  - Tree DP / LCA on the condensed graph.

Handles multi-edges and self-loops. build() resets per-run state, so it can
be called more than once. addEdge must be called before build().
*/
struct EdgeBCC {
    int n;
    int timer;
    int compCnt;
    vector<vector<int>> adj;   // adj[u] = directed edge ids out of u
    vector<int> to;            // to[e] = head of directed edge e; twin is e ^ 1
    vector<int> tin;
    vector<int> low;
    vector<int> compId;
    vector<int> edgeU;         // one endpoint of each input edge
    vector<int> edgeV;         // the other endpoint
    vector<char> isBridgeRaw;  // per directed edge, size 2*m
    vector<char> isBridge;     // per input edge, size m

    explicit EdgeBCC(int numNodes) : n(numNodes), timer(0), compCnt(0) {
        adj.assign(n, {});
        tin.assign(n, -1);
        low.assign(n, 0);
        compId.assign(n, -1);
    }

    // Adds undirected edge u-v. Returns its input edge id.
    int addEdge(int u, int v) {
        assert(tin[0] == -1 && "addEdge called after build()");
        int edgeId = (int)to.size() / 2;
        int dirId = (int)to.size();
        to.push_back(v);
        adj[u].push_back(dirId);
        to.push_back(u);
        adj[v].push_back(dirId ^ 1);
        edgeU.push_back(u);
        edgeV.push_back(v);
        return edgeId;
    }

    void build() {
        int m = (int)to.size() / 2;
        fill(tin.begin(), tin.end(), -1);
        fill(low.begin(), low.end(), 0);
        fill(compId.begin(), compId.end(), -1);
        timer = 0;
        compCnt = 0;
        isBridgeRaw.assign(to.size(), 0);
        isBridge.assign(m, 0);

        for (int u = 0; u < n; u++) {
            if (tin[u] == -1) {
                dfs(u, -1);
            }
        }

        for (int u = 0; u < n; u++) {
            if (compId[u] == -1) {
                queue<int> q;
                q.push(u);
                compId[u] = compCnt;
                while (!q.empty()) {
                    int x = q.front();
                    q.pop();
                    for (int e : adj[x]) {
                        int y = to[e];
                        if (!isBridgeRaw[e] && compId[y] == -1) {
                            compId[y] = compCnt;
                            q.push(y);
                        }
                    }
                }
                compCnt++;
            }
        }
    }

    // Adjacency of the bridge tree. Nodes are 0..compCnt-1 (2ECC ids), and
    // edges are the bridges of the original graph.
    vector<vector<int>> bridgeTree() const {
        vector<vector<int>> tree(compCnt);
        int m = (int)edgeU.size();
        for (int i = 0; i < m; i++) {
            if (!isBridge[i]) {
                continue;
            }
            int a = compId[edgeU[i]];
            int b = compId[edgeV[i]];
            tree[a].push_back(b);
            tree[b].push_back(a);
        }
        return tree;
    }

    void dfs(int u, int parentEdge) {
        tin[u] = timer;
        low[u] = timer;
        timer++;
        for (int e : adj[u]) {
            if (e == parentEdge) {
                continue;
            }
            int v = to[e];
            if (tin[v] != -1) {
                low[u] = min(low[u], tin[v]);
            } else {
                dfs(v, e ^ 1);
                low[u] = min(low[u], low[v]);
                if (low[v] > tin[u]) {
                    isBridgeRaw[e] = 1;
                    isBridgeRaw[e ^ 1] = 1;
                    isBridge[e / 2] = 1;
                }
            }
        }
    }
};