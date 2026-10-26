# 🧰 toolbelt

Personal competitive programming kit: a fast local C++ test runner, a contest template with a VS Code snippet, and a library of prewritten algorithms and data structures.

## 🗂️ Layout

```
test_runner.py            # compile + run a solution against sample cases
cc_listen.py              # Competitive Companion listener: one click -> problem folder with samples
templates/
  base_template.cpp       # contest template (debug macros, single-test main)
  gen_snippet.py          # syncs the template into the VS Code "cf" snippet
algorithms/               # DP, graph theory, math
data_structures/          # Fenwick tree, sparse tables, trie, union-find, LCA, ...
```

## 📥 Fetching problems

Install the [Competitive Companion](https://github.com/jmerle/competitive-companion) browser extension, then leave this running during a contest:

```bash
python3 cc_listen.py
```

Click the extension icon on a problem page (or a contest page to grab every problem at once). Each problem becomes a folder under `~/cp/`:

```
~/cp/cf_round_987_div_2/A/sol.cpp     # base_template.cpp + a header comment: name, URL, time/memory limit
                          input.txt   # sample inputs, ~~~ separated
                          output.txt  # expected outputs, ~~~ separated
~/cp/ac_beginner_contest_380/C/...
~/cp/usaco_2024_december_contest_gold/1_cow_trip/...
```

Each new `sol.cpp` opens in VS Code. A folder that already exists is left alone, so hand-added cases survive a re-parse; delete it to fetch fresh samples. Port, root folder and the editor command are at the top of [cc_listen.py](cc_listen.py).

## 🚀 Test runner

Sample cases live in `input.txt` next to the solution (separate multiple cases with a `~~~` line); `cc_listen.py` writes them for you, or add your own. Then, from any directory:

```bash
python3 test_runner.py sol.cpp
```

- 🎯 **One case only**: `python3 test_runner.py sol.cpp 2`
- ✅ **Compare mode**: append `c` to diff against `output.txt`, mismatches highlighted:

```bash
python3 test_runner.py sol.cpp c
```

Under the hood 🔧:
- Compiles with sanitizers + `_GLIBCXX_DEBUG`, so out-of-bounds and UB **fail a test** instead of passing silently
- Caches a precompiled header for fast incremental compiles
- Defines `LOCAL` to enable the template's `debug(...)`/`trace(...)` macros — except in compare mode, which builds without it so the output diff sees exactly what the judge will
- Times each case and cleans up binaries afterward

Compiler and flags are configured at the top of [test_runner.py](test_runner.py).

## 📝 Template & snippet

Type `cf` + Tab in any `.cpp` file to expand [base_template.cpp](templates/base_template.cpp): `i64`/`i128`, `ckmin`/`ckmax`, single-test `main` (uncomment one line for multitest), and the debug macros below.

### 🐛 `debug(...)`

Pretty-prints pairs, tuples, nested containers, and even `__int128`. String literals print as plain text, everything else as `name = value`:

```cpp
debug("phase 2", i, fixed);   // phase 2 i = 4, fixed = {0, 7}
```

`string` values print quoted (`s = ""` vs `s = " "`) and `char`s as `'c'` — non-printable chars print as their int value, so `vector<char>` flags show as `{0, 1, 0}`.

### 🌳 `trace(...)` / `tret(...)` — recursion tracing

Call `trace(args)` on entry. Each call prints its entry as `fn(args)` and closes itself with `└ fn(args)` when it returns — by any path, so multiple `return`s need nothing extra. Add `tret(val)` right before a `return` when you also want the value shown (`└ fn(args) = value`). Any `debug(...)` inside the call is indented to match:

```cpp
int dfs(int u, int d) {
    trace(u, d);
    int sz = 1;
    for (int v : g[u]) {
        sz += dfs(v, d + 1);
        debug("after child", v, sz);
    }
    tret(sz);
    return sz;
}
```
```
dfs(u = 0, d = 0)
│ dfs(u = 1, d = 1)
│ │ dfs(u = 3, d = 2)
│ │ └ dfs(u = 3, d = 2) = 1
│ after child v = 3, sz = 2
│ └ dfs(u = 1, d = 1) = 2
after child v = 1, sz = 3
│ dfs(u = 2, d = 1)
│ └ dfs(u = 2, d = 1) = 1
after child v = 2, sz = 4
└ dfs(u = 0, d = 0) = 4
```

Rules:
- `tret` is optional and is its own statement — it never wraps the return value. Call it on the line before the `return`. Skip it for `void`, pointer returns, or functions with many return sites; the close line still prints.
- `trace_depth(k);` traces only the top `k` call levels (for `fib(40)`-sized trees).
- Lambdas print as `λ` (they have no `__func__` name). Recursive lambdas work fine. `trace()` with no args prints `fn()`.
- Leave the calls in when you submit: without `LOCAL`, `debug`/`trace`/`tret`/`trace_depth` compile to nothing.
- On LeetCode there's no `-DLOCAL`: paste the whole `#ifdef LOCAL … #endif` block and add `#define LOCAL` above it to debug; delete that line before submitting.

After editing the template, regenerate the snippet (never edit `cpp.json` by hand):

```bash
python3 templates/gen_snippet.py
```

## 📚 Library

`algorithms/` and `data_structures/` are self-contained, copy-paste-ready reference implementations — paste into a solution and adapt on the spot.
