#!/usr/bin/env python
from __future__ import print_function
import os
from datetime import datetime

BASEDIR = '/eos/uscms/store/user/ranchen/MCSIDM'
#BASEDIR = '/eos/uscms/store/user/wsi/MCSIDM'
PREFIX  = 'SIDM_BsTo2DpTo2Mu2e'
#PREFIX  = 'SIDM_BsTo2DpTo4Mu'
LINKFILEDIR = '../2018'
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
        linkfilename = l.replace('_slc6_amd64_gcc481_CMSSW_7_1_30_tarball', '') + '_TOTEVENTS' + '.list'
        cmd0 = "ls {0}/*_AOD.root |sed 's/^/file:/' > {1}/{2}".format(targetdir, LINKFILEDIR, linkfilename)
        os.system(cmd0)

        truelinkfilename = linkfilename.replace('TOTEVENTS', str(len(open(LINKFILEDIR+'/'+linkfilename).readlines()))+'k')
        cmd1 = "mv {0}/{1} {0}/{2}".format(LINKFILEDIR, linkfilename, truelinkfilename)
        os.system(cmd1)

        if VERBOSE:
            print(targetdir+'/*_AOD.root', '  ==>  '+LINKFILEDIR+'/'+truelinkfilename, sep='\n')
