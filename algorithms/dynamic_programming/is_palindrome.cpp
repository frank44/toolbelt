#include <bits/stdc++.h>
using namespace std;

// isPal[i][j] == 1 iff s[i..j] is a palindrome, O(n^2)
vector<vector<char>> computePalindromes(const string& s) {
    int n = s.size();
    vector isPal(n, vector<char>(n, 0));
    for (int i=0; i<n; i++) {
        isPal[i][i] = 1;
        if (i>0 && s[i] == s[i-1]) {
            isPal[i-1][i] = 1;
        }
    }
    for (int l=3; l<=n; l++) {
        for (int i=0; i+l-1<n; i++) {
            if (s[i] == s[i+l-1] && isPal[i+1][i+l-2]) {
                isPal[i][i+l-1] = 1;
            }
        }
    }
    return isPal;
}