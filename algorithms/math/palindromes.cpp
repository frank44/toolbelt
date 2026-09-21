#include <bits/stdc++.h>
using namespace std;

using i64 = long long;

// Precomputes all the palindromic numbers in [0, LIMIT], sorted ascending.
// Set LIMIT per problem; expect ~2*10^(d/2) entries for a d-digit LIMIT (~121k for INT_MAX).
// Keep LIMIT < ~9e17 otherwise there will be intermediate i64 overflow issues 

const i64 LIMIT = INT_MAX;
vector<i64> pals;

auto init = []() {
    pals.push_back(0); // include zero
    for (i64 i=1; ; i++) {
        // odd length palindrome (reflect all but last digit)
        i64 p = i;
        i64 x = i/10;
        while (x > 0) {
            int d = x%10;
            p = 10*p + d;
            x /= 10;
        }
        
        if (p <= LIMIT) {
            pals.push_back(p);
        } else {
            break;
        }

        // even length palindrome (reflect all)
        p = i;
        x = i;
        while (x > 0) {
            int d = x % 10;
            p = 10*p + d;
            x /= 10;
        }
        
        if (p <= LIMIT) {
            pals.push_back(p);
        }
    }
    ranges::sort(pals);
    return 0;
}();


// basic isPalidrome helper
bool isPal(i64 x) {
    vector<int> digits;
    while (x > 0) {
        digits.push_back(x%10);
        x /= 10;
    }
    int len = digits.size();
    for (int i=0, j=len-1; i<j; i++, j--) {
        if (digits[i] != digits[j]) {
            return false;
        }
    }
    return true;
}
