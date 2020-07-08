#!/usr/bin/env python
"""GlobalTag from: https://twiki.cern.ch/twiki/bin/viewauth/CMS/PdmVAnalysisSummaryTable
"""
from __future__ import print_function

import json
import shlex
import subprocess
from os.path import join
from datetime import datetime

from rootpy.io import root_open
from Firefighter.ffConfig.datasetUtils import ffdatasetdata
from DataFormats.FWLite import Events


DATASOURCES = [
    '/DoubleMuon/Run2018A-17Sep2018-v2/AOD',
    '/DoubleMuon/Run2018B-17Sep2018-v1/AOD',
    '/DoubleMuon/Run2018C-17Sep2018-v1/AOD',
    '/DoubleMuon/Run2018D-PromptReco-v2/AOD',
]

XDIRECTOR = "root://cmseos.fnal.gov/"
SOURCEEOSPATH = "/store/group/lpcmetx/SIDM/Skim/2018"



def fetchFiles(d):
    dtag = d.split('-')[0][1:].replace('/', '_')
    eospath_ = SOURCEEOSPATH + d.rsplit('/', 1)[0]
    timestamps = subprocess.check_output(shlex.split('eos {0} ls {1}'.format(XDIRECTOR, eospath_))).split()
    if not timestamps:
        print("--> Zero timestamp directory found under", eospath_)
        print("--> Empty list returned for", dtag)
        return dtag, []
    timestamps = sorted(timestamps, key=lambda x: datetime.strptime(x, "%y%m%d_%H%M%S"))
    eospath = join(eospath_, timestamps[-1]) # most recent submission

    flist = []

    try:
        flist = subprocess.check_output(shlex.split('eos {0} find -name "*ffAOD*.root" -f --xurl {1}'.format(XDIRECTOR, eospath))).split()
    except:
        print("--> cannot stat eos path: ", eospath)
        print("--> Empty list returned for", dtag)
        return dtag, []

    print(dtag, "Total number of files (uncleaned):", len(flist))

    nonzeroes_ = []
    for f in flist:
        nevents = 0
        try:
            thefile = root_open(f) # make sure it can be opened properly
        except Exception as e:
            print("--> Cannot open file", f)
            print(str(e))
            continue

        try:
            events = Events(f)
            for evt in events: nevents += 1
        except Exception as e:
            print("--> FWLite failed for", f)
            print(str(e))
            continue

        if nevents>0: nonzeroes_.append(f)

    return dtag, nonzeroes_




if __name__ == "__main__":

    datasetlist = []

    for d in DATASOURCES:
        dtag, flist = fetchFiles(d)
        if flist: datasetlist.append(dtag)

        flist = [f for f in flist if f]
        ffds = ffdatasetdata()
        ffds.datasetNames = [d]
        if '2018D' in dtag: ffds.globalTag = '102X_dataRun2_Prompt_v16'
        else:               ffds.globalTag = '102X_dataRun2_v13'
        ffds.fileList = [sorted(flist)]
        with open(dtag + '.yml', 'w') as f:
            f.write(ffds.dump())


    # description
    with open('description.json', 'w') as f:
        f.write(json.dumps(sorted(
            [join('src/Firefighter/ffConfig/python/production/Skim2LJ18/data', dtag + '.yml') for dtag in datasetlist]
            ), indent=4))
