// github.com/frank44/toolbelt — prewritten template
#include <bits/stdc++.h>
using namespace std;
using i64 = long long;

#ifdef LOCAL
template<class T> void dbg(const T& x);
template<class A, class B> void dbg(const pair<A,B>& p) { cerr << "("; dbg(p.first); cerr << ", "; dbg(p.second); cerr << ")"; }
template<class... T> void dbg(const tuple<T...>& t) { cerr << "("; apply([](auto&&... a){ int i = 0; ((cerr << (i++ ? ", " : ""), dbg(a)), ...); }, t); cerr << ")"; }
template<class T> void dbg(const T& x) {
    if constexpr (requires { cerr << x; }) { cerr << x; }
    else { cerr << "{"; int i = 0; for (auto&& e : x) { cerr << (i++ ? ", " : ""); dbg(e); } cerr << "}"; }
}
template<class T, class C> void dbg(stack<T,C> s) { cerr << "{"; int i = 0; while (!s.empty()) { cerr << (i++ ? ", " : ""); dbg(s.top()); s.pop(); } cerr << "}"; }
template<class T, class C> void dbg(queue<T,C> q) { cerr << "{"; int i = 0; while (!q.empty()) { cerr << (i++ ? ", " : ""); dbg(q.front()); q.pop(); } cerr << "}"; }
template<class T, class C, class P> void dbg(priority_queue<T,C,P> q) { cerr << "{"; int i = 0; while (!q.empty()) { cerr << (i++ ? ", " : ""); dbg(q.top()); q.pop(); } cerr << "}"; }
template<class... T> void dbg_all(const T&... x) { int i = 0; ((cerr << (i++ ? ", " : ""), dbg(x)), ...); }
#define debug(...) cout.flush(), cerr << "[" << #__VA_ARGS__ << "] = ", dbg_all(__VA_ARGS__), cerr << "\n"
#else
#define debug(...)
#endif

template<class T> bool ckmin(T& a, const T& b) { return b < a ? a = b, true : false; }
template<class T> bool ckmax(T& a, const T& b) { return a < b ? a = b, true : false; }

const i64 MOD = 1e9 + 7;
const i64 INF = 1'000'000'000'000'000'000LL; // 1e18

void solve() {

}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t = 1;
    cin >> t;  // comment out for single-test problems
    while (t--) {
        solve();
    }
    return 0;
}
