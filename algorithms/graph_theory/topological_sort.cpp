#include <bits/stdc++.h>
using namespace std;

/*
    Kahn's algorithm: topological order of a directed graph, O(n + m)

    Returns the vertices in an order where every edge u -> v has u before v

    If g has a cycle the order is partial: order.size() < n, and the missing
    vertices are exactly the ones on a cycle or reachable from one

    Notes:
        - Lexicographically smallest order: swap the queue for
              priority_queue / min-heap
        - Iterative, so no recursion depth issues on n = 1e6

    Usage:
        auto order = topologicalSort(g);
        if ((int)order.size() < n) { // cycle
            ...
        }
        for (int u : order) {        // DP along the order, e.g. longest path
            for (int v : g[u]) {
                ckmax(dp[v], dp[u] + 1);
            }
        }
*/
vector<int> topologicalSort(const vector<vector<int>>& g) {
    int n = g.size();
    vector<int> inDegree(n);
    for (int u = 0; u < n; u++) {
        for (int v : g[u]) {
            inDegree[v]++;
        }
    }

    queue<int> q;
    for (int u = 0; u < n; u++) {
        if (inDegree[u] == 0) {
            q.push(u);
        }
    }

    vector<int> order;
    order.reserve(n);
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        order.push_back(u);
        for (int v : g[u]) {
            if (--inDegree[v] == 0) {
                q.push(v);
            }
        }
    }
    return order;
}
