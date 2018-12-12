#!/usr/bin/env python2.7
from __future__ import print_function
import os
import time
import subprocess

CRAB_WORK_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'crabWorkArea')
VERBOSE = True

def main():

    crabTaskList = [os.path.join(CRAB_WORK_DIR, d) for d in os.listdir(CRAB_WORK_DIR)]
    cmd = 'crab status -d {0}'

    with open('checkCrabOutput.log', 'w') as of:
        of.write(time.asctime()+'\n')
        of.write('='*79 + '\n\n')

        toWriteTags = ['CRAB project directory', 'Task name', 'Jobs status', 'Output dataset']
        for t in crabTaskList:
            print(cmd.format(t))
            try:
                raw = subprocess.check_output(cmd.format(t).split())
                for line in raw.split('\n'):
                    if line.split(':')[0] in toWriteTags:
                        of.write(line+'\n')
                        if VERBOSE: print(line)
                of.write('-'*79+'\n\n')
            except:
                print('crab status error -> '+t)
                pass

if __name__ == "__main__":
    main()