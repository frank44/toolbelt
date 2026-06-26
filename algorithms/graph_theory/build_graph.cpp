#include <bits/stdc++.h>
using namespace std;


// simple unweighted graph
// vertices assumed 0-indexed in [0, n); for 1-indexed input, pass n+1
vector<vector<int>> buildGraph(int n, const vector<vector<int>>& edges, bool bidirectional=true) {
    vector<vector<int>> g(n);
    for (const auto& e : edges) {
        int u = e[0];
        int v = e[1];  
        g[u].push_back(v);
        if (bidirectional) {
            g[v].push_back(u);
        }
    }
    return g;
}


//  ------


using i64 = long long;
using Edge = pair<i64, int>; // weight, to

// weighted graph; edges are {u, v, w}. Edge ordering is {weight, to} to match dijkstra
// vertices assumed 0-indexed in [0, n); for 1-indexed input, pass n+1
vector<vector<Edge>> buildWeightedGraph(int n, const vector<vector<int>>& edges, bool bidirectional=true) {
    vector<vector<Edge>> g(n);
    for (const auto& e : edges) {
        int u = e[0];
        int v = e[1];
        i64 w = e[2];
        g[u].emplace_back(w, v);
        if (bidirectional) {
            g[v].emplace_back(w, u);
        }
    }
    return g;
}
