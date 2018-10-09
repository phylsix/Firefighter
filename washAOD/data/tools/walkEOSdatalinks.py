#!/usr/bin/env python
from __future__ import print_function
import os
from datetime import datetime

BASEDIR = '/eos/uscms/store/user/wsi/MCSIDM'
PREFIX  = 'SIDM_BsTo2DpTo4Mu'
LINKFILEDIR = '../2017'
TOTEVENTS = '15k'
VERBOSE = True

def sort_timestring(timestrlist):
    """ 181008-110115 """
    timelist = [datetime.strptime(t, '%y%m%d-%H%M%S') for t in timestrlist]
    sorted(timelist)
    return [t.strftime('%y%m%d-%H%M%S') for t in timelist]


if __name__ == '__main__':

    for l in os.listdir(BASEDIR):
        if not l.startswith(PREFIX): continue

        timestampdir = sort_timestring(os.listdir(os.path.join(BASEDIR, l)))[0]
        targetdir = os.path.join(BASEDIR, l, timestampdir)
        linkfilename = l.replace('_slc6_amd64_gcc481_CMSSW_7_1_30_tarball', '') + '_' + TOTEVENTS + '.list'
        cmd = "ls {0}/*_AOD.root |sed 's/^/file:/' > {1}/{2}".format(targetdir, LINKFILEDIR, linkfilename)

        if VERBOSE:
            print(targetdir+'/*_AOD.root', '  ==>  '+LINKFILEDIR+'/'+linkfilename, sep='\n')

        os.system(cmd)
