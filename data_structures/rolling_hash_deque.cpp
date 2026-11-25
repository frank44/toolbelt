#include <bits/stdc++.h>
using namespace std;
using i64 = long long;
using i128 = __int128;

/*
    Deque Hash: hash of one string that grows / shrinks at either end.
    Polynomial hash mod 2^61 - 1 with a random base.
        every operation: O(1)

    hash(a[0..n-1]) = a[0] * BASE^(n-1) + a[1] * BASE^(n-2) + ... + a[n-2] * BASE^1 + a[n-1]
    Same value as PrefixHash (rolling_hash_prefix.cpp), so the two are comparable
    when both are pasted into one file (keep one copy of the shared globals).

    Usage:
        DequeHash d;                    // empty string
        d.pushBack(c);   d.pushFront(c);
        d.popBack(c);    d.popFront(c); // c = the value being removed
        d.h                             // hash of the current string
        d.len                           // its length

        Sliding window of size k:  d.pushBack(a[i]);  d.popFront(a[i - k]);

    Notes:
        - Values must be in [0, HASH_MOD). If the input has negative numbers, add the
          same constant to every value before hashing (e.g. x + 1e9 for x >= -1e9).
        - Value 0 is only safe when the compared strings have equal length
          ({0} and {0, 0} both hash to 0). If lengths can differ, shift values by +1.
*/

const i64 HASH_MOD = (1LL << 61) - 1;
const i64 BASE = [] {
    mt19937_64 gen(chrono::steady_clock::now().time_since_epoch().count());
    return uniform_int_distribution<i64>(1e6, HASH_MOD - 2)(gen);
}();

i64 mult(i64 a, i64 b) {
    return (i128)a * b % HASH_MOD;
}

vector<i64> pw{1};  // pw[i] = BASE^i, grown on demand

void growPowers(int n) {
    while ((int)pw.size() <= n) {
        pw.push_back(mult(pw.back(), BASE));
    }
}

// BASE^(HASH_MOD - 2): only popBack needs it
const i64 INV_BASE = [] {
    i64 ans = 1;
    i64 a = BASE;
    for (i64 e = HASH_MOD - 2; e > 0; e >>= 1) {
        if (e & 1) {
            ans = mult(ans, a);
        }
        a = mult(a, a);
    }
    return ans;
}();

struct DequeHash {
    i64 h = 0;    // hash of the current string
    int len = 0;

    void pushBack(i64 x) {
        growPowers(len);
        h = (mult(h, BASE) + x) % HASH_MOD;
        len++;
    }

    void pushFront(i64 x) {
        growPowers(len);
        h = (mult(x, pw[len]) + h) % HASH_MOD;
        len++;
    }

    // x = the current last value
    void popBack(i64 x) {
        h = mult(h + HASH_MOD - x, INV_BASE);
        len--;
    }

    // x = the current first value
    void popFront(i64 x) {
        len--;
        h = (h + HASH_MOD - mult(x, pw[len])) % HASH_MOD;
    }
};
