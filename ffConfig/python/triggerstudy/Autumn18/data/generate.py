#!/usr/bin/env python
"""GlobalTag from: https://twiki.cern.ch/twiki/bin/viewauth/CMS/PdmVAnalysisSummaryTable
"""

import json
import shlex
import subprocess

pyfrag = """\
#!/usr/bin/env python

ffDataSet = {
    'datasetNames': %s,
    'maxEvents': -1,
    'globalTag': '%s',
    'lumiMask': 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions18/13TeV/ReReco/Cert_314472-325175_13TeV_17SeptEarlyReReco2018ABC_PromptEraD_Collisions18_JSON_MuonPhys.txt',
    'fileList': %s,
}
"""

datasources_abc = """\
/SingleMuon/Run2018A-17Sep2018-v2/AOD
/SingleMuon/Run2018B-17Sep2018-v1/AOD
/SingleMuon/Run2018C-17Sep2018-v1/AOD"""

datasources_d = '/SingleMuon/Run2018D-22Jan2019-v2/AOD'

datasetlist = []

# abc
for d in datasources_abc.split():
    dtag = d.split('-')[0][1:].replace('/', '_')
    datasetlist.append(dtag)
    flist = subprocess.check_output(
            shlex.split('dasgoclient -query="file dataset={0}"'.format(d))
        ).split()
    with open(dtag + '.py', 'w') as f:
        f.write(pyfrag % (str([d]), '102X_dataRun2_v11', str([sorted(flist)])))

# d
d = datasources_d
dtag = d.split('-')[0][1:].replace('/', '_')
datasetlist.append(dtag)
flist = subprocess.check_output(
        shlex.split('dasgoclient -query="file dataset={0}"'.format(d))
    ).split()
with open(dtag + '.py', 'w') as f:
    f.write(pyfrag % (str([d]), '102X_dataRun2_Prompt_v14', str([sorted(flist)])))

# description
with open('description.json', 'w') as f:
    f.write(json.dumps(['Firefighter.ffConfig.production.Autumn18.data.{0}'.format(dtag) for dtag in datasetlist]))
