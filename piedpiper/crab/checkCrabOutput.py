#!/usr/bin/env python2.7
from __future__ import print_function
import os
import time
from CRABAPI.RawCommand import crabCommand


CRAB_WORK_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'crabWorkArea')
VERBOSE = True

def main():

    crabTaskList = [os.path.join(CRAB_WORK_DIR, d) for d in os.listdir(CRAB_WORK_DIR)]

    with open('checkCrabOutput.log', 'w') as of:
        of.write(time.asctime()+'\n')
        of.write('='*79 + '\n\n')

        for t in crabTaskList:

            statusDict = crabCommand('status', dir='crabWorkArea/{0}'.format(t))

            of.write('directory:\t {0}\n'.format(os.path.join(CRAB_WORK_DIR, t)))
            of.write('task:\t {0}\n'.format(statusDict.get('userWebDirURL', '').split('/')[-1]))
            of.write('status:\t {0} {1}/{2}\n'.format(
                statusDict.get('status', ''),
                statusDict.get('jobsPerStatus', {}).get('finished', 0),
                statusDict.get('jobsPerStatus', {}).get('finished', 0)+statusDict.get('jobsPerStatus', {}).get('failed', 0),
            ))
            of.write('publication:\t dataset: {0} | done: {1}\n'.format(
                statusDict.get('outdatasets', ''),
                statusDict.get('publication', {}).get('done', 0)
            ))


if __name__ == "__main__":
    main()