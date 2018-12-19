#!/usr/bin/env python
from __future__ import print_function
import os
import time
from CRABAPI.RawCommand import crabCommand

def main():

    toResub = [d for d in os.listdir('crabWorkArea') if 'GENSIM' in d]

    for t in toResub:
        
        relDir = os.path.relpath(os.path.abspath(t), '.')
        statusDict = crabCommand('status', dir=relDir)
        if statusDict.get('status', '') != 'FAILED': continue
        
        print('-'*79)
        print(os.path.abspath(t))
        crabCommand('resubmit', dir=relDir)
        print('-'*79)
        time.sleep(1)


if __name__ == "__main__":
    main()