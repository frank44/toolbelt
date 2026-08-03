// github.com/frank44/toolbelt — prewritten template
#include <bits/stdc++.h>
using namespace std;
using i64 = long long;

#ifdef LOCAL
template<class T> void dbg(const T& x);
template<class A, class B> void dbg(const pair<A,B>& p) { cout << "("; dbg(p.first); cout << ", "; dbg(p.second); cout << ")"; }
template<class... T> void dbg(const tuple<T...>& t) { cout << "("; apply([](auto&&... a){ int i = 0; ((cout << (i++ ? ", " : ""), dbg(a)), ...); }, t); cout << ")"; }
template<class T> constexpr bool dbg_printable = requires(const T& t) { cout << t; };
template<class T> constexpr bool dbg_iterable = requires(const T& t) { begin(t); end(t); };
inline int dbg_depth = 0; // indent level for nested multi-line containers
template<class T> void dbg(const T& x) {
    if constexpr (dbg_printable<T>) { cout << x; }
    else {
        using E = remove_cvref_t<decltype(*begin(x))>;
        if constexpr (dbg_iterable<E> && !dbg_printable<E>) { // container of containers -> one row per line
            cout << "{\n"; dbg_depth++;
            for (auto&& row : x) { cout << string(2 * dbg_depth, ' '); dbg(row); cout << "\n"; }
            dbg_depth--; cout << string(2 * dbg_depth, ' ') << "}";
        } else { cout << "{"; int i = 0; for (auto&& e : x) { cout << (i++ ? ", " : ""); dbg(e); } cout << "}"; }
    }
}
template<class T, class C> void dbg(stack<T,C> s) { cout << "{"; int i = 0; while (!s.empty()) { cout << (i++ ? ", " : ""); dbg(s.top()); s.pop(); } cout << "}"; }
template<class T, class C> void dbg(queue<T,C> q) { cout << "{"; int i = 0; while (!q.empty()) { cout << (i++ ? ", " : ""); dbg(q.front()); q.pop(); } cout << "}"; }
template<class T, class C, class P> void dbg(priority_queue<T,C,P> q) { cout << "{"; int i = 0; while (!q.empty()) { cout << (i++ ? ", " : ""); dbg(q.top()); q.pop(); } cout << "}"; }
template<class... T> void dbg_all(const T&... x) { int i = 0; ((cout << (i++ ? ", " : ""), dbg(x)), ...); }
#define debug(...) cout << "[" << #__VA_ARGS__ << "] = ", dbg_all(__VA_ARGS__), cout << "\n"
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
