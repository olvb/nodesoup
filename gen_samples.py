#!/usr/bin/env python3

## run all tests in tests/ directory
## or with file given as arg (ex ./run-tests tests/01_decl_assign.py)

import sys
import os
import subprocess

BIN_PATH = "./bin/dot2png"
DOTS_PATH = "samples/"
PNGS_PATH = "samples/"

dots = []
if len(sys.argv) > 1:
    dots = [sys.argv[1]]
else:
    for dotfile in os.scandir(DOTS_PATH):
        if dotfile.is_dir():
            continue
        filename = dotfile.name
        if not filename.endswith(".dot"):
            continue
        dots.append(DOTS_PATH + filename)
dots.sort()

if not os.path.exists(PNGS_PATH):
    os.makedirs(PNGS_PATH)

for dot in dots:
    name = os.path.splitext(os.path.basename(dot))[0]

    print(dot + " fr")
    cmd_fr = BIN_PATH + " -m fr " + dot + " " + PNGS_PATH + name + "_fr.png"
    subprocess.run([cmd_fr], shell=True)

    print(dot + " kk")
    cmd_kk = BIN_PATH + " -m kk " + dot + " " + PNGS_PATH + name + "_kk.png"
    subprocess.run([cmd_kk], shell=True)
