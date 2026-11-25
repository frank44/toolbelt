#!/usr/bin/env python3
# Competitive Companion listener: turns a browser click on a problem page into a
# ready-to-run problem folder.
#
#     python3 cc_listen.py            # leave running during the contest, Ctrl-C to stop
#
# Install the Competitive Companion extension (Chrome/Firefox), open a problem or
# contest page, click its icon. For every problem it sends, this creates
#
#     ~/Documents/programming/contest/<contest>/<problem>/<problem>.cpp   base_template.cpp + a header comment (name, URL, limits)
#                                              input.txt       sample inputs,  ~~~ separated (test_runner.py format)
#                                              output.txt      expected outputs, ~~~ separated
#
# e.g. ~/Documents/programming/contest/cf_round_987_div_2/A/A.cpp, ~/Documents/programming/contest/ac_beginner_contest_380/C/C.cpp,
#      ~/Documents/programming/contest/usaco_2024_december_contest_gold/1_cow_trip/1_cow_trip.cpp
#
# Every click points the current VS Code window at that problem's contest folder (one window per
# contest, switching contests switches the folder) and opens the solution as a tab. A new contest
# folder gets a .vscode/tasks.json: the default test task runs the active file in compare mode, the default
# build task runs it with debug output.
#
# Only one contest is active at a time. A click on a different contest moves the previous one to
# ~/Documents/programming/contest/archive/ and, if the clicked one is already in the archive (upsolving), moves it
# back instead of creating it again. The symlink ~/Documents/programming/contest/_current always points at the
# active contest: submit from _current/<problem>/<problem>.cpp.
#
# A folder that already exists is left alone (hand-added cases survive a re-parse);
# delete it to fetch fresh samples. Then: run ~/Documents/programming/contest/<contest>/<problem>/<problem>.cpp c
#
# A judge the extension has no parser for never reaches this listener (the extension fails in the
# browser). cc_manual.py builds the same folders by hand: `new` scaffolds, `grab` reads samples off the clipboard.

import json
import os
import re
import shutil
import signal
import subprocess
import sys
import time
from http.server import BaseHTTPRequestHandler, HTTPServer

# ===== config (edit here) =====
ROOT = os.path.expanduser(os.environ.get('CONTESTS_ROOT', '~/Documents/programming/contest'))
PORT = 27121                        # in Competitive Companion's default port list; no extension config needed
OPEN_CMD = ['code', '-r']           # opens new contest folders and solutions in the current VS Code window; [] to disable
CURRENT = '_current'                # symlink in ROOT to the contest of the last click; the underscore sorts it first
ARCHIVE = 'archive'                 # folder in ROOT holding every contest but the current one
JUDGE_SHORT = {'codeforces': 'cf', 'atcoder': 'ac', 'codechef': 'cc', 'usaco': 'usaco', 'leetcode': 'lc'}

HERE = os.path.dirname(os.path.abspath(__file__))
TEMPLATE = os.path.join(HERE, 'templates', 'base_template.cpp')
RUNNER = os.path.join(HERE, 'test_runner.py')
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


def problem_dir(name, has_contest, url):
    """'A. Two Sum' / 'A1. Easy' / 'Ex - Hard' -> 'A' / 'A1' / 'Ex'; 'Problem 2. Cow Trip' -> '2_cow_trip'; else slug(name).
    A title with no ASCII letters (Cyrillic Eolymp problems) slugs to nothing: use the problem id from the URL instead."""
    m = re.match(r'^([A-Za-z]{1,2}\d?)\s*[.\-–]\s', name)
    if m and has_contest:
        return m.group(1)
    m = re.match(r'^Problem (\d+)\.\s*(.*)', name)
    if m:
        return m.group(1) + '_' + slug(m.group(2))
    ids = re.findall(r'\d+', url)
    return slug(name) or (ids[-1] if ids else 'problem')


def write_tasks(cdir):
    """VS Code tasks for the contest folder: test = compare mode, build = debug run, both on the active file."""
    run = f'python3 "{RUNNER}" "${{file}}"'  # plain python3: a versioned interpreter path breaks on the next upgrade
    task = lambda label, cmd, kind: {'label': label, 'type': 'shell', 'command': cmd, 'problemMatcher': [],
                                     'group': {'kind': kind, 'isDefault': True},
                                     'presentation': {'clear': True, 'reveal': 'always', 'focus': False}}
    tasks = {'version': '2.0.0', 'tasks': [task('test samples', run + ' c', 'test'), task('run with debug', run, 'build')]}
    os.makedirs(os.path.join(cdir, '.vscode'), exist_ok=True)
    with open(os.path.join(cdir, '.vscode', 'tasks.json'), 'w') as f:
        json.dump(tasks, f, indent=4)


def make_problem(cdir, pname, header='', tests=(), info=''):
    """Create <cdir>/<pname>/ with <pname>.cpp, input.txt and output.txt. Returns the solution path.
    Shared with cc_manual.py, which calls it with no header and no tests."""
    pdir = os.path.join(cdir, pname)
    sol = os.path.join(pdir, pname + '.cpp')
    if os.path.exists(pdir):
        print(f"  exists, skipped: {pdir}")
        return sol
    new_contest = not os.path.exists(cdir)
    os.makedirs(pdir)
    if new_contest:
        write_tasks(cdir)
    sep = '\n~~~\n'
    with open(os.path.join(pdir, 'input.txt'), 'w') as f:
        f.write(sep.join(t.get('input', '').strip() for t in tests) + '\n')
    with open(os.path.join(pdir, 'output.txt'), 'w') as f:
        f.write(sep.join(t.get('output', '').strip() for t in tests) + '\n')
    with open(TEMPLATE) as f:
        body = f.read()
    with open(sol, 'w') as f:
        f.write(header + body)
    print(f"  {pdir}" + (f"  ({info})" if info else ''))
    return sol


def write_problem(p):
    """Create the problem folder for a Competitive Companion payload. Returns (solution path, contest dir)."""
    group, name = p.get('group', ''), p.get('name', '')
    has_contest = ' - ' in group
    pname = problem_dir(name, has_contest, p.get('url', ''))
    cdir = os.path.join(ROOT, contest_dir(group))
    activate(cdir)
    tests = p.get('tests', [])
    limits = f"TL {p.get('timeLimit', 0) / 1000:g}s  ML {p.get('memoryLimit', 0)}MB"
    header = f"// {name}  {p.get('url', '')}  {limits}" + ("  INTERACTIVE" if p.get('interactive') else '') + '\n'
    return make_problem(cdir, pname, header, tests, f"{len(tests)} tests, {limits}"), cdir


def activate(cdir):
    """Make cdir the one active contest in ROOT. Call before creating anything in it.
    A contest sitting in the archive comes back instead of being created again (upsolving); every other
    contest folder goes to the archive; ROOT/_current is repointed, so the submit dialog always starts
    from the same place. Only folders with a .vscode/tasks.json (written by write_tasks) are archived:
    hand-made folders and the toolbelt itself stay put."""
    name = os.path.basename(cdir)
    adir = os.path.join(ROOT, ARCHIVE)
    os.makedirs(ROOT, exist_ok=True)
    if not os.path.exists(cdir) and os.path.isdir(os.path.join(adir, name)):
        shutil.move(os.path.join(adir, name), cdir)
        print(f"  restored from {ARCHIVE}/: {name}")
    for d in sorted(os.listdir(ROOT)):
        path = os.path.join(ROOT, d)
        if d in (name, ARCHIVE) or os.path.islink(path) or os.path.samefile(path, HERE):
            continue
        if not os.path.isfile(os.path.join(path, '.vscode', 'tasks.json')):
            continue
        if os.path.exists(os.path.join(adir, d)):
            print(f"  not archived, {ARCHIVE}/{d} already exists: {d}")
            continue
        os.makedirs(adir, exist_ok=True)
        shutil.move(path, os.path.join(adir, d))
        print(f"  archived: {d}")
    link = os.path.join(ROOT, CURRENT)  # relative link: survives moving ROOT; a non-symlink at that path is left alone
    if os.path.islink(link):
        os.remove(link)
    if not os.path.lexists(link):
        os.symlink(name, link)


def open_in_editor(cdir, sols):
    """The contest folder becomes the window's folder (a no-op if it already is); the files open as tabs."""
    if OPEN_CMD and shutil.which(OPEN_CMD[0]):
        subprocess.run(OPEN_CMD + [cdir] + sols)


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
        sol, cdir = write_problem(p)
        open_in_editor(cdir, [sol])


def kill_old_listeners():
    """Kill earlier runs of this script that still hold the port (a forgotten terminal, a Ctrl-Z'd one).
    SIGKILL because a suspended process never gets to handle SIGTERM. Any other program on the port is left alone."""
    pids = subprocess.run(['lsof', '-t', '-nP', f'-iTCP:{PORT}', '-sTCP:LISTEN'], capture_output=True, text=True).stdout.split()
    for pid in map(int, pids):
        cmd = subprocess.run(['ps', '-o', 'command=', '-p', str(pid)], capture_output=True, text=True).stdout
        if pid == os.getpid() or os.path.basename(__file__) not in cmd:
            continue
        try:
            os.kill(pid, signal.SIGKILL)
        except ProcessLookupError:
            continue
        print(f"Killed the previous listener (pid {pid}).")
    for _ in range(20):  # the port frees a moment after the kill
        if not pids or not subprocess.run(['lsof', '-t', '-nP', f'-iTCP:{PORT}', '-sTCP:LISTEN'], capture_output=True).stdout:
            break
        time.sleep(0.05)


if __name__ == '__main__':
    if not os.path.exists(TEMPLATE):
        sys.exit(f"template not found: {TEMPLATE}")
    os.makedirs(ROOT, exist_ok=True)
    sys.stdout.reconfigure(line_buffering=True)  # lines show up at once even when piped to a file
    kill_old_listeners()
    try:
        server = HTTPServer(('127.0.0.1', PORT), Handler)
    except OSError as e:
        sys.exit(f"Can't listen on port {PORT}: {e.strerror}.\n"
                 f"Something that is not this listener holds it:  lsof -nP -iTCP:{PORT} -sTCP:LISTEN")
    print(f"Listening on 127.0.0.1:{PORT}, writing to {ROOT}  (Ctrl-C to stop)")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print()
