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

    The struct owns a copy of the current values, so pops take no argument and the
    deque-style accessors below are available. This costs O(len) memory.

    Usage:
        DequeHash d;                    // empty string
        d.push_back(c);  d.push_front(c);
        d.pop_back();    d.pop_front(); // never call on an empty DequeHash
        d.h                             // hash of the current string
        d.size()                        // its length
        d.empty(), d.front(), d.back()  // same as on a std::deque
        d.values                        // the current values, front to back

        Sliding window of size k:  d.push_back(a[i]);  if (d.size() > k) d.pop_front();

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

// ensure pw[0..n] exist
void growPowers(size_t n) {
    while (pw.size() <= n) {
        pw.push_back(mult(pw.back(), BASE));
    }
}

// BASE^(HASH_MOD - 2) = BASE^-1 mod HASH_MOD: only pop_back needs it
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
    i64 h = 0;          // hash of the current string
    deque<i64> values;  // the current string, front to back

    size_t size() const { return values.size(); }
    bool empty() const { return values.empty(); }
    i64 front() const { return values.front(); }
    i64 back() const { return values.back(); }

    // append x: h = h * BASE + x
    void push_back(i64 x) {
        h = (mult(h, BASE) + x) % HASH_MOD;
        values.push_back(x);
        growPowers(values.size());
    }

    // prepend x: h = x * BASE^len + h
    void push_front(i64 x) {
        growPowers(values.size());
        h = (mult(x, pw[values.size()]) + h) % HASH_MOD;
        values.push_front(x);
    }

    // remove the last value: h = (h - back()) / BASE
    void pop_back() {
        h = mult(h + HASH_MOD - values.back(), INV_BASE);
        values.pop_back();
    }

    // remove the first value: h = h - front() * BASE^(len - 1)
    void pop_front() {
        i64 x = values.front();
        values.pop_front();
        h = (h + HASH_MOD - mult(x, pw[values.size()])) % HASH_MOD;
    }
};
