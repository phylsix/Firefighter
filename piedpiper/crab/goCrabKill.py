#!/usr/bin/env python
from __future__ import print_function
import os
import sys
import time
from CRABAPI.RawCommand import crabCommand

KEYWORD = sys.argv[1]


def main():

    toKill = [d for d in os.listdir("crabWorkArea") if KEYWORD in d]
    print("Going to kill:", *toKill, sep="\n")

    for t in toKill:
        relDir = os.path.join("crabWorkArea", t)
        try:
            crabCommand("kill", dir=relDir)
        except:
            print("EXCEPTION: ", relDir)
            pass


if __name__ == "__main__":
    main()
