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
