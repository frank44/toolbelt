import subprocess
import os
import sys
import time
import glob
import re
import shutil
import hashlib

# ===== C++ compiler config (edit here) =====
CPP_COMPILER = 'g++-16'
CPP_FLAGS = [
    '-std=c++23', '-O2', '-g',
    '-DLOCAL',
    '-Wall', '-Wextra', '-Wshadow',
    '-D_GLIBCXX_ASSERTIONS',
    '-Winvalid-pch',
    # --- debug/sanitizer flags (local only; never ship to CF) ---
    '-fsanitize=address,undefined',  # catch OOB/UAF + UB (signed overflow, etc.)
    '-fno-sanitize-recover=all',     # abort + nonzero exit on first error, so a test FAILS
    '-fno-omit-frame-pointer',       # readable stack traces in sanitizer reports
    '-D_GLIBCXX_DEBUG',              # bounds-check vector::operator[], catch bad iterators
]

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


# ===== Per-language strategy =====
# Each builder returns (compile_cmd_or_None, run_cmd, cleanup_fn)

def java_strategy(src):
    cls = os.path.splitext(os.path.basename(src))[0]
    compile_cmd = ['javac', '-g', src]
    run_cmd = ['java', cls]

    def cleanup():
        for f in glob.glob('*.class'):
            os.remove(f)

    return compile_cmd, run_cmd, cleanup


def cpp_strategy(src):
    binary = os.path.splitext(os.path.basename(src))[0]
    pch_inc = ensure_pch(CPP_COMPILER, CPP_FLAGS)
    compile_cmd = [CPP_COMPILER] + CPP_FLAGS + pch_inc + [src, '-o', binary, '-lstdc++exp']
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
    '.java': java_strategy,
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

    compile_cmd, run_cmd, cleanup = STRATEGIES[ext](src_file)
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

    test_cases_to_run = range(len(input_cases)) if specific_case is None else [specific_case - 1]

    for i in test_cases_to_run:
        input_case = input_cases[i]
        with open('temp_input.txt', 'w') as file:
            file.write(input_case.strip())

        start_time = time.time()
        with open('temp_input.txt', 'r') as infile:
            if compare:
                # keep stdout clean for comparison; debug (stderr) shown separately
                run_result = subprocess.run(run_cmd, stdin=infile, text=True, errors='replace', capture_output=True)
            else:
                # merge stderr into stdout so debug interleaves with output in program order
                run_result = subprocess.run(run_cmd, stdin=infile, text=True, errors='replace',
                                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        end_time = time.time()
        execution_time = end_time - start_time

        if run_result.returncode != 0:
            print(f"{RED}{BOLD}Runtime Error in Test Case {i+1} (exit {run_result.returncode}):{RESET}")
            if run_result.stdout:
                print(run_result.stdout)
            if run_result.stderr:
                print(run_result.stderr)
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


if len(sys.argv) not in [2, 3, 4]:
    print("Usage: python3 test_runner.py [SourceFile.java|.cpp] [optional: specific test case number] [optional: 'c' to compare]")
else:
    specific_case = int(sys.argv[2]) if len(sys.argv) >= 3 and sys.argv[2].isdigit() else None
    compare = 'c' in sys.argv
    run_test_cases(sys.argv[1], specific_case=specific_case, compare=compare)