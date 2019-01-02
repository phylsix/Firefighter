#!/usr/bin/env python
from __future__ import print_function
import os
import time
from CRABAPI.RawCommand import crabCommand

def main():

    toResub = ['crabWorkArea/{0}'.format(d) for d in os.listdir('crabWorkArea') if 'AODSIM' in d]

    for t in toResub:

        relDir = os.path.relpath(os.path.abspath(t), '.')
        statusDict = dict()
        try:
            statusDict = crabCommand('status', dir=relDir)
        except Exception as e:
            print(str(e))
            pass

        if statusDict.get('publication', {}).get('failed', 0) != 0:
            print('-'*79)
            print(os.path.abspath(t))
            crabCommand('resubmit', dir=relDir, publication=True)
            print('-'*79)
            time.sleep(1)

        if statusDict.get('status', '') == 'FAILED' \
            or statusDict.get('jobsPerStatus',{}).get('failed',0) != 0:
            print('-'*79)
            print(os.path.abspath(t))
            crabCommand('resubmit', dir=relDir)
            print('-'*79)
            time.sleep(1)


if __name__ == "__main__":
    main()
