#!/usr/bin/env python
"""GlobalTag from: https://twiki.cern.ch/twiki/bin/viewauth/CMS/PdmVAnalysisSummaryTable
"""

import json
import shlex
import subprocess
from os.path import join

from Firefighter.ffConfig.datasetUtils import ffdatasetdata


datasources_abc = """\
/SingleMuon/Run2018A-17Sep2018-v2/AOD
/SingleMuon/Run2018B-17Sep2018-v1/AOD
/SingleMuon/Run2018C-17Sep2018-v1/AOD"""

datasources_d = '/SingleMuon/Run2018D-22Jan2019-v2/AOD'


if __name__ == "__main__":

    datasetlist = []

    # abc
    for d in datasources_abc.split():
        dtag = d.split('-')[0][1:].replace('/', '_')
        datasetlist.append(dtag)
        flist = subprocess.check_output(
                shlex.split('dasgoclient -query="file dataset={0}"'.format(d))
            ).split()

        ffds = ffdatasetdata()
        ffds.datasetNames = [d]
        ffds.globalTag = '102X_dataRun2_v11'
        ffds.fileList = [sorted(flist)]
        with open(dtag + '.yml', 'w') as f:
            f.write(ffds.dump())

    # d
    d = datasources_d
    dtag = d.split('-')[0][1:].replace('/', '_')
    datasetlist.append(dtag)
    flist = subprocess.check_output(
            shlex.split('dasgoclient -query="file dataset={0}"'.format(d))
        ).split()

    ffds = ffdatasetdata()
    ffds.datasetNames = [d]
    ffds.globalTag = '102X_dataRun2_Prompt_v14'
    ffds.fileList = [sorted(flist)]
    with open(dtag + '.yml', 'w') as f:
        f.write(ffds.dump())

    # description
    with open('description.json', 'w') as f:
        f.write(json.dumps(sorted(
            [join('src/Firefighter/ffConfig/python/production/Autumn18/data', dtag + '.yml') for dtag in datasetlist]
            ), indent=4))
