#!/usr/bin/env python
"""submit job for a single dataset
"""
from __future__ import print_function

import importlib
import os
from os.path import join

PRODUCTIONBASE = join(
    os.getenv("CMSSW_BASE"), "src/Firefighter/ffConfig/python/production/"
)

# tosubd_ = "Firefighter.ffConfig.production.Autumn18.data.DoubleMuon_Run2018A"
tosubd_ = "Firefighter.ffConfig.production.Autumn18.bkgmc.TTJets_TuneCP5_13TeV-madgraphMLM-pythia8"
print(tosubd_)


def main():
    tosubdff = importlib.import_module(tosubd_).ffDataSet

    from Firefighter.ffConfig.condorConfigBuilder import configBuilder
    from Firefighter.piedpiper.utils import get_voms_certificate

    os.system(
        "tar -X EXCLUDEPATTERNS --exclude-vcs -zcf ${CMSSW_VERSION}.tar.gz -C ${CMSSW_BASE}/.. ${CMSSW_VERSION}"
    )
    get_voms_certificate()

    cb = configBuilder(tosubdff, eventRegion="all")
    for c in cb.build():
        configBuilder.submit(c)


if __name__ == "__main__":
    print(">>> I am Mr. ffSingleJobSubmitter <<<")
    main()
