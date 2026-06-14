#!/usr/bin/env python3
# Regenerates the VS Code "cf" snippet from base_template.cpp.
# Run this whenever you edit base_template.cpp to keep the snippet in sync:
#
#     python3 templates/gen_snippet.py
#
# Writes to ~/Library/Application Support/Code/User/snippets/cpp.json (macOS VS Code).
# Then type "cf" + Tab in any .cpp file to expand the template.

import json, os

HERE = os.path.dirname(os.path.abspath(__file__))
src = os.path.join(HERE, "base_template.cpp")
out = os.path.expanduser("~/Library/Application Support/Code/User/snippets/cpp.json")

with open(src) as f:
    lines = f.read().split("\n")
while lines and lines[-1] == "":
    lines.pop()

# In snippet bodies, '\' and '$' are special and must be escaped so they
# render literally. (JSON-level escaping of '"' and '\' is handled by json.dumps.)
def esc(line):
    return line.replace("\\", "\\\\").replace("$", "\\$")

body = [esc(l) for l in lines]

# Drop the final cursor ($0) into the empty solve() body.
for i, l in enumerate(body):
    if l.strip() == "void solve() {" and i + 1 < len(body) and body[i + 1].strip() == "":
        body[i + 1] = "    $0"
        break

snippet = {
    "competitive programming base template": {
        "prefix": "cf",
        "body": body,
        "description": "Base C++ template (debug macros + multitest main)",
    }
}

with open(out, "w") as f:
    f.write(json.dumps(snippet, indent=4) + "\n")
print("wrote:", out)
