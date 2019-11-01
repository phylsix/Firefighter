#!/usr/bin/env python
"""submit jobs for a list of dataset
$cmd: python ffBatchJobSubmitter.py batchdatasets.yml -s crab -t ntuple
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
parser.add_argument("--jobtype", "-t", default="ntuple", type=str, choices=["ntuple", "skim"])
args = parser.parse_args()
assert(os.path.exists(args.datasets[0]))

tosubd_ = yaml.load(open(args.datasets[0]), Loader=yaml.Loader)

print("++ submit jobs for:")
print(*tosubd_, sep="\n")
print("++ submit jobs with:\t", args.submitter)
print("++ submit jobs type:\t", args.jobtype)


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

    for ds in tosubd_:
        tosubdff = yaml.load(open(join(os.getenv('CMSSW_BASE'), ds)), Loader=yaml.Loader)

        eventRegion_ = 'control'
        if 'sigmc' in ds or 'bkgmc' in ds:
            eventRegion_ = 'all'
        cb = configBuilder(tosubdff,
                           eventRegion=eventRegion_,
                           ffConfigName='ffNtupleFromAOD_v2_cfg.py',
                           outbase='/store/group/lpcmetx/SIDM/ffNtupleV2/',)
        if args.jobtype == 'skim':
            cb = configBuilder(tosubdff,
                               eventRegion=eventRegion_,
                               ffConfigName='ffFullSkimFromAOD_cfg.py',
                               outbase='/store/group/lpcmetx/SIDM/Skim/',)
        for c in cb.build():
            configBuilder.submit(c)


if __name__ == "__main__":
    print(" I am Mr. ffBatchJobSubmitter ".center(79, '+'))
    main()
