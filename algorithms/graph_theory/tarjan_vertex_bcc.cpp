#include <bits/stdc++.h>
using namespace std;

/*
Tarjan: articulation points + biconnected components (BCC) + block-cut tree.

Undirected graphs only. O(N + M).

A biconnected component (BCC) is a maximal 2-vertex-connected subgraph.
The block-cut tree has one node per BCC and one node per articulation point,
with an edge between an articulation point and every BCC containing it. It is
always a tree.

Usage:
  VertexBCC vb(n);
  for (int i = 0; i < m; i++) {
      int u, v; cin >> u >> v;
      vb.addEdge(u, v);
  }
  vb.build();

After build():
  vb.isArt[u]         // 1 iff vertex u is an articulation point
  vb.bccCnt           // number of BCCs
  vb.artCnt           // number of articulation points
  vb.bccVertices[b]   // vertices in BCC b, b in [0, bccCnt)
  vb.bccOfVertex[u]   // BCC ids containing u

Block-cut tree (unified node ids, size = vb.treeSize):
  vb.tree[k]          // neighbors of node k
  vb.isBccNode[k]     // 1 if k is a BCC, 0 if k is an articulation point
  vb.artNode[u]       // block-cut tree node for articulation point u,
                      // or -1 if u is not an articulation point
  vb.treeSize         // bccCnt + artCnt

Mapping a vertex to a block-cut tree node:
  if (vb.isArt[u]) {
      int node = vb.artNode[u];
  } else {
      int node = vb.bccOfVertex[u][0];
  }

Handles multi-edges. Self-loops are ignored. build() resets per-run state.
addEdge must be called before build().
*/
struct VertexBCC {
    int n;
    int timer;
    int bccCnt;
    int artCnt;
    int treeSize;
    vector<vector<int>> adj;          // adj[u] = directed edge ids out of u
    vector<int> to;                   // to[e] = head of directed edge e; twin is e ^ 1
    vector<int> tin;
    vector<int> low;
    vector<char> isArt;
    vector<int> artNode;              // vertex -> block-cut tree node, or -1
    vector<vector<int>> bccOfVertex;  // vertex -> BCC ids containing it
    vector<vector<int>> bccVertices;  // BCC id -> vertices in it
    vector<int> edgeStack;            // stack of directed edge ids during DFS
    vector<char> isBccNode;           // per block-cut tree node
    vector<vector<int>> tree;         // block-cut tree adjacency

    explicit VertexBCC(int numNodes)
        : n(numNodes), timer(0), bccCnt(0), artCnt(0), treeSize(0) {
        adj.assign(n, {});
        tin.assign(n, -1);
        low.assign(n, 0);
        isArt.assign(n, 0);
        artNode.assign(n, -1);
        bccOfVertex.assign(n, {});
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
        fill(tin.begin(), tin.end(), -1);
        fill(low.begin(), low.end(), 0);
        fill(isArt.begin(), isArt.end(), 0);
        fill(artNode.begin(), artNode.end(), -1);
        for (int u = 0; u < n; u++) {
            bccOfVertex[u].clear();
        }
        bccVertices.clear();
        edgeStack.clear();
        timer = 0;
        bccCnt = 0;
        artCnt = 0;

        for (int u = 0; u < n; u++) {
            if (tin[u] == -1) {
                dfs(u, -1, true);
            }
        }

        for (int u = 0; u < n; u++) {
            if (isArt[u]) {
                artNode[u] = bccCnt + artCnt;
                artCnt++;
            }
        }

        treeSize = bccCnt + artCnt;
        tree.assign(treeSize, {});
        isBccNode.assign(treeSize, 0);
        for (int b = 0; b < bccCnt; b++) {
            isBccNode[b] = 1;
        }
        for (int u = 0; u < n; u++) {
            if (!isArt[u]) {
                continue;
            }
            int a = artNode[u];
            for (int b : bccOfVertex[u]) {
                tree[a].push_back(b);
                tree[b].push_back(a);
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
                if (tin[v] < tin[u]) {
                    edgeStack.push_back(e);
                    low[u] = min(low[u], tin[v]);
                }
            } else {
                edgeStack.push_back(e);
                dfs(v, e ^ 1, false);
                low[u] = min(low[u], low[v]);
                if (low[v] >= tin[u]) {
                    popBcc(e);
                    if (!isRoot) {
                        isArt[u] = 1;
                    }
                }
                children++;
            }
        }
        if (isRoot && children > 1) {
            isArt[u] = 1;
        }
    }

    // Pops edges up to and including target, forming one BCC.
    void popBcc(int target) {
        int bccId = bccCnt;
        bccCnt++;
        bccVertices.push_back({});
        while (!edgeStack.empty()) {
            int e = edgeStack.back();
            edgeStack.pop_back();
            int a = to[e ^ 1];
            int b = to[e];
            if (bccOfVertex[a].empty() || bccOfVertex[a].back() != bccId) {
                bccOfVertex[a].push_back(bccId);
                bccVertices[bccId].push_back(a);
            }
            if (bccOfVertex[b].empty() || bccOfVertex[b].back() != bccId) {
                bccOfVertex[b].push_back(bccId);
                bccVertices[bccId].push_back(b);
            }
            if (e == target) {
                break;
            }
        }
    }
};