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

# new calico A B C -> same folders as a listener click, for judges Competitive Companion can't parse
# grab A [n]       -> next two copies (sample input, then output) become a case in A/; n samples = 2n copies
alias new="$PY3 \"$TOOLBELT/cc_manual.py\" new"
alias grab="$PY3 \"$TOOLBELT/cc_manual.py\" grab"

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

# judge A.cpp      -> compile-check as the judge will: JUDGE_STD, no LOCAL, and C++20/23 extensions
#                     are errors. CALICO is g++ 13 / C++17; uses g++-13 when installed, else newest GCC.
JUDGE_STD=c++17
JUDGE_CXX=/opt/homebrew/bin/g++-13
[ -x "$JUDGE_CXX" ] || JUDGE_CXX="$CXX"
judge() {
    [ -z "$1" ] && { echo "usage: judge <source.cpp>"; return 1; }
    "$JUDGE_CXX" -std=$JUDGE_STD -Wall -Wextra -Werror=c++20-extensions -Werror=c++23-extensions \
        -fsyntax-only "$1" && echo "ok ($JUDGE_STD, ${JUDGE_CXX:t}): $1"
}
