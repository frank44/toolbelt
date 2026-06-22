#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
using namespace std;

using i64 = long long;

i64 dfs(auto& dp, vector<int>& num, int threshold, int pos, int lastDigit, int started, int restricted);

i64 dfs(auto& dp, vector<int>& num, int threshold, int pos, int lastDigit, int started, int restricted) {
    if (pos == (int)num.size()) {
        return started;
    }

    i64& ans = dp[pos][lastDigit][started][restricted];
    if (ans != -1) {
        return ans;
    }
    ans = 0;
    for (int i=0; i<=9; i++) {
        if (started && abs(i-lastDigit) > threshold) {
            continue;
        }
        if (restricted && i > num[pos]) {
            continue;
        }
        int nStarted = started || i > 0;
        int nR = restricted && i == num[pos];
        ans += dfs(dp, num, threshold, pos+1, i, nStarted, nR);
    }

    return ans;
}

i64 countDp(i64 num, int threshold) {
    if (num <= 0) { // guard non-positive numbers
        return 0LL;
    }
    vector<int> digits;
    while (num > 0) {
        digits.push_back(num%10);
        num /= 10;
    }
    ranges::reverse(digits); 
    int n = digits.size();

    // i = n, position, [0, n-1] (0 is highest ordered digit)
    // j = 10, last digit
    // k = 2, started or not
    // r = 2, restricted or not
    vector dp(n, vector(10, vector(2, vector(2, -1LL))));
    return dfs(dp, digits, threshold, 0, 0, 0, 1);
}

i64 goodIntegers(i64 l, i64 r, int k) {
    return countDp(r, k) - countDp(l-1, k);
}