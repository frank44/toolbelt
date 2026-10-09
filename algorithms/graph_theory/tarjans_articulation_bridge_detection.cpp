#include <bits/stdc++.h>
using namespace std;

/*
 * Tarjan: bridges + articulation points + 2-edge-connected components.
 *
 * Undirected graphs only. O(N + M).
 *
 * Tested with:
 *   https://leetcode.com/problems/critical-connections-in-a-network/
 *
 * Usage:
 *   Tarjan tj(n);
 *   for (int i = 0; i < m; i++) {
 *       int u, v; cin >> u >> v; --u; --v;
 *       tj.addEdge(u, v);
 *   }
 *   tj.build();
 *
 * After build():
 *   tj.isBridge[i]  // 1 iff input edge i is a bridge (size == #addEdge calls)
 *   tj.isArt[u]     // 1 iff vertex u is an articulation point
 *   tj.compId[u]    // 2-edge-connected component id of u
 *   tj.compCnt      // number of 2ECCs
 *
 * Contracting each 2ECC gives the bridge tree (nodes 0..compCnt-1, edges are the
 * bridges). Bridges and articulation points are the usual "removal disconnects
 * the graph" edges/vertices. For vertex-biconnected components (block-cut tree),
 * use a separate edge-stack DFS -- not computed here.
 *
 * Bridge tree recipe:
 *   vector<vector<int>> tree(compCnt);
 *   for (int i = 0; i < m; i++) {
 *       if (!isBridge[i]) { continue; }
 *       int a = compId[to[2 * i]];
 *       int b = compId[to[2 * i + 1]];
 *       tree[a].push_back(b);
 *       tree[b].push_back(a);
 *   }
 *
 * Handles multi-edges and self-loops. build() resets per-run state, so it can
 * be called more than once. addEdge must be called before build().
 *
 * Two things to remember cold:
 *   - isBridge is indexed by input edge (0..m-1), not by directed edge.
 *     No 2*i at the call site.
 *   - to[2*i] / to[2*i+1] are the two endpoints of input edge i, in reverse
 *     of addEdge order. Only relevant if you build the bridge tree.
 */
struct Tarjan {
    int n;
    int timer;
    int compCnt;
    vector<vector<int>> adj;   // adj[u] = directed edge ids out of u
    vector<int> to;            // to[e] = head of directed edge e; twin is e ^ 1
    vector<int> tin;
    vector<int> low;
    vector<int> compId;
    vector<char> isBridgeRaw;  // per directed edge, size 2*m
    vector<char> isBridge;     // per input edge, size m
    vector<char> isArt;

    explicit Tarjan(int numNodes) : n(numNodes), timer(0), compCnt(0) {
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
        isArt.assign(n, 0);

        for (int u = 0; u < n; u++) {
            if (tin[u] == -1) {
                dfs(u, -1, true);
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

    void dfs(int u, int parentEdge, bool isRoot) {
        tin[u] = timer;
        low[u] = timer;
        timer++;
        int children = 0;
        for (int e : adj[u]) {
            if (e == parentEdge) {
                continue;
            }
            int v = to[e];
            if (tin[v] != -1) {
                low[u] = min(low[u], tin[v]);
            } else {
                dfs(v, e ^ 1, false);
                low[u] = min(low[u], low[v]);
                if (low[v] > tin[u]) {
                    isBridgeRaw[e] = 1;
                    isBridgeRaw[e ^ 1] = 1;
                    isBridge[e / 2] = 1;
                }
                if (!isRoot && low[v] >= tin[u]) {
                    isArt[u] = 1;
                }
                children++;
            }
        }
        if (isRoot && children > 1) {
            isArt[u] = 1;
        }
    }
};