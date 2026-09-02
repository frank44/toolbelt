#include <bits/stdc++.h>
#include <cassert>
using namespace std;

using i64 = long long;

/*
    Converts numbers to arbitrary bases (2 to 36, digits 0-9 then a-z)
        x - number to convert (must be non-negative)
        base - base system to convert to
        minLen - 0 pad to a certain min length

    For the inverse operation use: stoll(num, nullptr, base)
*/
string to_base(i64 x, int base, int minLen=0) {
    assert(x >= 0 && base >= 2 && base <= 36);
    string str;
    while (x > 0) {
        int d = x % base;
        str += d >= 10 ? (char)('a'+d-10) : (char)('0'+d);
        x /= base;
    }
    if (str.empty()) str += "0";

    while ((int)str.size() < minLen) {
        str += "0";
    }
    reverse(str.begin(), str.end());
    return str;
}