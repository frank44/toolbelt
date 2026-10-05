import subprocess
import os
import sys
import time
import glob
import re
import shutil
import hashlib

# Kill a case that runs longer than this (infinite loop / runaway recursion) and report TLE.
# The sanitizer build is ~2-3x slower than a judge, so this is deliberately loose.
TIME_LIMIT_S = 10

# ===== C++ compiler config (edit here) =====
CPP_COMPILER = 'g++-16'
CPP_FLAGS = [
    '-std=c++23', '-O2', '-g',
    '-Wall', '-Wextra', '-Wshadow',
    '-Wduplicated-cond', '-Wduplicated-branches', '-Wlogical-op',
    '-Wfloat-equal',
    '-D_GLIBCXX_ASSERTIONS',
    '-Winvalid-pch',
    '-fdiagnostics-color=always',    # runner captures output via pipe; keep colors
    '-fmax-errors=3',
    # --- debug/sanitizer flags (local only; never ship to CF) ---
    '-fsanitize=address,undefined',  # catch OOB/UAF + UB (signed overflow, etc.)
    '-fsanitize=float-divide-by-zero,float-cast-overflow',
    '-fno-sanitize-recover=all',     # abort + nonzero exit on first error, so a test FAILS
    '-fno-omit-frame-pointer',       # readable stack traces in sanitizer reports
    '-D_GLIBCXX_DEBUG',              # bounds-check vector::operator[], catch bad iterators
    '-fno-inline',                   # one stack frame per call, so crash traces point at the exact line
]

# Enables the template's debug()/trace(). Compare mode is the pre-submit check, so it
# builds WITHOUT this: debug output vanishes exactly as on the judge and stdout matches
# output.txt. Kept out of CPP_FLAGS so the PCH (which never reads LOCAL) is valid for both.
CPP_LOCAL_FLAGS = ['-DLOCAL']

# Link-only flags: must NOT be passed to the PCH build (-Wl forces a link step there)
CPP_LINK_FLAGS = [
    '-Wl,-stack_size,0x20000000',    # 512MB stack (macOS): match CF's deep-recursion headroom
]

# Runtime sanitizer options, so every crash comes with a symbolized stack trace:
#   handle_abort=1     -> _GLIBCXX_DEBUG failures and assert() call abort(); have ASan trace that too
#   dump_registers=0   -> drop the register dump noise from those reports
#   print_stacktrace=1 -> UBSan reports (signed overflow, etc.) get a full trace, not just one line
SANITIZER_ENV = {
    'ASAN_OPTIONS': 'handle_abort=1:dump_registers=0',
    'UBSAN_OPTIONS': 'print_stacktrace=1',
}

# ===== Precompiled-header cache (auto-managed, full <bits/stdc++.h>) =====
PCH_DIR = os.path.expanduser('~/cp/pch')
LITE_PCH = True   # CP-focused header: drops <regex>/<locale> for speed, keeps <print>
LITE_HEADER = '''#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <array>
#include <string>
#include <string_view>
#include <algorithm>
#include <numeric>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <deque>
#include <list>
#include <tuple>
#include <utility>
#include <bitset>
#include <functional>
#include <optional>
#include <variant>
#include <complex>
#include <cmath>
#include <climits>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <random>
#include <chrono>
#include <type_traits>
#include <stdexcept>
#include <print>
'''

def ensure_pch(compiler, flags):
    """Build/refresh a PCH for the real <bits/stdc++.h>. Returns ['-I', dir] or [] on fallback."""
    bits_dir = os.path.join(PCH_DIR, 'bits')
    hdr = os.path.join(bits_dir, 'stdc++.h')
    gch = os.path.join(bits_dir, 'stdc++.h.gch')
    sigfile = os.path.join(bits_dir, '.sig')
    try:
        ver = subprocess.run([compiler, '--version'], capture_output=True, text=True).stdout.splitlines()[0]
    except Exception:
        ver = compiler
    # Include the macOS deployment target in the cache key: it comes from the
    # environment (MACOSX_DEPLOYMENT_TARGET), not from `flags`, and if it drifts
    # between when the PCH is built and when it's used, GCC silently rejects the
    # PCH (-Winvalid-pch) and recompiles every header from scratch (~5s/run).
    dep_target = os.environ.get('MACOSX_DEPLOYMENT_TARGET', '')
    sig = hashlib.md5((ver + ' ' + ' '.join(flags) + ' lite=' + str(LITE_PCH) + ' dep=' + dep_target).encode()).hexdigest()
    cur = open(sigfile).read().strip() if os.path.exists(sigfile) else None
    if os.path.exists(gch) and cur == sig:
        return ['-I', PCH_DIR]
    os.makedirs(bits_dir, exist_ok=True)
    probe = subprocess.run([compiler, '-std=c++23', '-x', 'c++', '-E', '-H', '-'],
                           input='#include <bits/stdc++.h>\n', capture_output=True, text=True)
    sys_hdr = None
    for line in probe.stderr.splitlines():
        s = line.strip()
        if s.startswith('. ') and s.endswith('stdc++.h'):
            sys_hdr = s[2:].strip(); break
    if LITE_PCH:
        with open(hdr, 'w') as f:
            f.write(LITE_HEADER)
    else:
        if not sys_hdr or not os.path.exists(sys_hdr):
            return []
        shutil.copyfile(sys_hdr, hdr)
    print("Building precompiled header (one-time, ~several seconds)...")
    r = subprocess.run([compiler] + flags + ['-x', 'c++-header', hdr, '-o', gch],
                       capture_output=True, text=True)
    if r.returncode != 0:
        print("PCH build failed; continuing without it.\n" + r.stderr)
        return []
    with open(sigfile, 'w') as f:
        f.write(sig)
    return ['-I', PCH_DIR]

# ANSI escape codes
RED = '\033[91m'
GREEN = '\033[92m'
RESET = '\033[0m'
BOLD = '\033[1m'


def highlight_differences(expected_output, actual_output):
    expected_lines = expected_output.strip().split('\n')
    actual_lines = actual_output.strip().split('\n')
    max_lines = max(len(expected_lines), len(actual_lines))
    highlighted_output = ""

    for i in range(max_lines):
        expected_line = expected_lines[i] if i < len(expected_lines) else ""
        actual_line = actual_lines[i] if i < len(actual_lines) else ""
        trimmed_expected_line = expected_line.rstrip()
        trimmed_actual_line = actual_line.rstrip()

        if trimmed_expected_line != trimmed_actual_line:
            max_width = max(len(trimmed_expected_line), len(trimmed_actual_line))
            padded_actual = actual_line.ljust(max_width)
            highlighted_output += f">   {padded_actual}     (Expected: {trimmed_expected_line})\n"
        else:
            highlighted_output += f"    {actual_line}\n"

    return highlighted_output


# A sanitizer stack frame; symbolized ones end in file:line, e.g.
#   "    #3 0x0001022227e8 in get(std::vector<int>&, int) sol.cpp:5"
FRAME_RE = re.compile(r'^\s*#\d+ 0x[0-9a-f]+ ')
SRC_FRAME_RE = re.compile(r'^\s*#\d+ 0x[0-9a-f]+ in (.+) (\S+):(\d+)(?::\d+)?$')


def short_func(name):
    """'int solve()::'lambda'(auto&&, int)::operator()<...>(...)' -> 'solve lambda'; 'get(int)' -> 'get'."""
    head, is_lambda, _ = name.partition("::'lambda'")
    words = head.split('(')[0].split()
    return (words[-1] if words else '?') + (' lambda' if is_lambda else '')


def crash_site(report, src_file):
    """Boil a sanitizer stack trace down to the frames in src_file (innermost first), with the source line."""
    src_name = os.path.basename(src_file)
    with open(src_file, 'r', errors='replace') as file:
        src_lines = file.read().split('\n')
    frames = []  # [line number, function, repeat count]
    for line in report.split('\n'):
        if not FRAME_RE.match(line):
            if frames:
                break  # only the first trace: later ones are where memory was allocated/freed
            continue
        m = SRC_FRAME_RE.match(line)
        if not m or os.path.basename(m.group(2)) != src_name:
            continue  # libstdc++ / libc frame
        line_no, func = int(m.group(3)), short_func(m.group(1))
        if frames and frames[-1][:2] == [line_no, func]:
            frames[-1][2] += 1  # collapse recursion
        else:
            frames.append([line_no, func, 1])
    if not frames:
        return ""
    rows = []
    for line_no, func, count in frames:
        code = src_lines[line_no - 1].strip() if 0 < line_no <= len(src_lines) else ''
        rows.append((f"{src_name}:{line_no}", func + (f" x{count}" if count > 1 else ''), code))
    loc_width = max(len(loc) for loc, _, _ in rows)
    func_width = max(len(func) for _, func, _ in rows)
    out = f"{RED}{BOLD}Crashed at {rows[0][0]}{RESET}  (innermost call first)\n"
    for loc, func, code in rows:
        out += f"  {BOLD}{loc.ljust(loc_width)}{RESET}  {func.ljust(func_width)}  {code}\n"
    return out


# ===== Per-language strategy =====
# Each builder returns (compile_cmd_or_None, run_cmd, cleanup_fn)

def cpp_strategy(src, compare=False):
    binary = os.path.splitext(os.path.basename(src))[0]
    pch_inc = ensure_pch(CPP_COMPILER, CPP_FLAGS)
    flags = CPP_FLAGS + ([] if compare else CPP_LOCAL_FLAGS)
    compile_cmd = [CPP_COMPILER] + flags + CPP_LINK_FLAGS + pch_inc + [src, '-o', binary, '-lstdc++exp']
    run_cmd = [f'./{binary}']

    def cleanup():
        # The compiled binary, plus a.out from any stray default-output compile.
        for f in [binary, 'a.out']:
            if os.path.exists(f):
                os.remove(f)
        # Debug-symbol bundles are directories (e.g. dp_1639.dSYM, a.out.dSYM).
        for d in glob.glob('*.dSYM'):
            shutil.rmtree(d, ignore_errors=True)

    return compile_cmd, run_cmd, cleanup


STRATEGIES = {
    '.cpp': cpp_strategy,
    '.cc': cpp_strategy,
    '.cxx': cpp_strategy,
}


def run_test_cases(src_file, input_file='input.txt', output_file='output.txt', specific_case=None, compare=False):
    invocation_start = time.time()
    ext = os.path.splitext(src_file)[1]
    if ext not in STRATEGIES:
        print(f"{RED}{BOLD}Unsupported file type '{ext}'. Supported: {', '.join(STRATEGIES)}{RESET}")
        return

    compile_cmd, run_cmd, cleanup = STRATEGIES[ext](src_file, compare)
    all_tests_passed = True

    with open(input_file, 'r') as file:
        input_cases = re.split(r'~{3,}', file.read().strip())

    if compare:
        with open(output_file, 'r') as file:
            output_cases = re.split(r'~{3,}', file.read().strip())

    compile_result = subprocess.run(compile_cmd, capture_output=True, text=True)
    if compile_result.returncode != 0:
        print(f"{RED}{BOLD}Compilation Error:{RESET}\n{compile_result.stderr}")
        return
    compile_time = time.time() - invocation_start
    # Surface warnings even on a successful compile (-Wconversion, etc.)
    if compile_result.stderr.strip():
        print(f"{BOLD}Compiler warnings:{RESET}\n{compile_result.stderr.strip()}")
        print("-" * 50)

    run_env = os.environ | SANITIZER_ENV

    test_cases_to_run = range(len(input_cases)) if specific_case is None else [specific_case - 1]

    for i in test_cases_to_run:
        input_case = input_cases[i]
        with open('temp_input.txt', 'w') as file:
            file.write(input_case.strip())

        start_time = time.time()
        with open('temp_input.txt', 'r') as infile:
            try:
                if compare:
                    # keep stdout clean for comparison; sanitizer reports (stderr) shown separately
                    run_result = subprocess.run(run_cmd, stdin=infile, text=True, errors='replace',
                                                capture_output=True, timeout=TIME_LIMIT_S, env=run_env)
                else:
                    # merge stderr into stdout so sanitizer reports interleave with output in program order
                    run_result = subprocess.run(run_cmd, stdin=infile, text=True, errors='replace',
                                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=TIME_LIMIT_S, env=run_env)
            except subprocess.TimeoutExpired as e:
                run_result = None
                partial = e.stdout or b''  # bytes even with text=True
                if isinstance(partial, bytes):
                    partial = partial.decode(errors='replace')
        end_time = time.time()
        execution_time = end_time - start_time

        if run_result is None:
            print(f"{RED}{BOLD}Time Limit Exceeded in Test Case {i+1} (killed after {TIME_LIMIT_S}s){RESET}")
            if partial.strip():
                print("  Output before kill:")
                print(partial.strip())
            all_tests_passed = False
        elif run_result.returncode != 0:
            print(f"{RED}{BOLD}Runtime Error in Test Case {i+1} (exit {run_result.returncode}):{RESET}")
            if run_result.stdout:
                print(run_result.stdout)
            if run_result.stderr:
                print(run_result.stderr)
            # Last thing printed, so the line number is right above the prompt.
            print(crash_site((run_result.stdout or '') + (run_result.stderr or ''), src_file), end='')
            all_tests_passed = False
        else:
            output_correct = True
            if compare:
                output_case_lines = output_cases[i].strip().split('\n')
                actual_output_lines = run_result.stdout.strip().split('\n')

                length_difference = len(output_case_lines) - len(actual_output_lines)
                if length_difference > 0:
                    actual_output_lines += [""] * length_difference
                elif length_difference < 0:
                    output_case_lines += [""] * (-length_difference)

                for expected_line, actual_line in zip(output_case_lines, actual_output_lines):
                    if expected_line.rstrip() != actual_line.rstrip():
                        output_correct = False
                        break

                if output_correct:
                    print(f"{GREEN}{BOLD}Test Case {i+1} SUCCESS: ALL OUTPUTS MATCH! (Execution Time: {execution_time:.4f} seconds){RESET}")
                else:
                    print(f"{RED}{BOLD}Test Case {i+1} ERROR: OUTPUTS DO NOT MATCH! (Execution Time: {execution_time:.4f} seconds){RESET}")
                    print("  Output:")
                    print(highlight_differences("\n".join(output_case_lines), "\n".join(actual_output_lines)))
                    all_tests_passed = False
            else:
                print(f"{BOLD}Test Case {i+1} Output (Execution Time: {execution_time:.4f} seconds):{RESET}")
                print(run_result.stdout.strip())

        print("-" * 50)

    total_time = time.time() - invocation_start
    print(f"{BOLD}Total: {total_time:.4f}s  (compile: {compile_time:.4f}s){RESET}")

    if all_tests_passed and compare:
        print(f"{GREEN}You're a stud, at least on the sample data{RESET}\n")

    if os.path.exists('temp_input.txt'):
        os.remove('temp_input.txt')
    cleanup()


# Options after the source file may come in any order: a case number, and/or 'c' to compare.
opts = sys.argv[2:]
bad = [a for a in opts if not (a.isdigit() or a == 'c')]
if len(sys.argv) < 2 or bad:
    print("Usage: python3 test_runner.py SourceFile.cpp [case number] [c]   (options in any order)")
else:
    specific_case = next((int(a) for a in opts if a.isdigit()), None)
    run_test_cases(sys.argv[1], specific_case=specific_case, compare='c' in opts)