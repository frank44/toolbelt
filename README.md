# 🧰 toolbelt

Personal competitive programming kit: a fast local test runner, a C++ contest template with a VS Code snippet, and a library of prewritten algorithms and data structures.

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
- Defines `LOCAL` to enable the template's `debug(...)` macro
- Times each case and cleans up binaries afterward

Also runs Java (`.java`). Compiler and flags are configured at the top of [test_runner.py](test_runner.py).

## 📝 Template & snippet

Type `cf` + Tab in any `.cpp` file to expand [base_template.cpp](templates/base_template.cpp): `i64`/`i128`, `ckmin`/`ckmax`, multitest `main`, and a `debug(...)` macro that pretty-prints pairs, tuples, nested containers, and even `__int128`.

After editing the template, regenerate the snippet (never edit `cpp.json` by hand):

```bash
python3 templates/gen_snippet.py
```

## 📚 Library

`algorithms/` and `data_structures/` are self-contained, copy-paste-ready reference implementations — paste into a solution and adapt on the spot.
