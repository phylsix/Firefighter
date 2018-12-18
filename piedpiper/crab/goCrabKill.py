#!/usr/bin/env python
from __future__ import print_function
import os
import time
from CRABAPI.RawCommand import crabCommand

def main():

    toResub = [d for d in os.listdir('crabWorkArea') if 'PREMIXRAWHLT' in d]

    for t in toResub:
        
        relDir = 'crabWorkArea/{0}'.format(t)
        crabCommand('kill', dir=relDir)

        time.sleep(1)


if __name__ == "__main__":
    main()