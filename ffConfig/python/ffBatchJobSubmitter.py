#!/usr/bin/env python
"""submit jobs for a list of dataset
$cmd: python ffBatchJobSubmitter.py batchdatasets.json -s crab
"""
from __future__ import print_function

import argparse
import importlib
import os
from os.path import join
import json

PRODUCTIONBASE = join(os.getenv("CMSSW_BASE"), "src/Firefighter/ffConfig/python/production/")

## parser
parser = argparse.ArgumentParser(description="Submit many datasets in a BATCH.")
parser.add_argument("datasets", type=str, nargs=1, help='JSON contains list of datasets')
parser.add_argument("--submitter", "-s", default="condor", type=str, choices=["condor", "crab"])
args = parser.parse_args()
assert(os.path.exists(args.datasets[0]))

tosubd_ = json.load(open(args.datasets[0]))
print(*tosubd_, sep="\n")


def main():

    if args.submitter == 'condor':
        from Firefighter.ffConfig.condorConfigBuilder import configBuilder
        from Firefighter.piedpiper.utils import get_voms_certificate

        os.system(
            "tar -X EXCLUDEPATTERNS --exclude-vcs -zcf ${CMSSW_VERSION}.tar.gz -C ${CMSSW_BASE}/.. ${CMSSW_VERSION}"
        )
        get_voms_certificate()
    elif args.submitter == 'crab':
        from Firefighter.ffConfig.crabConfigBuilder import configBuilder

    for ds in tosubd_:
        tosubdff = importlib.import_module(ds).ffDataSet

        cb = configBuilder(tosubdff, eventRegion="all")
        for c in cb.build():
            configBuilder.submit(c)


if __name__ == "__main__":
    print(">>> I am Mr. ffBatchJobSubmitter <<<")
    main()
