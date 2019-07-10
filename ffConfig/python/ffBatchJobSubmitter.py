#!/usr/bin/env python
"""submit jobs for a list of dataset
"""
from __future__ import print_function

import importlib
import os
from os.path import join

PRODUCTIONBASE = join(
    os.getenv("CMSSW_BASE"), "src/Firefighter/ffConfig/python/production/"
)

tosubd_ = [
    "Firefighter.ffConfig.production.Autumn18.bkgmc.ttWJets_TuneCP5_13TeV_madgraphMLM_pythia8",
    "Firefighter.ffConfig.production.Autumn18.bkgmc.WJetsToLNu_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8",
    "Firefighter.ffConfig.production.Autumn18.bkgmc.WJetsToLNu_HT-1200To2500_TuneCP5_13TeV-madgraphMLM-pythia8",
    "Firefighter.ffConfig.production.Autumn18.bkgmc.WJetsToLNu_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8",
    "Firefighter.ffConfig.production.Autumn18.bkgmc.WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8",
    "Firefighter.ffConfig.production.Autumn18.bkgmc.WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8",
    "Firefighter.ffConfig.production.Autumn18.bkgmc.WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8",
    "Firefighter.ffConfig.production.Autumn18.bkgmc.WJetsToLNu_HT-70To100_TuneCP5_13TeV-madgraphMLM-pythia8",
    "Firefighter.ffConfig.production.Autumn18.bkgmc.WJetsToLNu_HT-800To1200_TuneCP5_13TeV-madgraphMLM-pythia8",
] # all backgrounds
print(*tosubd_, sep='\n')


def main():

    from Firefighter.ffConfig.condorConfigBuilder import configBuilder
    from Firefighter.piedpiper.utils import get_voms_certificate

    os.system(
        "tar -X EXCLUDEPATTERNS --exclude-vcs -zcf ${CMSSW_VERSION}.tar.gz -C ${CMSSW_BASE}/.. ${CMSSW_VERSION}"
    )
    get_voms_certificate()

    for ds in tosubd_:
        tosubdff = importlib.import_module(ds).ffDataSet

        cb = configBuilder(tosubdff, eventRegion="all")
        for c in cb.build():
            configBuilder.submit(c)


if __name__ == "__main__":
    print(">>> I am Mr. ffBatchJobSubmitter <<<")
    main()
