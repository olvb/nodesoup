#!/usr/bin/env python3

import os
import subprocess
import re

FR_CMD = './bin/dot2png -m fr samples/{0}.dot samples/{0}_fr.png'
KK_CMD = './bin/dot2png -m kk samples/{0}.dot samples/{0}_kk.png'
GV_CMD = 'neato -Nshape=point -Nwidth=0.1 -Gsize=10,7\! -Gdpi=100 -Tpng samples/{0}.dot -o samples/{0}_gv.png'
GV_RESIZE_CMD = 'convert samples/{0}_gv.png -resize 930x700 -gravity center -extent 1024x768 samples/{0}_gv.png'
THUMB_CMD = 'convert samples/{0}.png -resize 50% samples/thumbs/{0}.png'
ANIM_CMD = './bin/dot2png -a -m fr samples/{0}.dot samples/{0}_frame.png'
ANIM_COMPILE_CMD = 'convert -colorspace gray -loop 256 -delay 5 {1} -delay 300 {2} -layers optimize samples/thumbs/{0}_fr.gif'

def gen_fr(name):
    print('Generating ' + name + ' fr...')
    cmd = FR_CMD.format(name)
    subprocess.run([cmd], shell=True)
    gen_thumb(name + '_fr')

def gen_kk(name):
    print('Generating ' + name + ' kk...')
    cmd = KK_CMD.format(name)
    subprocess.run([cmd], shell=True)
    gen_thumb(name + '_kk')

def gen_graphviz(name):
    print('Generating ' + name + ' graphviz...')
    cmd = GV_CMD.format(name)
    subprocess.run([cmd], shell=True)
    cmd = GV_RESIZE_CMD.format(name)
    subprocess.run([cmd], shell=True)
    gen_thumb(name + '_gv')

def gen_thumb(name):
    cmd = THUMB_CMD.format(name)
    subprocess.run([cmd], shell=True)

def gen_fr_anim(name):
    print('Generating ' + name + ' anim...')
    # generate all frames
    cmd = ANIM_CMD.format(name)
    subprocess.run([cmd], shell=True)

    # identify frames
    frame_names = []
    for file in os.scandir('samples/'):
        if not re.match(name + r'_frame_\d{3}.png', file.name):
            continue
        frame_name = file.name[:-len('.png')]
        frame_names.append(frame_name)

    # scale down all frames to thumbnail format
    for frame_name in frame_names:
        gen_thumb(frame_name)
        os.unlink('samples/' + frame_name + '.png')

    # compile all frames in one gif file
    frame_names.sort()
    frame_thumbs = ['samples/thumbs/' + fn + '.png' for fn in frame_names]
    cmd = ANIM_COMPILE_CMD.format(name, ' '.join(frame_thumbs), frame_thumbs[-1])
    subprocess.run([cmd], shell=True)

    # delete frames thumb images
    for frame_thumb in frame_thumbs:
        os.unlink(frame_thumb)

def main():
    if not os.path.exists('samples/thumbs/'):
        os.makedirs('samples/thumbs/')

    for file in os.scandir('samples/'):
        if not file.name.endswith('.dot'):
            continue

        name = file.name[:-len('.dot')]

        gen_fr(name)
        if not 'large' in name:
            gen_kk(name)
        gen_graphviz(name)
        gen_fr_anim(name)

main()
