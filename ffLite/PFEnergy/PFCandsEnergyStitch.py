#!/usr/bin/env python
from __future__ import print_function
import os
import sys
from glob import glob

from PIL import Image

dataType = sys.argv[1]
outdir = os.path.join(os.path.dirname(os.path.abspath(__file__)), dataType)
if not os.path.exists(outdir):
    os.makedirs(outdir)


def main():

    indivImg = [x for x in os.listdir(outdir) if x.endswith('.png')]
    fromAod = [
        os.path.join(outdir, x) for x in indivImg if x.startswith('event')
    ]
    fromNtuple = [
        os.path.join(outdir, x) for x in indivImg if x.startswith('ffNtuple')
    ]

    toCombPair = []
    for im in fromAod:
        idx = os.path.basename(im).replace('.png', '').split('_')[1]
        matched = [
            x for x in fromNtuple
            if os.path.basename(x).replace('.png', '').split('_')[1] == idx
        ]
        if matched:
            toCombPair.append((im, matched[0]))

    for p in toCombPair:
        fn = p[0].replace('event', 'comb')
        widths, heights = zip(*(Image.open(i).size for i in p))
        total_width = sum(widths)
        max_height = max(heights)

        new_im = Image.new('RGB', (total_width, max_height))
        x_offset = 0
        for im_ in p:
            im = Image.open(im_)
            new_im.paste(im, (x_offset, 0))
            x_offset += im.size[0]
        new_im.save(fn)


if __name__ == "__main__":
    main()