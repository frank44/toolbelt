#include <bits/stdc++.h>
using namespace std;

/*
    Longest increasing subsequence, O(n log n)

    len[i] = length of the LIS that ends at a[i]; the answer is its max

    tail[k] = smallest value that ends an increasing subsequence of length k+1

    Notes:
        - Non-decreasing: lower_bound -> upper_bound (and < to <= in recoverLis)
        - Longest decreasing: negate the values (or reverse the array) first
        - LIS starting at each i: run it on the reversed, negated array
        - Min number of non-increasing subsequences that cover a = LIS length

    Usage:
        auto len = lisEndingAt(a);
        int best = *max_element(len.begin(), len.end());   // a must be non-empty
        auto lis = recoverLis(a, len);      // indices of one LIS, increasing
*/
vector<int> lisEndingAt(vector<int>& a) {
    int n = a.size();
    vector<int> tail;
    vector<int> len(n);
    for (int i = 0; i < n; i++) {
        auto it = lower_bound(tail.begin(), tail.end(), a[i]);
        len[i] = it - tail.begin() + 1;
        if (it == tail.end()) {
            tail.push_back(a[i]);
        } else {
            *it = a[i];
        }
    }
    return len;
}

// indices of one LIS: walk right to left taking lengths best, best-1, ..., 1
vector<int> recoverLis(vector<int>& a, vector<int>& len) {
    int n = a.size();
    int need = n == 0 ? 0 : *max_element(len.begin(), len.end());
    vector<int> lis;
    for (int i = n - 1; i >= 0; i--) {
        if (len[i] == need && (lis.empty() || a[i] < a[lis.back()])) {
            lis.push_back(i);
            need--;
        }
    }
    reverse(lis.begin(), lis.end());
    return lis;
}
