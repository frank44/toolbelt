#include <bits/stdc++.h>
using namespace std;
using i64 = long long;
using i128 = __int128;

/*
    Prefix Hash: hash of any substring of a fixed string, from prefix hashes.
    Polynomial hash mod 2^61 - 1 with a random base.
        build: O(n)    get: O(1)

    hash(a[l..r]) = v[l] * BASE^(r-l) + v[l+1] * BASE^(r-l-1) + ... + v[r-1] * BASE^1 + v[r]
    where v[i] = a[i] + 1, so zeros still count and different lengths don't collide.

    Usage:
        PrefixHash h(s);                // string or vector<int>
        i64 x = h.get(l, r);            // hash of s[l..r], 0-indexed, inclusive

    Notes:
        - Values must be >= 0. If the input has negative numbers, add the same
          constant to every value before hashing (e.g. x + 1e9 for x >= -1e9).
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

struct PrefixHash {
    vector<i64> pre{0};  // pre[i] = hash of the first i values

    PrefixHash(const string& s) {
        pre.reserve(s.size() + 1);
        for (char c : s) {
            pre.push_back((mult(pre.back(), BASE) + c + 1) % HASH_MOD);
        }
        growPowers(s.size());
    }

    PrefixHash(const vector<int>& a) {
        pre.reserve(a.size() + 1);
        for (int x : a) {
            pre.push_back((mult(pre.back(), BASE) + x + 1) % HASH_MOD);
        }
        growPowers(a.size());
    }

    // hash of [l, r] inclusive
    i64 get(int l, int r) const {
        return (pre[r + 1] + HASH_MOD - mult(pre[l], pw[r - l + 1])) % HASH_MOD;
    }
};
