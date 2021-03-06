#!/usr/bin/env python
"""submit jobs for a list of dataset, demo for trigger studies
$cmd: python ffBatchJobSubmitter_trigger.py batchdatasets.yml -s crab
"""
from __future__ import print_function

import argparse
import os
from os.path import join

import yaml


## parser
parser = argparse.ArgumentParser(description="Submit many datasets in a BATCH.")
parser.add_argument("datasets", type=str, nargs=1, help='YAML contains list of datasets')
parser.add_argument("--submitter", "-s", default="condor", type=str, choices=["condor", "crab"])
args = parser.parse_args()
assert(os.path.exists(args.datasets[0]))

tosubd_ = yaml.load(open(args.datasets[0]), Loader=yaml.Loader)
print(*tosubd_, sep="\n")


def main():

    if args.submitter == 'condor':
        from Firefighter.ffConfig.condorConfigBuilder import configBuilder
        from Firefighter.piedpiper.utils import get_voms_certificate

        os.system(
            "tar -X EXCLUDEPATTERNS --exclude-vcs -zcf `basename ${CMSSW_BASE}`.tar.gz -C ${CMSSW_BASE}/.. `basename ${CMSSW_BASE}`"
        )
        get_voms_certificate()
    elif args.submitter == 'crab':
        from Firefighter.ffConfig.crabConfigBuilder import configBuilder

    ## customize kwargs for configBuilder
    customize = dict(
        eventRegion='all',
        ffConfigName='ffNtupleFromAOD_trigger_cfg.py',
        outbase="/store/group/lpcmetx/SIDM/ffNtuple4trigger/",
        denomTriggerPaths=['HLT_Mu17', 'HLT_IsoMu24'],
    )

    for ds in tosubd_:
        tosubdff = yaml.load(open(join(os.getenv("CMSSW_BASE"), ds)), Loader=yaml.Loader)

        cb = configBuilder(tosubdff, **customize)
        for c in cb.build():
            configBuilder.submit(c)


if __name__ == "__main__":
    print(">>> I am Mr. ffBatchJobSubmitter (wearing a trigger hat) <<<")
    main()
