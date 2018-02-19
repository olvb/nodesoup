#!/usr/bin/env python3

## run all tests in tests/ directory
## or with file given as arg (ex ./run-tests tests/01_decl_assign.py)

import sys
import os
import subprocess
import re

BIN_PATH = "./bin/dot2png"
DOTS_PATH = "samples/"
PNGS_PATH = "samples/"
THUMBS_PATH = "samples/thumbs/"
ANIMS_PATH = "samples/thumbs/"
FR_CMD = BIN_PATH + " -m fr {} {}"
KK_CMD = BIN_PATH + " -m kk {} {}"
GV_CMD = "neato -Nshape=point -Nwidth=0.1 -Gsize=10,7\! -Gdpi=100 -Tpng {} -o {}"
GV_RESIZE_CMD = "convert {} -resize 930x700 -gravity center -extent 1024x768 {}"
THUMB_CMD = "convert {} -resize 50% {}"
ANIM_CMD = BIN_PATH + " -a -m fr {} {}"
ANIM_COMPILE_CMD = "convert -colorspace gray -loop 256 -delay 5 {} -delay 300 {} -layers optimize {}"

dots = []
for dot_file in os.scandir(DOTS_PATH):
    filename = dot_file.name
    if not filename.endswith(".dot"):
        continue
    dots.append(DOTS_PATH + filename)
dots.sort()

if not os.path.exists(PNGS_PATH):
    os.makedirs(PNGS_PATH)

if not os.path.exists(THUMBS_PATH):
    os.makedirs(THUMBS_PATH)

def gen_fr(dot):
    print(dot + " fr")
    png = PNGS_PATH + name + "_fr.png"
    cmd = FR_CMD.format(dot, png)
    subprocess.run([cmd], shell=True)
    return png

def gen_kk(dot):
    print(dot + " kk")
    png = PNGS_PATH + name + "_kk.png"
    cmd = KK_CMD.format(dot, png)
    subprocess.run([cmd], shell=True)
    return png

def gen_graphviz(dot):
    print(dot + " graphviz")
    png = PNGS_PATH + name + "_gv.png"
    cmd = GV_CMD.format(dot, png)
    subprocess.run([cmd], shell=True)
    cmd = GV_RESIZE_CMD.format(png, png)
    subprocess.run([cmd], shell=True)
    return png

def gen_fr_anim(dot):
    print(dot + " anim")
    frame_model = THUMBS_PATH + name + "_frame.png"
    cmd = ANIM_CMD.format(dot, frame_model)
    subprocess.run([cmd], shell=True)

    # identify frame files
    frames = []
    frame_regexp = re.compile(name + r"_frame_\d{3}.png")
    for frame_file in os.scandir(THUMBS_PATH):
        filename = frame_file.name
        if not frame_regexp.match(filename):
            continue
        frame = THUMBS_PATH + filename
        frames.append(frame)

    # scale down all frames to thumbnail format
    for frame in frames:
        cmd = THUMB_CMD.format(frame, frame)
        subprocess.run([cmd], shell=True)
    frames.sort()

    # compile all frames in 1 gif
    anim = THUMBS_PATH + name + "_fr.gif"
    cmd = ANIM_COMPILE_CMD.format(' '.join(frames), frames[-1], anim)
    subprocess.run([cmd], shell=True)

    # delete frame images
    for frame in frames:
        os.unlink(frame)

    return anim

def gen_thumb(png):
    filename = os.path.basename(png)
    thumb = THUMBS_PATH + filename
    cmd = THUMB_CMD.format(png, thumb)
    subprocess.run([cmd], shell=True)
    return thumb

for dot in dots:
    name = os.path.splitext(os.path.basename(dot))[0]

    fr_png = gen_fr(dot)
    gen_thumb(fr_png)

    if not "large" in name:
        kk_png = gen_kk(dot)
        gen_thumb(kk_png)

    gv_png = gen_graphviz(dot)
    gen_thumb(gv_png)

    gen_fr_anim(dot)
