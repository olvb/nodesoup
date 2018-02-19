#!/usr/bin/env python3

## run all tests in tests/ directory
## or with file given as arg (ex ./run-tests tests/01_decl_assign.py)

import sys
import os
import subprocess

BIN_PATH = "./bin/dot2png"
DOTS_PATH = "samples/"
PNGS_PATH = "samples/"
PNG_THUMBS_PATH = "samples/thumbs/"

FR_CMD = BIN_PATH + " -m fr {} {}"
KK_CMD = BIN_PATH + " -m kk {} {}"
GV_CMD = "neato -Nshape=point -Nwidth=0.1 -Gsize=10,7\! -Gdpi=100 -Tpng {} -o {}"
GV_RESIZE_CMD = "convert {} -resize 930x700 -gravity center -extent 1024x768 {}"
THUMB_CMD = "convert {} -resize 50% {}"


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

pngs = []
for dot in dots:
    name = os.path.splitext(os.path.basename(dot))[0]

    print(dot + " fr")
    png = PNGS_PATH + name + "_fr.png"
    cmd = FR_CMD.format(dot, png)
    subprocess.run([cmd], shell=True)
    pngs.append(png)

    print(dot + " kk")
    png = PNGS_PATH + name + "_kk.png"
    cmd = KK_CMD.format(dot, png)
    subprocess.run([cmd], shell=True)
    pngs.append(png)

    print(dot + " graphviz")
    png = PNGS_PATH + name + "_gv.png"
    cmd = GV_CMD.format(dot, png)
    subprocess.run([cmd], shell=True)
    cmd = GV_RESIZE_CMD.format(png, png)
    subprocess.run([cmd], shell=True)
    pngs.append(png)

if not os.path.exists(PNG_THUMBS_PATH):
    os.makedirs(PNG_THUMBS_PATH)

for png in pngs:
    filename = os.path.basename(png)
    thumb = PNG_THUMBS_PATH + filename
    cmd = THUMB_CMD.format(png, thumb)
    subprocess.run([cmd], shell=True)
