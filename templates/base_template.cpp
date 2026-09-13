// github.com/frank44/toolbelt — prewritten template
#include <bits/stdc++.h>
using namespace std;
using i64 = long long;
using i128 = __int128;

#ifdef LOCAL
template<class T> void dbg(const T& x);
inline void dbg(i128 x) { if (x < 0) { cout << '-'; x = -x; } string s; do { s += char('0' + int(x % 10)); x /= 10; } while (x); reverse(s.begin(), s.end()); cout << s; }
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
inline vector<string> dbg_names(const char* s) { // split #__VA_ARGS__ at top-level commas
    vector<string> out; string cur; int depth = 0; bool quoted = false;
    for (const char* p = s; *p; p++) {
        if (*p == '"' && (p == s || p[-1] != '\\')) quoted = !quoted;
        if (!quoted) {
            if (strchr("([{", *p)) depth++;
            if (strchr(")]}", *p)) depth--;
            if (*p == ',' && depth == 0) { out.push_back(cur); cur.clear(); if (p[1] == ' ') p++; continue; }
        }
        cur += *p;
    }
    out.push_back(cur);
    return out;
}
// debug(...) prints args left to right: string literals plainly (flavor text), everything else as name = value.
// e.g. debug("phase 2", i, fixed) -> phase 2 i = 4, fixed = {0, 7}
template<class... T> void dbg_all(const char* names, const T&... x) {
    auto ns = dbg_names(names);
    int i = 0; bool prevText = false;
    auto one = [&](const auto& v) {
        constexpr bool text = is_convertible_v<remove_cvref_t<decltype(v)>, const char*>;
        if (i) cout << (text || prevText ? " " : ", ");
        if constexpr (text) cout << v;
        else { cout << ns[i] << " = "; dbg(v); }
        prevText = text; i++;
    };
    (one(x), ...);
}
#define debug(...) dbg_all(#__VA_ARGS__, __VA_ARGS__), cout << endl
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
