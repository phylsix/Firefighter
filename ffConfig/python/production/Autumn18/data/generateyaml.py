#!/usr/bin/env python
"""GlobalTag from: https://twiki.cern.ch/twiki/bin/viewauth/CMS/PdmVAnalysisSummaryTable
"""

import json
import shlex
import subprocess
from os.path import join

from Firefighter.ffConfig.datasetUtils import ffdatasetdata


DATASETS = [
    "/DoubleMuon/Run2018A-17Sep2018-v2/AOD",
    "/DoubleMuon/Run2018B-17Sep2018-v1/AOD",
    "/DoubleMuon/Run2018C-17Sep2018-v1/AOD",
    "/DoubleMuon/Run2018D-PromptReco-v2/AOD",

    "/SingleMuon/Run2018A-17Sep2018-v2/AOD",
    "/SingleMuon/Run2018B-17Sep2018-v1/AOD",
    "/SingleMuon/Run2018C-17Sep2018-v1/AOD",
    "/SingleMuon/Run2018D-22Jan2019-v2/AOD",
]



if __name__ == "__main__":

    datasetlist = []

    for d in DATASETS:
        dtag = d.split('-')[0][1:].replace('/', '_')
        datasetlist.append(dtag)
        flist = subprocess.check_output(
                shlex.split('dasgoclient -query="file dataset={0}"'.format(d))
            ).split()

        ffds = ffdatasetdata()
        ffds.datasetNames = [d]
        if 'Run2018D' in d: ffds.globalTag = '102X_dataRun2_Prompt_v16'
        else:               ffds.globalTag = '102X_dataRun2_v13'
        ffds.fileList = [sorted(flist)]
        with open(dtag + '.yml', 'w') as f:
            f.write(ffds.dump())



    # description
    with open('description.json', 'w') as f:
        f.write(json.dumps(sorted([
            join('src/Firefighter/ffConfig/python/production/Autumn18/data', dtag + '.yml')
            for dtag in datasetlist if dtag.startswith('DoubleMuon')
        ]), indent=4))
