#!/usr/bin/env python
from __future__ import print_function
import os
import time
from datetime import datetime
from CRABAPI.RawCommand import crabCommand
from CRABClient.ClientExceptions import ClientException
from httplib import HTTPException

def main():

    #toResub = ['crabWorkArea/{0}'.format(d) for d in os.listdir('crabWorkArea') if 'AODSIM' in d]
    toResub = [
            'crabWorkArea/{0}'.format(d) \
                    for d in os.listdir('crabWorkArea') \
                    if 'AODSIM' in d \
                    and ( datetime.now()-datetime.strptime(d.rsplit('_',1)[-1], '%y%m%d-%H%M%S') ).days<1
            ]

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
            try:
                crabCommand('resubmit', dir=relDir, publication=True)
            except HTTPException as hte:
                print('Failed to resubmit for task {0}: {1}'.format(relDir, hte.headers))
            except ClientException as cle:
                print('Failed to resubmit for task {0}: {1}'.format(relDir, cle))
            print('-'*79)
            time.sleep(1)

        if statusDict.get('status', '') == 'FAILED' \
            or statusDict.get('jobsPerStatus',{}).get('failed',0) != 0:
            print('-'*79)
            print(os.path.abspath(t))
            try:
                crabCommand('resubmit', dir=relDir)
            except HTTPException as hte:
                print('Failed to resubmit for task {0}: {1}'.format(relDir, hte.headers))
            except ClientException as cle:
                print('Failed to resubmit for task {0}: {1}'.format(relDir, cle))
            print('-'*79)
            time.sleep(1)


if __name__ == "__main__":
    main()
