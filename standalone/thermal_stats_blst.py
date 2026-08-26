#!/usr/bin/env python3
"""
thermal_stats_blst.py

Standalone Python equivalent of thermal_stats_blst.c.
It loads a small C helper (libblst_ir_helper.so) that links against the bundled
BLST SDK archive, then parses the proprietary temperature matrix stored inside
BLST IR image files (e.g. *_I.jpg).

Build the helper (run from this directory):
    gcc -std=c99 -O2 -shared -fPIC \
        -I../include \
        blst_ir_helper.c \
        ../lib/linux-x86_64/libblstsdk.a \
        -o libblst_ir_helper.so -lm

Usage:
    python3 thermal_stats_blst.py 19700101_001348_I.jpg

The script looks for libblst_ir_helper.so in the same directory as this file.
"""

import argparse
import os
import sys
from ctypes import CDLL, POINTER, c_char_p, c_float, c_int, c_size_t, c_uint16


def load_helper():
    """Load the shared helper that wraps the BLST SDK static archive."""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    helper_names = [
        os.path.join(script_dir, "libblst_ir_helper.so"),
        os.path.join(script_dir, "libblst_ir_helper.dll"),
        "libblst_ir_helper.so",
        "libblst_ir_helper.dll",
    ]

    last_error = None
    for name in helper_names:
        try:
            return CDLL(name)
        except OSError as exc:
            last_error = exc
            continue

    sys.stderr.write(
        "Error: could not load libblst_ir_helper.so\n"
        "Build it with:\n"
        "    gcc -std=c99 -O2 -shared -fPIC -I../include \\\n"
        "        blst_ir_helper.c \\\n"
        "        ../lib/linux-x86_64/libblstsdk.a \\\n"
        "        -o libblst_ir_helper.so -lm\n"
    )
    if last_error:
        sys.stderr.write(f"Underlying error: {last_error}\n")
    sys.exit(1)


def main():
    parser = argparse.ArgumentParser(
        description="Extract temperature statistics from a BLST IR image."
    )
    parser.add_argument("image", help="Path to the BLST IR image (e.g. *_I.jpg)")
    parser.add_argument(
        "--matrix",
        action="store_true",
        help="Print the full temperature matrix",
    )
    parser.add_argument(
        "--csv",
        metavar="FILE",
        help="Also write the temperature matrix to a CSV file",
    )
    args = parser.parse_args()

    lib = load_helper()

    # int blst_ir_helper_get_info(const char *, uint16_t *, uint16_t *, size_t *)
    lib.blst_ir_helper_get_info.argtypes = [
        c_char_p,
        POINTER(c_uint16),
        POINTER(c_uint16),
        POINTER(c_size_t),
    ]
    lib.blst_ir_helper_get_info.restype = c_int

    # int blst_ir_helper_parse_matrix(const char *, float *, size_t)
    lib.blst_ir_helper_parse_matrix.argtypes = [
        c_char_p,
        POINTER(c_float),
        c_size_t,
    ]
    lib.blst_ir_helper_parse_matrix.restype = c_int

    width = c_uint16()
    height = c_uint16()
    count = c_size_t()

    path_bytes = args.image.encode("utf-8")
    ret = lib.blst_ir_helper_get_info(path_bytes, width, height, count)
    if ret != 0:
        sys.stderr.write(
            f"Error: blst_ir_helper_get_info failed for '{args.image}' (ret={ret})\n"
            "       The file is not a supported BLST IR image or the SDK could not read it.\n"
        )
        return 1

    w, h, n = width.value, height.value, count.value
    if n == 0 or w == 0 or h == 0:
        sys.stderr.write(f"Error: invalid image info (width={w} height={h} count={n})\n")
        return 1

    temps = (c_float * n)()
    ret = lib.blst_ir_helper_parse_matrix(path_bytes, temps, n)
    if ret != 0:
        sys.stderr.write(f"Error: blst_ir_helper_parse_matrix failed (ret={ret})\n")
        return 1

    # Compute statistics
    min_temp = temps[0]
    max_temp = temps[0]
    min_idx = 0
    max_idx = 0
    total = 0.0

    for i in range(n):
        t = temps[i]
        total += t
        if t < min_temp:
            min_temp = t
            min_idx = i
        if t > max_temp:
            max_temp = t
            max_idx = i

    min_x = min_idx % w
    min_y = min_idx // w
    max_x = max_idx % w
    max_y = max_idx // w
    avg_temp = total / n
    diff_temp = max_temp - min_temp

    print("[Image Info]")
    print(f"Width:  {w}")
    print(f"Height: {h}")
    print(f"Total Pixels: {n}")
    print()

    print("[Temperature Statistics]")
    print(f"Lowest Temperature:  {min_temp:.2f} C @ ({min_x}, {min_y})")
    print(f"Highest Temperature: {max_temp:.2f} C @ ({max_x}, {max_y})")
    print(f"Average Temperature: {avg_temp:.2f} C")
    print(f"Temperature Difference: {diff_temp:.2f} C")
    print()

    if args.matrix:
        print(f"[Temperature Matrix] ({w} x {h})")
        for y in range(h):
            row = [f"{temps[y * w + x]:.2f}" for x in range(w)]
            print(" ".join(row))
        print()

    if args.csv:
        with open(args.csv, "w", encoding="utf-8") as f:
            for y in range(h):
                row = [f"{temps[y * w + x]:.2f}" for x in range(w)]
                f.write(",".join(row) + "\n")
        print(f"[CSV] Wrote temperature matrix to {args.csv}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
