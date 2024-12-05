#!/bin/env python

if __name__ != "__main__":
    exit(1)

import sys
import re

if len(sys.argv) not in (2,3):
    print("Usage: h2b.py <.hex> [.bin]")

inp = sys.argv[1]
out = inp + ".bin"

if len(sys.argv) == 3:
    out = sys.argv[2]

with open(inp, "r") as f:
    hex_dat = f.read()

hex_dat = re.sub(r"#.*", "", hex_dat)
hex_dat = re.sub(r"\s+", "", hex_dat)
bin_dat = bytes.fromhex(hex_dat)

with open(out, "wb") as f:
    f.write(bin_dat)
