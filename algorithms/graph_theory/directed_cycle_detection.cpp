#include <bits/stdc++.h>
using namespace std;

/*
    Cycle detection in a DIRECTED graph by 3-color DFS, O(n + m).
    A cycle exists iff the DFS finds a back edge: an edge u -> v with v
    still on the recursion stack (state 1)

    Notes:
        - Directed only. On an undirected adjacency list every edge u-v is
          also v-u, so this reports a cycle everywhere; use UnionFind there
        - topologicalSort(g).size() < n gives the same answer iteratively
          (no recursion depth concerns)
        - Self-loops and parallel edges are handled

    Usage:
        if (hasCycle(g)) { ... }
*/
bool hasCycle(const vector<vector<int>>& g) {
    int n = g.size();
    vector<int> state(n); // 0 = unvisited, 1 = on recursion stack, 2 = done

    auto dfs = [&](auto&& self, int u) -> bool {
        if (state[u] == 1) { // back edge
            return true;
        }
        if (state[u] == 2) { // already fully explored, no cycle through here
            return false;
        }
        state[u] = 1;
        for (int v : g[u]) {
            if (self(self, v)) {
                return true;
            }
        }
        state[u] = 2;
        return false;
    };

    for (int u = 0; u < n; u++) {
        if (state[u] == 0 && dfs(dfs, u)) {
            return true;
        }
    }
    return false;
}
