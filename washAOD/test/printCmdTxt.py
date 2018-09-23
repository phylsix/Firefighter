#!/usr/bin/env python
from __future__ import print_function

year = 2017
frag = 'dsaMatcher'

def main():

    print()
    print("#"*79)
    print("YEAR = ", year)
    print("FRAG = ", '%s_cfg.py'%frag)
    print("#"*79)
    print()

    ctaus = ['1p20e-03', '0p012', '0p12', '0p6', '1p2', '3p6']
    for c in ctaus:
        print("nohup ./mkNtuple.sh SIDMmumu_Mps-200_MZp-1p2_ctau-{0}_100k {1} {2} &".format(
            c, frag, year))

if __name__ == '__main__':
    main()