#!/bin/zsh
# New-machine setup for toolbelt (macOS). Safe to re-run: every step skips what is already done.
#
#     git clone https://github.com/frank44/toolbelt.git ~/Documents/programming/toolbelt
#     ~/Documents/programming/toolbelt/setup/setup.sh
#
# Installs GCC + Python via Homebrew, sources setup/aliases.zsh from ~/.zshrc, installs the
# VS Code keybindings (Cmd+' run, Cmd+Enter compare) and the "cf" snippet, then lists the
# two things that need a human: the VS Code `code` command and the Competitive Companion extension.

set -e
TOOLBELT="${0:A:h:h}"
ZSHRC="$HOME/.zshrc"
VSCODE_USER="$HOME/Library/Application Support/Code/User"

step() { print -P "%B==> $1%b"; }
note() { print "    $1"; }

step "Homebrew packages (gcc, python)"
if ! command -v brew >/dev/null; then
    note "Homebrew is not installed. Install it from https://brew.sh and re-run this script."
    exit 1
fi
export HOMEBREW_NO_AUTO_UPDATE=1
# Check for the binaries, not formula names: python is installed as python@3.x, and checking
# for "python" would reinstall (and relink python3 to) a newer version on every machine.
if ls /opt/homebrew/bin/g++-* >/dev/null 2>&1; then
    note "gcc: already installed"
else
    brew install gcc
fi
if [[ -x /opt/homebrew/bin/python3 ]]; then
    note "python: already installed ($(/opt/homebrew/bin/python3 --version))"
else
    brew install python
fi
GXX="$(ls /opt/homebrew/bin/g++-* 2>/dev/null | sort -V | tail -1)"
note "compiler: ${GXX:-none found}"
if [[ -n "$GXX" && "$(basename "$GXX")" != "$(grep -o "CPP_COMPILER = '[^']*'" "$TOOLBELT/test_runner.py" | cut -d"'" -f2)" ]]; then
    note "test_runner.py's CPP_COMPILER names a different version; set it to $(basename "$GXX") at the top of that file."
fi

step "Shell aliases (~/.zshrc sources setup/aliases.zsh)"
LINE="source \"$TOOLBELT/setup/aliases.zsh\""
if grep -qF "$LINE" "$ZSHRC" 2>/dev/null; then
    note "already sourced"
else
    printf '\n# toolbelt: run / listen / cc / chk / g++23\n%s\n' "$LINE" >> "$ZSHRC"
    note "added to $ZSHRC (open a new terminal to pick it up)"
fi

step "VS Code keybindings (Cmd+' run, Cmd+Enter compare)"
mkdir -p "$VSCODE_USER/snippets"
if [[ ! -e "$VSCODE_USER/keybindings.json" ]]; then
    cp "$TOOLBELT/setup/keybindings.json" "$VSCODE_USER/keybindings.json"
    note "installed"
elif cmp -s "$TOOLBELT/setup/keybindings.json" "$VSCODE_USER/keybindings.json"; then
    note "already installed"
else
    note "a different keybindings.json exists; merge by hand from $TOOLBELT/setup/keybindings.json"
fi

step "VS Code 'cf' snippet"
/opt/homebrew/bin/python3 "$TOOLBELT/templates/gen_snippet.py"

step "Manual steps"
if command -v code >/dev/null; then
    note "VS Code 'code' command: found"
else
    note "VS Code 'code' command: missing. In VS Code run 'Shell Command: Install code command in PATH' (the listener uses it to open problems)."
fi
note "Install the Competitive Companion extension:"
note "  Chrome:  https://chromewebstore.google.com/detail/competitive-companion/cjnmckjndlpiamhfimnnjmnckgghkjbl"
note "  Firefox: https://addons.mozilla.org/en-US/firefox/addon/competitive-companion/"
note "Then: open a new terminal, run 'listen', click the extension on a problem page."
