#!/usr/bin/env python
"""GlobalTag from: https://twiki.cern.ch/twiki/bin/viewauth/CMS/PdmVAnalysisSummaryTable
"""
from __future__ import print_function

import json
import shlex
import subprocess
from os.path import join
from datetime import datetime

from Firefighter.ffConfig.datasetUtils import ffdatasetdata
from DataFormats.FWLite import Events

def countingEvents(fn):
    return sum([1 for e in Events(fn)])

DATASOURCES_ABC = """\
/DoubleMuon/Run2018A-17Sep2018-v2/AOD
/DoubleMuon/Run2018B-17Sep2018-v1/AOD
/DoubleMuon/Run2018C-17Sep2018-v1/AOD"""

DATASOURCES_D = '/DoubleMuon/Run2018D-PromptReco-v2/AOD'

XDIRECTOR = "root://cmseos.fnal.gov/"
SOURCEEOSPATH = "/store/group/lpcmetx/SIDM/Skim/2018"

if __name__ == "__main__":

    datasetlist = []

    # abc
    for d in DATASOURCES_ABC.split():
        dtag = d.split('-')[0][1:].replace('/', '_')
        datasetlist.append(dtag)
        flist = []
        try:
            eospath_ = SOURCEEOSPATH + d.rsplit('/', 1)[0]
            timestamps = subprocess.check_output(shlex.split('eos {0} ls {1}'.format(XDIRECTOR, eospath_))).split()
            timestamps = sorted(timestamps, key=lambda x: datetime.strptime(x, "%y%m%d_%H%M%S"))
            eospath = join(eospath_, timestamps[-1]) # most recent submission
            flist = subprocess.check_output(shlex.split('eos {0} find -name "*ffAOD*.root" -f --xurl {1}'.format(XDIRECTOR, eospath))).split()
            flist = [f for f in flist if countingEvents(f)!=0]
        except:
            print("cannot stat eos path: ", ds)
            print("empty list returned!")

        flist = [f for f in flist if f]
        ffds = ffdatasetdata()
        ffds.datasetNames = [d]
        ffds.globalTag = '102X_dataRun2_v12'
        ffds.fileList = [sorted(flist)]
        with open(dtag + '.yml', 'w') as f:
            f.write(ffds.dump())

    # d
    d = DATASOURCES_D
    dtag = d.split('-')[0][1:].replace('/', '_')
    datasetlist.append(dtag)
    flist = []
    try:
        eospath_ = SOURCEEOSPATH + d.rsplit('/', 1)[0]
        timestamps = subprocess.check_output(shlex.split('eos {0} ls {1}'.format(XDIRECTOR, eospath_))).split()
        timestamps = sorted(timestamps, key=lambda x: datetime.strptime(x, "%y%m%d_%H%M%S"))
        eospath = join(eospath_, timestamps[-1]) # most recent submission
        flist = subprocess.check_output(shlex.split('eos {0} find -name "*ffAOD*.root" -f --xurl {1}'.format(XDIRECTOR, eospath))).split()
        flist = [f for f in flist if countingEvents(f)!=0]
    except:
        print("cannot stat eos path: ", ds)
        print("empty list returned!")

    flist = [f for f in flist if f]
    ffds = ffdatasetdata()
    ffds.datasetNames = [d]
    ffds.globalTag = '102X_dataRun2_Prompt_v15'
    ffds.fileList = [sorted(flist)]
    with open(dtag + '.yml', 'w') as f:
        f.write(ffds.dump())

    # description
    with open('description.json', 'w') as f:
        f.write(json.dumps(sorted(
            [join('src/Firefighter/ffConfig/python/production/Skim2LJ18/data', dtag + '.yml') for dtag in datasetlist]
            ), indent=4))
