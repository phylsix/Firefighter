#!/usr/bin/env python
"""GlobalTag from: https://twiki.cern.ch/twiki/bin/viewauth/CMS/PdmVAnalysisSummaryTable
"""
from __future__ import print_function
import json
import shlex
import subprocess
from datetime import datetime
from os.path import join

from Firefighter.ffConfig.datasetUtils import ffdatasetbackground
from DataFormats.FWLite import Events

def countingEvents(fn):
    return sum([1 for e in Events(fn)])

DATASOURCES = """\
/WW_TuneCP5_13TeV-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM
/WZ_TuneCP5_13TeV-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v3/AODSIM
/ZZ_TuneCP5_13TeV-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM
/DYJetsToLL_M-10to50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM
/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM
/QCD_Pt-15to20_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v3/AODSIM
/QCD_Pt-20to30_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v4/AODSIM
/QCD_Pt-30to50_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v3/AODSIM
/QCD_Pt-50to80_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v3/AODSIM
/QCD_Pt-80to120_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM
/QCD_Pt-80to120_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM
/QCD_Pt-120to170_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM
/QCD_Pt-120to170_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM
/QCD_Pt-170to300_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v3/AODSIM
/QCD_Pt-300to470_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext3-v1/AODSIM
/QCD_Pt-300to470_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v3/AODSIM
/QCD_Pt-470to600_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM
/QCD_Pt-470to600_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM
/QCD_Pt-600to800_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM
/QCD_Pt-800to1000_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext3-v2/AODSIM
/QCD_Pt-1000toInf_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM
/TTJets_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM"""

XDIRECTOR = "root://cmseos.fnal.gov/"
SOURCEEOSPATH = "/store/group/lpcmetx/SIDM/Skim/2018"

if __name__ == "__main__":

    datasets = {}

    for d in DATASOURCES.split():
        dtag = d.split("/")[1]
        if dtag in datasets:
            datasets[dtag].append(d)
        else:
            datasets[dtag] = [d]

    def singleWrite(dtag, datasets):
        fragName = dtag + ".yml"
        flist = []
        for ds in datasets[dtag]:
            subflist = []
            try:
                eospath_ = SOURCEEOSPATH + ds.rsplit('/', 1)[0]
                timestamps = subprocess.check_output(shlex.split('eos {0} ls {1}'.format(XDIRECTOR, eospath_))).split()
                timestamps = sorted(timestamps, key=lambda x: datetime.strptime(x, "%y%m%d_%H%M%S"))
                eospath = join(eospath_, timestamps[-1]) # most recent submission
                subflist = subprocess.check_output(shlex.split('eos {0} find -name "*ffAOD*.root" -f --xurl {1}'.format(XDIRECTOR, eospath))).split()
                subflist = [f for f in subflist if countingEvents(f)!=0]
            except:
                print("cannot stat eos path: ", ds)
                print("empty list returned!")
            subflist = [f for f in subflist if f]
            flist.append(sorted(subflist))

        if sum([len(subf) for subf in flist])==0:
            print("no skimmed events left after filtering, won't dump.")
            return

        ffds = ffdatasetbackground()
        ffds.datasetNames = datasets[dtag]
        ffds.fileList = flist
        with open(fragName, "w") as f:
            f.write(ffds.dump())

    for i, k in enumerate(datasets, start=1):
        print('[{}/{}]  {}'.format(i, len(datasets), k))
        singleWrite(k, datasets)

    # summary
    with open('description.json', 'w') as f:
        f.write(json.dumps(sorted(
            [join('src/Firefighter/ffConfig/python/production/Skim2LJ18/bkgmc/', dtag + '.yml') for dtag in datasets]
            ), indent=4))
