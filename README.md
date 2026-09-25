# 🧰 toolbelt

Personal competitive programming kit: a fast local C++ test runner, a contest template with a VS Code snippet, and a library of prewritten algorithms and data structures.

## 🗂️ Layout

```
test_runner.py            # compile + run a solution against sample cases
templates/
  base_template.cpp       # contest template (debug macros, multitest main)
  gen_snippet.py          # syncs the template into the VS Code "cf" snippet
algorithms/               # DP, graph theory, math
data_structures/          # Fenwick tree, sparse tables, trie, union-find, LCA, ...
```

## 🚀 Test runner

Put sample cases in `input.txt` (separate multiple cases with a `~~~` line), then:

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

Type `cf` + Tab in any `.cpp` file to expand [base_template.cpp](templates/base_template.cpp): `i64`/`i128`, `ckmin`/`ckmax`, multitest `main`, and the debug macros below.

### 🐛 `debug(...)`

Pretty-prints pairs, tuples, nested containers, and even `__int128`. String literals print as plain text, everything else as `name = value`:

```cpp
debug("phase 2", i, fixed);   // phase 2 i = 4, fixed = {0, 7}
```

### 🌳 `trace(...)` / `tret(...)` — recursion tracing

Call `trace(args)` on entry and `tret(val)` (or bare `tret()` for `void`) right before each `return`. Each call prints its entry as `fn(args)`, and its `tret` closes it with `└ fn(args) = value`, so returns are matched to their call by name, not just by indent. Any `debug(...)` inside the call is indented to match:

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
- `tret` is its own statement — it never wraps the return value. Call it on the line before the `return`.
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
