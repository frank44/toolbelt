#include <bits/stdc++.h>
using namespace std;

/*
    2D longest increasing subsequence (Russian doll envelopes), O(n log n)

    Length of the longest chain of pairs where BOTH coordinates increase from
    one pair to the next. The pairs may be picked in any order

    Sort by first ascending, then run a 1D LIS on second. Ties on first are
    sorted by second DESCENDING, so two pairs with the same first can never
    both land in a strictly increasing run of second

    Notes:
        - Equal coordinates allowed: plain sort(p.begin(), p.end()) and
          lower_bound -> upper_bound
        - Boxes that can rotate: normalize each pair to (min, max) first

    Usage:
        vector<pair<int, int>> p = {...};
        int best = longestChain2D(p);
*/
int longestChain2D(vector<pair<int, int>> p) {
    sort(p.begin(), p.end(), [](auto& a, auto& b) {
        if (a.first != b.first) {
            return a.first < b.first;   // first ascending
        }
        return a.second > b.second;     // ties: second descending

    });

    vector<int> tail;
    for (auto [x, y] : p) {
        auto it = lower_bound(tail.begin(), tail.end(), y);
        if (it == tail.end()) {
            tail.push_back(y);
        } else {
            *it = y;
        }
    }
    return tail.size();
}
