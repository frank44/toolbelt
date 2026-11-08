#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
using namespace std;

using i64 = long long;

/*
    Sample Digit DP Solution
        > https://leetcode.com/problems/count-good-integers-in-a-range/
*/

class Solution {
public:
    long long goodIntegers(long long l, long long r, int k) {
        return countDp(r, k) - countDp(l-1, k);
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
        reverse(digits.begin(), digits.end());
        int n = digits.size();

        // i = n, position, [0, n-1] (0 is highest ordered digit)
        // j = 10, last digit
        // k = 2, started or not
        // r = 2, restricted or not
        vector dp(n, vector(10, vector(2, vector(2, -1LL))));

        auto dfs = [&](this auto&& self, int pos, int lastDigit, int started, int restricted) -> i64 {
            if (pos == n) {
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
                if (restricted && i > digits[pos]) {
                    continue;
                }
                int nStarted = started || i > 0;
                int nR = restricted && i == digits[pos];
                ans += self(pos+1, i, nStarted, nR);
            }

            return ans;
        };

        return dfs(0, 0, 0, 1);
    }

};
