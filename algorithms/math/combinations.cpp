#include <bits/stdc++.h>
using namespace std;
using i64 = long long;

/*
    Exact nCr calculation in O(N), for answers that fit in a 64-bit integer, otherwise returns -1
        > Handles huge n with small m, e.g. choose(2'000'000'000, 3)
        > Out-of-range inputs (m < 0 or m > n) return 0
        > Intermediate calculations never exceed the final answer, so no overflow danger as long as the answer itself fits.  
    
        O(min(m, n-m)) per call
*/
i64 choose(i64 n, i64 m) {
    if (m < 0 || m > n) {
        return 0;
    }
    m = min(m, n-m);
    __int128 ans = 1;
    for (int i=1; i<=m; i++) {
        ans = ans * (n + 1 - i) / i; 
        if (ans > INT64_MAX) { // overflow case
            return -1;
        }
    }
    return ans;
}