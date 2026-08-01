#include <bits/stdc++.h>
using namespace std;
using i64 = long long;

/*
    Helper that computes N choose K modulo P (for prime P)

    Complexity: Initial O(N) precomputation cost, then O(1) for subsequent queries
    
    Use: Everything is set to initially at start-up. Just set the global constants MAX_N and MOD
    and then call choose(n, k).

*/

constexpr int MAX_N = 100000;
constexpr i64 MOD = 1'000'000'007;

i64 modpow(i64 a, i64 b, i64 mod) { // a^b % mod (note: 0^0 returns 1)
    a %= mod;
    i64 ans = 1;
    while (b > 0) {
        if ((b & 1) == 1) {
            ans = (ans * a) % mod;
        }
        b >>= 1;
        a = a * a % mod;
    }
    return ans;
}

i64 modInverse(i64 a, i64 mod) { // Fermat's only works for PRIME values of mod
    return modpow(a, mod-2, mod);
}

vector<i64> fact = [] {
    vector<i64> f(MAX_N+1);
    f[0] = 1;
    for (int i=1; i<(int)f.size(); i++) {
        f[i] = i * f[i-1] % MOD;
    }
    return f;
}();

vector<i64> invFact = [] {
    vector<i64> inv(MAX_N+1);
    inv[MAX_N] = modInverse(fact[MAX_N], MOD);
    for (int i=MAX_N-1; i>=0; i--) {
        inv[i] = (i+1) * inv[i+1] % MOD;
    }
    return inv;
}();

i64 choose(int n, int k) {
    if (k < 0 || k > n) {
        return 0;
    }
    return fact[n] * invFact[k] % MOD * invFact[n-k] % MOD;
}