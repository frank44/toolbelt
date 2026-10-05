#include <bits/stdc++.h>
using namespace std;

/*
    Kosaraju's SCCs on a directed graph, O(n + m).
    Returns {k, comp}: k components, comp[v] in [0, k).

    Component ids are in topological order of the condensation:
        every edge u -> v has comp[u] <= comp[v] (comp 0 is a source)

    Usage: 
        auto [k, comp] = kosaraju(g);
        auto gCondensed = buildCondensedGraph(g, k, comp);
*/
pair<int, vector<int>> kosaraju(const vector<vector<int>>& g) {
    int n = g.size();
    vector<vector<int>> gRev(n); // g with every edge reversed
    for (int u = 0; u < n; u++) {
        for (int v : g[u]) {
            gRev[v].push_back(u);
        }
    }

    // appends every unseen vertex reachable from u in graph to out, in post-order
    vector<char> seen(n);
    auto dfs = [&](auto&& self, const vector<vector<int>>& graph, vector<int>& out, int u) -> void {
        seen[u] = 1;
        for (int v : graph[u]) {
            if (!seen[v]) {
                self(self, graph, out, v);
            }
        }
        out.push_back(u);
    };

    // pass 1: post-order of g
    vector<int> order;
    for (int u = 0; u < n; u++) {
        if (!seen[u]) {
            dfs(dfs, g, order, u);
        }
    }

    // pass 2: flood gRev in reverse post-order, each flood is exactly one SCC
    seen.assign(n, 0);
    int k = 0;
    vector<int> comp(n);
    for (int i = n - 1; i >= 0; i--) {
        int u = order[i];
        if (!seen[u]) {
            vector<int> scc;
            dfs(dfs, gRev, scc, u);
            for (int v : scc) {
                comp[v] = k;
            }
            k++;
        }
    }
    return {k, comp};
}

/*
    Condensation DAG: one node per SCC, edge comp[u] -> comp[v] for every
    edge u -> v crossing components. Keeps parallel edges (sort + unique
    each list if counting paths).

    Node ids are already toposorted, so DP is a plain loop:
        sinks first:   for (int c = k - 1; c >= 0; c--)
        sources first: for (int c = 0; c < k; c++)

    Usage: auto dag = buildCondensedGraph(g, k, comp);
*/
vector<vector<int>> buildCondensedGraph(const vector<vector<int>>& g, int k, const vector<int>& comp) {
    int n = g.size();
    vector<vector<int>> dag(k);
    for (int u = 0; u < n; u++) {
        for (int v : g[u]) {
            if (comp[u] != comp[v]) {
                dag[comp[u]].push_back(comp[v]);
            }
        }
    }
    return dag;
}
