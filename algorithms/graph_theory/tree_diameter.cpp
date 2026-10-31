#include <bits/stdc++.h>
using namespace std;

/*
    Diameter and center of an unweighted tree by double BFS, O(n)

    BFS from any vertex: the farthest vertex u is one end of a diameter.
    BFS from u: the farthest vertex v is the other end. BFS from v as well,
    and the pair (du, dv) answers most questions about the tree:
        - diameter = du[v]
        - eccentricity of i (distance to its farthest vertex) = max(du[i], dv[i])
        - i is an endpoint of some diameter  <=>  max(du[i], dv[i]) == diameter
        - i lies on the u..v path            <=>  du[i] + dv[i] == diameter
        - center = middle of the u..v path: one vertex if diameter is even,
          two adjacent vertices if odd. Every diameter passes through it:
              even: the two endpoints sit in different subtrees of center[0]
              odd:  one endpoint on each side of the edge center[0]-center[1],
                    i.e. one with du == diameter and one with dv == diameter
        - radius = (diameter + 1) / 2

    Notes:
        - Iterative, so no recursion depth issues on n = 1e6
        - bfs() alone is plain single-source unweighted distances, -1 = unreachable
        - Assumes g is connected (a tree); for a forest run it per component

    Usage:
        auto [u, v, diameter, du, dv, center] = treeDiameter(g);
*/
vector<int> bfs(const vector<vector<int>>& g, int source) {
    int n = g.size();
    vector<int> dist(n, -1);
    queue<int> q;
    q.push(source);
    dist[source] = 0;
    while (!q.empty()) {
        int i = q.front();
        q.pop();
        for (int next : g[i]) {
            if (dist[next] != -1) {
                continue;
            }
            q.push(next);
            dist[next] = 1 + dist[i];
        }
    }
    return dist;
}

struct TreeDiameter {
    int u, v;            // endpoints of one diameter
    int diameter;        // number of edges on the u..v path
    vector<int> du, dv;  // distances from u and from v
    vector<int> center;  // 1 vertex (even diameter) or 2 adjacent vertices (odd), u's side first
};

TreeDiameter treeDiameter(const vector<vector<int>>& g) {
    int n = g.size();
    vector<int> d0 = bfs(g, 0);
    int u = max_element(d0.begin(), d0.end()) - d0.begin();
    vector<int> du = bfs(g, u);
    int v = max_element(du.begin(), du.end()) - du.begin();
    vector<int> dv = bfs(g, v);
    int diameter = du[v];

    vector<int> center;
    for (int i = 0; i < n; i++) {
        if (du[i] + dv[i] == diameter && abs(du[i] - dv[i]) <= 1) {
            center.push_back(i);
        }
    }
    if (center.size() == 2 && du[center[0]] > du[center[1]]) {
        swap(center[0], center[1]);
    }
    return TreeDiameter{u, v, diameter, move(du), move(dv), move(center)};
}
