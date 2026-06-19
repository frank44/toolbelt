#include <bits/stdc++.h>
using namespace std;

// TESTED ON: https://cses.fi/problemset/task/1639/
    
/*
    The edit distance between two strings is the minimum number of 
    operations required to transform one string into the other
    
    The allowed operations are:
        Add one character to the string
        Remove one character from the string
        Replace one character in the string
    */
int editDistance(string s1, string s2) {
    int n = s1.size();
    int m = s2.size();
    vector<vector<int>> dp(n+1, vector<int>(m+1));
    for (int i=0; i<=n; i++) {
        dp[i][0] = i;
    }
    for (int j=0; j<=m; j++) {
        dp[0][j] = j;
    }

    for (int i=1; i<=n; i++) {
        for (int j=1; j<=m; j++) {
            if (s1[i-1] == s2[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            } else {
                                //     edit        delete     insert
                dp[i][j] = 1 + min({dp[i-1][j-1], dp[i-1][j], dp[i][j-1]});
            }
        }
    }
    return dp[n][m];
}