#include <bits/stdc++.h>
using namespace std;

using i64 = long long;
using Edge = pair<i64, int>; // weight, v

const i64 INF = 1'000'000'000'000'000'000LL; // 1e18

vector<i64> dijkstra(const vector<vector<Edge>>& g, int source) {
    int n = g.size();
    vector<i64> dist(n, INF);
    priority_queue<Edge, vector<Edge>, greater<Edge>> pq; // min-heap
    pq.emplace(0LL, source);
    dist[source] = 0;

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (d > dist[u]) {
            continue;
        }
        for (const auto& [weight, to] : g[u]) {
            if (d + weight < dist[to]) {
                dist[to] = d + weight;
                pq.emplace(d + weight, to);
            }
        }
    }
    return dist;
}