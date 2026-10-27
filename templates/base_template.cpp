// github.com/frank44/toolbelt — prewritten template
#include <bits/stdc++.h>
using namespace std;
using i64 = long long;
using i128 = __int128;

#ifdef LOCAL
template<class T> void dbg(const T& x);
inline void dbg(i128 x) { if (x < 0) { cout << '-'; x = -x; } string s; do { s += char('0' + int(x % 10)); x /= 10; } while (x); reverse(s.begin(), s.end()); cout << s; }
inline void dbg(const string& s) { cout << '"' << s << '"'; }
inline void dbg(char c) { if (isprint((unsigned char)c)) { cout << '\'' << c << '\''; } else { cout << int(c); } }
template<class A, class B> void dbg(const pair<A,B>& p) { cout << "("; dbg(p.first); cout << ", "; dbg(p.second); cout << ")"; }
template<class... T> void dbg(const tuple<T...>& t) { cout << "("; apply([](auto&&... a){ int i = 0; ((cout << (i++ ? ", " : ""), dbg(a)), ...); }, t); cout << ")"; }
template<class T> constexpr bool dbg_printable = requires(const T& t) { cout << t; };
template<class T> constexpr bool dbg_iterable = requires(const T& t) { begin(t); end(t); };
inline int dbg_depth = 0, dbg_rec = 0, dbg_max_rec = 1e9;
inline bool dbg_on() { return dbg_rec <= dbg_max_rec; }
inline string dbg_pre() { string s; for (int d = 1; d < dbg_rec; d++) s += "│ "; return s; }
template<class T> using dbg_elem = remove_cvref_t<decltype(*begin(declval<const T&>()))>;
template<class T> void dbg(const T& x) {
    if constexpr (dbg_printable<T>) { cout << x; }
    else if constexpr (dbg_iterable<dbg_elem<T>> && !dbg_printable<dbg_elem<T>>) {
        cout << "{\n"; dbg_depth++;
        for (auto&& row : x) { cout << dbg_pre() << string(2 * dbg_depth, ' '); dbg(row); cout << "\n"; }
        dbg_depth--; cout << dbg_pre() << string(2 * dbg_depth, ' ') << "}";
    } else { cout << "{"; int i = 0; for (auto&& e : x) { cout << (i++ ? ", " : ""); dbg(e); } cout << "}"; }
}
template<class T, class C> void dbg(stack<T,C> s) { cout << "{"; int i = 0; while (!s.empty()) { cout << (i++ ? ", " : ""); dbg(s.top()); s.pop(); } cout << "}"; }
template<class T, class C> void dbg(queue<T,C> q) { cout << "{"; int i = 0; while (!q.empty()) { cout << (i++ ? ", " : ""); dbg(q.front()); q.pop(); } cout << "}"; }
template<class T, class C, class P> void dbg(priority_queue<T,C,P> q) { cout << "{"; int i = 0; while (!q.empty()) { cout << (i++ ? ", " : ""); dbg(q.top()); q.pop(); } cout << "}"; }
inline vector<string> dbg_names(const char* s, bool angle) {
    vector<string> out; string cur; int depth = 0; bool quoted = false;
    for (const char* p = s; *p; p++) {
        if (*p == '"' && (p == s || p[-1] != '\\')) quoted = !quoted;
        if (!quoted && strchr(angle ? "([{<" : "([{", *p)) depth++;
        if (!quoted && strchr(angle ? ")]}>" : ")]}", *p)) depth--;
        if (!quoted && *p == ',' && depth == 0) { out.push_back(cur); cur.clear(); p += p[1] == ' '; } else cur += *p;
    }
    out.push_back(cur); return out;
}
template<class... T> void dbg_all(const char* names, const T&... x) {
    auto ns = dbg_names(names, true); if (ns.size() != sizeof...(T)) ns = dbg_names(names, false);
    int i = 0; bool prevText = false;
    auto one = [&](const auto& v) {
        constexpr bool text = is_convertible_v<remove_cvref_t<decltype(v)>, const char*>;
        if (i) cout << (text || prevText ? " " : ", ");
        if constexpr (text) cout << v; else { cout << ns[i] << " = "; dbg(v); }
        prevText = text; i++;
    };
    (one(x), ...); (void)one;
}
#define debug(...) (dbg_on() ? (cout << dbg_pre(), dbg_all(#__VA_ARGS__, __VA_ARGS__), cout << endl, 0) : 0)
inline vector<pair<string, bool>> dbg_calls;
inline const char* dbg_fn(const char* f) { return strcmp(f, "operator()") ? f : "λ"; }
template<class... T> string dbg_str(const char* names, const T&... x) { ostringstream o; auto* old = cout.rdbuf(o.rdbuf()); dbg_all(names, x...); cout.rdbuf(old); return o.str(); }
inline void dbg_close() { dbg_calls.back().second = true; cout << dbg_pre() << "└ " << dbg_calls.back().first; }
struct dbg_frame {
    dbg_frame(string call) { dbg_rec++; dbg_calls.emplace_back(move(call), false); if (dbg_on()) cout << dbg_pre() << dbg_calls.back().first << endl; }
    ~dbg_frame() { if (dbg_on() && !dbg_calls.back().second) { dbg_close(); cout << endl; } dbg_rec--; dbg_calls.pop_back(); }
};
inline void dbg_ret() {}
template<class T> void dbg_ret(const T& v) { cout << " = "; dbg(v); }
#define DBG_CAT(a, b) a##b
#define DBG_GUARD(n) DBG_CAT(dbg_guard_, n)
#define trace(...) dbg_frame DBG_GUARD(__COUNTER__)(string(dbg_fn(__func__)) + "(" + dbg_str(#__VA_ARGS__ __VA_OPT__(,) __VA_ARGS__) + ")")
#define tret(...) (dbg_on() ? (dbg_close(), dbg_ret(__VA_ARGS__), cout << endl, 0) : 0)
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
const i64 INF = 1e18;
const double PI = acos(-1.0);

void solve() {

}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t = 1;
    // cin >> t;
    while (t--) {
        solve();
    }
    return 0;
}
