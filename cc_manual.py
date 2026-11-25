#!/usr/bin/env python3
# Manual stand-in for a Competitive Companion click, for judges the extension has no parser for
# (CALICO, one-off university judges, PDF statements). Builds the same folders as cc_listen.py;
# the samples come off the clipboard instead of from the extension.
#
#     new calico_fall_26 A B C     scaffold ~/Documents/programming/contest/calico_fall_26/{A,B,C}/ and open them
#                                  in VS Code. {A..H} works too. Existing folders are left alone.
#     grab A                       then copy the sample input, copy the sample output: appended as a
#                                  new case to A/input.txt and A/output.txt
#     grab A 3                     three samples (six copies)
#     grab                         one sample into the current folder
#
# grab watches the clipboard, so the terminal never needs focus: stay on the statement and Cmd+C.
# It exits after its last copy, so a later Cmd+C (your solution, on the way to the submit box) is
# never taken for a sample. Ctrl-C drops a half-copied pair; finished pairs are already on disk.
# macOS only (NSPasteboard, pbpaste).

import ctypes
import ctypes.util
import os
import subprocess
import sys
import time

import cc_listen as cc


def new(contest, problems):
    cdir = os.path.join(cc.ROOT, cc.slug(contest))
    cc.activate(cdir)
    cc.open_in_editor(cdir, [cc.make_problem(cdir, p) for p in problems])


def clipboard():
    """Returns wait(): blocks until the next copy, returns its text. Watches NSPasteboard's change
    counter instead of the text, so an output identical to its input still counts as a second copy."""
    objc = ctypes.CDLL(ctypes.util.find_library('objc'))
    ctypes.CDLL(ctypes.util.find_library('AppKit'))
    objc.objc_getClass.restype = objc.sel_registerName.restype = ctypes.c_void_p
    objc.objc_getClass.argtypes = objc.sel_registerName.argtypes = [ctypes.c_char_p]
    send = objc.objc_msgSend
    send.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
    send.restype = ctypes.c_void_p
    board = send(objc.objc_getClass(b'NSPasteboard'), objc.sel_registerName(b'generalPasteboard'))
    send.restype = ctypes.c_long
    count = lambda: send(board, objc.sel_registerName(b'changeCount'))
    seen = count()

    def wait():
        nonlocal seen
        while True:
            while count() == seen:
                time.sleep(0.1)
            time.sleep(0.05)  # some apps clear the board, then fill it: read after the second write
            seen = count()
            text = subprocess.run(['pbpaste'], capture_output=True, encoding='utf-8', errors='replace',
                                  env=dict(os.environ, LC_CTYPE='UTF-8')).stdout  # pbpaste is MacRoman without a UTF-8 locale
            text = text.replace('\r\n', '\n').replace('\u00a0', ' ').strip()  # PDFs and HTML tables copy NBSPs, which break cin
            if text:  # an image or an empty copy is not a sample
                return text

    return wait


def add_case(path, text):
    old = open(path).read().strip() if os.path.exists(path) else ''
    with open(path, 'w') as f:
        f.write((old + '\n~~~\n' if old else '') + text + '\n')
    return len(old.split('~~~')) + 1 if old else 1


def grab(pdir, n):
    wait = clipboard()
    name = os.path.basename(os.path.abspath(pdir))
    show = lambda t: f"{t.splitlines()[0][:40]!r}" + (f" (+{t.count(chr(10))} more)" if chr(10) in t else '')
    for _ in range(n):
        print(f"{name}: copy the sample input ... ", end='', flush=True)
        inp = wait()
        print(show(inp))
        print(f"{name}: copy the sample output ... ", end='', flush=True)
        out = wait()
        print(show(out))
        total = add_case(os.path.join(pdir, 'input.txt'), inp)
        add_case(os.path.join(pdir, 'output.txt'), out)
        print(f"{name}: case {total} added")


if __name__ == '__main__':
    cmd, args = (sys.argv[1:2] or [''])[0], sys.argv[2:]
    if cmd == 'new' and len(args) >= 2:
        new(args[0], args[1:])
    elif cmd == 'grab' and len(args) <= 2:
        # A trailing number is the sample count, unless a folder has that name (URL-id problem folders).
        n = int(args.pop()) if args and args[-1].isdigit() and not os.path.isdir(args[-1]) else 1
        pdir = args[0] if args else '.'
        if len(args) > 1 or not os.path.isdir(pdir):
            sys.exit(f"no such problem folder: {' '.join(args)}")
        try:
            grab(pdir, n)
        except KeyboardInterrupt:
            print()
    else:
        sys.exit("usage: new <contest> <problem>...\n       grab [problem folder] [samples]")
