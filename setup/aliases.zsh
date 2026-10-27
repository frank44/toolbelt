# Toolbelt shell aliases. Source this from ~/.zshrc (setup.sh adds the line):
#
#     source "$HOME/Documents/programming/toolbelt/setup/aliases.zsh"
#
# Paths are derived from this file's location, so the clone can live anywhere.

TOOLBELT="${${(%):-%x}:A:h:h}"
PY3=/opt/homebrew/bin/python3
# Newest Homebrew GCC (g++-16, g++-17, ...). Apple's clang is deliberately not used:
# judges run real GCC, and the template relies on GCC-only bits (__int128, PBDS, bits/stdc++.h).
CXX="$(ls /opt/homebrew/bin/g++-* 2>/dev/null | sort -V | tail -1)"
CXXFLAGS=(-std=c++23 -O2 -Wall -Wextra)

# run A.cpp        -> all sample cases (debug build: sanitizers + debug()/trace() on)
# run A.cpp 3      -> only case 3
# run A.cpp c      -> all cases, compare against output.txt (judge-like build, no debug output)
alias run="$PY3 \"$TOOLBELT/test_runner.py\""

# listen           -> Competitive Companion listener; one problem folder per click. Ctrl-C to stop.
alias listen="$PY3 \"$TOOLBELT/cc_listen.py\""

# g++23 x.cpp      -> plain judge-like compile
alias g++23="$CXX ${CXXFLAGS[*]}"

# cc A.cpp         -> compiles to ./A   (binary named after the source)
# cc A.cpp foo     -> compiles to ./foo
cc() {
    local src="$1"
    [ -z "$src" ] && { echo "usage: cc <source.cpp> [output]"; return 1; }
    local out="${2:-${src%.*}}"
    "$CXX" "${CXXFLAGS[@]}" "$src" -o "$out"
}

# chk snippet.cpp  -> syntax-check only, no main/linking needed (for library files)
chk() {
    [ -z "$1" ] && { echo "usage: chk <source.cpp>"; return 1; }
    "$CXX" "${CXXFLAGS[@]}" -fsyntax-only "$1" && echo "ok: $1"
}
