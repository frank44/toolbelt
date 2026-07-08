#include <bits/stdc++.h>
using namespace std;

/*
    Sieve that precomputes the smallest prime factor for all numbers [1...MAX_N].
    Plus so additional factoring related helper methods.

    Time: O(N*loglogN) sieve build
    Space: O(N)

    Tested using: https://leetcode.com/problems/divisible-game/
    Note: to only run once, drop this at global scope
*/
const int MAX_N = 1'000'000;
vector<int> spf = []{ // smallest-prime-factor sieve
    vector<int> s(MAX_N+1);
    s[0] = s[1] = 1; // 0, 1 edge cases
    for (int i=2; i<=MAX_N; i++) {
        if (s[i] == 0) {
            for (int j=i; j<=MAX_N; j+=i) {
                if (s[j] == 0) {
                    s[j] = i;
                }
            }
        }
    }
    return s;
}();

// returns all deduped prime factors in ascending order
vector<int> uniquePrimeFactors(int x) {
    vector<int> primes;
    while (x > 1) {
        int p = spf[x];
        primes.push_back(p);
        while (x % p == 0) {
            x /= p;
        }
    }
    return primes;
}

// returns all prime factors in ascending order (includes dupes)
vector<int> primeFactors(int x) {
    vector<int> primes;
    while (x > 1) {
        int p = spf[x];
        while (x % p == 0) {
            primes.push_back(p);
            x /= p;
        }
    }
    return primes;
}


// returns the prime factorization of x: p0^a0 * p1^a1 * p2^a2... as (p0, a0), (p1, a1), (p2, a2)...
// in ascending order of primes
vector<pair<int,int>> factor(int x) {
    vector<pair<int,int>> primes;
    while (x > 1) {
        int p = spf[x];
        int ct = 0;
        while (x % p == 0) {
            ct++;
            x /= p;
        }
        primes.push_back(make_pair(p, ct));
    }
    return primes;
}
