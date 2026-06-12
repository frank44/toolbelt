/*
    Application of Fermat's little theorem to find the multiplicative inverse of X modulo P (for prime P)
*/

using i64 = long long;

i64 modpow(i64 a, i64 b, i64 mod) { // a^b % mod (note: 0^0 returns 1)
    a %= mod;
    i64 ans = 1;
    while (b > 0) {
        if (b & 1) {
            ans = ans * a % mod;
        }
        b >>= 1;
        a = a * a % mod;
    }
    return ans;
}

i64 modInverse(i64 a, i64 mod) { 
    return modpow(a, mod-2, mod);
}