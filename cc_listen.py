#!/usr/bin/env python3
# Competitive Companion listener: turns a browser click on a problem page into a
# ready-to-run problem folder.
#
#     python3 cc_listen.py            # leave running during the contest, Ctrl-C to stop
#
# Install the Competitive Companion extension (Chrome/Firefox), open a problem or
# contest page, click its icon. For every problem it sends, this creates
#
#     ~/cp/<contest>/<problem>/sol.cpp      base_template.cpp + a header comment (name, URL, limits)
#                             input.txt    sample inputs,  ~~~ separated (test_runner.py format)
#                             output.txt   expected outputs, ~~~ separated
#
# e.g. ~/cp/cf_round_987_div_2/A/, ~/cp/ac_beginner_contest_380/C/,
#      ~/cp/usaco_2024_december_contest_gold/1_cow_trip/
#
# A folder that already exists is left alone (hand-added cases survive a re-parse);
# delete it to fetch fresh samples. Then: run ~/cp/<contest>/<problem>/sol.cpp c

import json
import os
import re
import shutil
import subprocess
import sys
from http.server import BaseHTTPRequestHandler, HTTPServer

# ===== config (edit here) =====
CP_ROOT = os.path.expanduser(os.environ.get('CP_ROOT', '~/cp'))
PORT = 27121                        # in Competitive Companion's default port list; no extension config needed
OPEN_CMD = ['code', '-r']           # opens each new sol.cpp in the current VS Code window; [] to disable
JUDGE_SHORT = {'codeforces': 'cf', 'atcoder': 'ac', 'codechef': 'cc', 'usaco': 'usaco', 'leetcode': 'lc'}

HERE = os.path.dirname(os.path.abspath(__file__))
TEMPLATE = os.path.join(HERE, 'templates', 'base_template.cpp')
MAX_BODY = 4 * 1024 * 1024


def slug(s, limit=60):
    s = re.sub(r'[^a-z0-9]+', '_', s.lower()).strip('_')
    return s[:limit].rstrip('_')


def contest_dir(group):
    """'Codeforces - Codeforces Round 987 (Div. 2)' -> 'cf_round_987_div_2'; 'AtCoder' alone -> 'ac'."""
    judge, _, category = group.partition(' - ')
    short = JUDGE_SHORT.get(judge.strip().lower(), slug(judge))
    category = re.sub(re.escape(judge.strip()) + r'\s*', '', category, count=1, flags=re.I)  # 'AtCoder Beginner Contest 380' -> 'Beginner Contest 380'
    return short + ('_' + slug(category) if slug(category) else '')


def problem_dir(name, has_contest):
    """'A. Two Sum' / 'A1. Easy' / 'Ex - Hard' -> 'A' / 'A1' / 'Ex'; 'Problem 2. Cow Trip' -> '2_cow_trip'; else slug(name)."""
    m = re.match(r'^([A-Za-z]{1,2}\d?)\s*[.\-–]\s', name)
    if m and has_contest:
        return m.group(1)
    m = re.match(r'^Problem (\d+)\.\s*(.*)', name)
    if m:
        return m.group(1) + '_' + slug(m.group(2))
    return slug(name) or 'problem'


def write_problem(p):
    group, name = p.get('group', ''), p.get('name', '')
    has_contest = ' - ' in group
    pdir = os.path.join(CP_ROOT, contest_dir(group), problem_dir(name, has_contest))
    sol = os.path.join(pdir, 'sol.cpp')
    if os.path.exists(pdir):
        print(f"  exists, skipped: {pdir}")
        return sol
    os.makedirs(pdir)
    tests = p.get('tests', [])
    sep = '\n~~~\n'
    with open(os.path.join(pdir, 'input.txt'), 'w') as f:
        f.write(sep.join(t.get('input', '').strip() for t in tests) + '\n')
    with open(os.path.join(pdir, 'output.txt'), 'w') as f:
        f.write(sep.join(t.get('output', '').strip() for t in tests) + '\n')
    limits = f"TL {p.get('timeLimit', 0) / 1000:g}s  ML {p.get('memoryLimit', 0)}MB"
    header = f"// {name}  {p.get('url', '')}  {limits}" + ("  INTERACTIVE" if p.get('interactive') else '') + '\n'
    with open(TEMPLATE) as f:
        body = f.read()
    with open(sol, 'w') as f:
        f.write(header + body)
    print(f"  {pdir}  ({len(tests)} tests, {limits})")
    return sol


class Handler(BaseHTTPRequestHandler):
    def log_message(self, *args):
        pass  # quiet; we print our own lines

    def do_POST(self):
        # The extension posts from its background page (no web Origin). A request carrying
        # an http(s) Origin is a web page poking at localhost: refuse it.
        if re.match(r'^https?://', self.headers.get('Origin', '')):
            self.send_response(403); self.end_headers()
            return
        n = int(self.headers.get('Content-Length', 0))
        if n > MAX_BODY:
            self.send_response(413); self.end_headers()
            return
        try:
            p = json.loads(self.rfile.read(n))
            assert isinstance(p, dict) and p.get('name')
        except Exception:
            self.send_response(400); self.end_headers()
            return
        self.send_response(200); self.end_headers()
        print(f"{p['name']}  [{p.get('group', '')}]")
        sol = write_problem(p)
        if OPEN_CMD and shutil.which(OPEN_CMD[0]):
            subprocess.run(OPEN_CMD + [sol])


if __name__ == '__main__':
    if not os.path.exists(TEMPLATE):
        sys.exit(f"template not found: {TEMPLATE}")
    os.makedirs(CP_ROOT, exist_ok=True)
    sys.stdout.reconfigure(line_buffering=True)  # lines show up at once even when piped to a file
    print(f"Listening on 127.0.0.1:{PORT}, writing to {CP_ROOT}  (Ctrl-C to stop)")
    try:
        HTTPServer(('127.0.0.1', PORT), Handler).serve_forever()
    except KeyboardInterrupt:
        print()
