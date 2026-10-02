#include <bits/stdc++.h>
using namespace std;

using i64 = long long;

/*
    Nearest palindrome to num, excluding num itself. Ties go to the smaller one.
        num - must be in [1, 1e18)  (19 digits would overflow p10 / the candidates)

    Idea: the answer keeps the high half of num (maybe +-1) and mirrors it onto the
    low half, unless the digit count changes. That leaves 5 candidates, O(len) total:
        99..9   (len-1 digits)       e.g. 1000  -> 999
        10..01  (len+1 digits)       e.g. 999   -> 1001
        mirror(half + d), d in {-1, 0, 1}
                                     e.g. 12345 -> 12221, 12321, 12421

    Tested using: https://leetcode.com/problems/find-the-closest-palindrome/description/ 
*/

// Appends the digits of ext, reversed, onto base: mirror(123, 12) = 12321.
i64 mirror(i64 base, i64 ext) {
    while (ext > 0) {
        base = 10*base + ext%10;
        ext /= 10;
    }
    return base;
}

i64 nearestPalindrome(i64 num) {
    int len = to_string(num).size();

    vector<i64> p10(19, 1);
    for (int i=1; i<19; i++) {
        p10[i] = 10*p10[i-1];
    }

    int halfLen = len - len/2;
    i64 half = num / p10[len/2];

    vector<i64> cands{p10[len-1] - 1, p10[len] + 1};
    for (i64 h : {half-1, half, half+1}) {
        if (h < p10[halfLen-1] || h >= p10[halfLen]) {
            continue;
        }
        // odd len: the middle digit is not repeated, so drop it before reflecting
        cands.push_back(mirror(h, len%2 ? h/10 : h));
    }

    i64 ans = -1;
    for (i64 c : cands) {
        if (c != num && (ans == -1 || pair{abs(c-num), c} < pair{abs(ans-num), ans})) {
            ans = c;
        }
    }
    return ans;
}
