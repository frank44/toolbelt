// github.com/frank44/toolbelt — prewritten template
#include <bits/stdc++.h>
using namespace std;
using i64 = long long;
using i128 = __int128;

// #define LOCAL // LeetCode has no -DLOCAL: uncomment to enable debug/trace there, re-comment to submit
#ifdef LOCAL
template<class T> void dbg(const T& x);
inline void dbg(i128 x) { if (x < 0) { cout << '-'; x = -x; } string s; do { s += char('0' + int(x % 10)); x /= 10; } while (x); reverse(s.begin(), s.end()); cout << s; }
template<class A, class B> void dbg(const pair<A,B>& p) { cout << "("; dbg(p.first); cout << ", "; dbg(p.second); cout << ")"; }
template<class... T> void dbg(const tuple<T...>& t) { cout << "("; apply([](auto&&... a){ int i = 0; ((cout << (i++ ? ", " : ""), dbg(a)), ...); }, t); cout << ")"; }
template<class T> constexpr bool dbg_printable = requires(const T& t) { cout << t; };
template<class T> constexpr bool dbg_iterable = requires(const T& t) { begin(t); end(t); };
inline int dbg_depth = 0; // indent level for nested multi-line containers
inline int dbg_rec = 0, dbg_max_rec = 1e9; // trace() call depth; trace_depth(k) traces only the top k levels
inline string dbg_pre() { string s; for (int d = 1; d < dbg_rec; d++) s += "\u2502 "; return s; } // trace indent prefix
template<class T> void dbg(const T& x) {
    if constexpr (dbg_printable<T>) { cout << x; }
    else {
        using E = remove_cvref_t<decltype(*begin(x))>;
        if constexpr (dbg_iterable<E> && !dbg_printable<E>) { // container of containers -> one row per line
            cout << "{\n"; dbg_depth++;
            for (auto&& row : x) { cout << dbg_pre() << string(2 * dbg_depth, ' '); dbg(row); cout << "\n"; }
            dbg_depth--; cout << dbg_pre() << string(2 * dbg_depth, ' ') << "}";
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
    (one(x), ...); (void)one; // trace() may pass zero args
}
#define debug(...) (dbg_on() ? (cout << dbg_pre(), dbg_all(#__VA_ARGS__, __VA_ARGS__), cout << endl, 0) : 0)
// recursion tracing: trace(args) on entry, tret(val) right before each return.
//   dfs(u = 1, d = 1)          <- trace
//   \u2502 dfs(u = 3, d = 2)        <- nested call, one level deeper
//   \u2502 \u2514 dfs(u = 3, d = 2) = 1  <- its tret
//   \u2514 dfs(u = 1, d = 1) = 2
inline bool dbg_on() { return dbg_rec <= dbg_max_rec; }
inline vector<string> dbg_calls; // entry text of every active trace(), so tret can name the call it closes
inline const char* dbg_fn(const char* f) { return strcmp(f, "operator()") ? f : "\u03bb"; } // lambdas have no name
template<class... T> string dbg_str(const char* names, const T&... x) { // dbg_all's output as a string
    ostringstream o; auto* old = cout.rdbuf(o.rdbuf()); dbg_all(names, x...); cout.rdbuf(old); return o.str();
}
struct dbg_frame {
    dbg_frame(string call) { dbg_rec++; dbg_calls.push_back(move(call)); if (dbg_on()) cout << dbg_pre() << dbg_calls.back() << endl; }
    ~dbg_frame() { dbg_rec--; dbg_calls.pop_back(); }
};
inline void dbg_ret() {}
template<class T> void dbg_ret(const T& v) { cout << " = "; dbg(v); }
#define DBG_CAT(a, b) a##b
#define DBG_GUARD(n) DBG_CAT(dbg_guard_, n) // unique name so a traced lambda inside a traced fn doesn't -Wshadow
#define trace(...) dbg_frame DBG_GUARD(__COUNTER__)(string(dbg_fn(__func__)) + "(" + dbg_str(#__VA_ARGS__ __VA_OPT__(,) __VA_ARGS__) + ")")
#define tret(...) (dbg_on() ? (cout << dbg_pre() << "\u2514 " << dbg_calls.back(), dbg_ret(__VA_ARGS__), cout << endl, 0) : 0)
#define trace_depth(k) dbg_max_rec = (k)
#else
#define debug(...)
#define trace(...)
#define tret(...)
#define trace_depth(k)
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
