#!/usr/bin/env python
"""submit jobs for a list of dataset
$cmd: python ffBatchJobSubmitter.py batchdatasets.yml -s crab -t ntuple
"""
from __future__ import print_function

import argparse
import os, time
from os.path import join

import yaml


## parser
parser = argparse.ArgumentParser(description="Submit many datasets in a BATCH.")
parser.add_argument("datasets", type=str, nargs=1, help='YAML contains list of datasets')
parser.add_argument("--submitter", "-s", default="condor", type=str, choices=["condor", "crab"])
parser.add_argument("--ignorelocality", dest='ignorelocality', action='store_true', help='Only take effect when submit with crab, ignore locality.')
parser.add_argument("--enforcelocality", dest='ignorelocality', action='store_false', help='Only take effect when submit with crab, enforce locality. DEFAULT')
parser.set_defaults(ignorelocality=False)
parser.add_argument("--jobtype", "-t", default="ntuple", type=str, choices=["ntuple", "skim", "ntuplefromskim", "dummy"])
parser.add_argument("--eventregion", "-r", default="all", type=str, choices=["all", "single", "signal", "control", "proxy", "muonType"])
args = parser.parse_args()
assert(os.path.exists(args.datasets[0]))

tosubd_ = yaml.load(open(args.datasets[0]), Loader=yaml.Loader)

print("++ submit jobs for:")
print(*tosubd_, sep="\n")
print("++ submit jobs with:\t", args.submitter)
print("++ submit jobs type:\t", args.jobtype)
print("++ submit jobs eventRegion:\t", args.eventregion)


def main():

    if args.submitter == 'condor':
        from Firefighter.ffConfig.condorConfigBuilder import configBuilder
        from Firefighter.piedpiper.utils import get_voms_certificate

        os.system("cd $CMSSW_BASE/src/Firefighter && scram b -j12 && cd -")
        os.system("tar -X EXCLUDEPATTERNS --exclude-vcs -zcf `basename ${CMSSW_BASE}`.tar.gz -C ${CMSSW_BASE}/.. `basename ${CMSSW_BASE}`")
        os.system("cd $CMSSW_BASE/src/Firefighter && scram b -j12 && cd -") # ensure tarball is copied to $CMSSW_base/python
        get_voms_certificate()
    elif args.submitter == 'crab':
        from Firefighter.ffConfig.crabConfigBuilder import configBuilder


    ## set event region
    eventRegion_ = args.eventregion

    ## base submitter config settings
    cbkwargs = dict(
        eventRegion=eventRegion_,
        ffConfigName='ffNtupleFromAOD_v2_cfg.py',
        outbase='/store/group/lpcmetx/SIDM/ffNtupleV2/',
        unitsPerJob=1,
    )
    ## update CMSSW config file and outputbase for skim jobs
    if args.jobtype == 'skim':
        cbkwargs.update(dict(
            ffConfigName='ffFullSkimFromAOD_cfg.py',
            outbase='/store/group/lpcmetx/SIDM/Skim/',
            ))
    if eventRegion_ in ['proxy', 'muonType']:
        cbkwargs['outbase'] += '{}/'.format(eventRegion_)

    ## update unitsPerJob for ntuple jobs with skimmed files as source
    if args.jobtype == 'ntuplefromskim':
        cbkwargs['unitsPerJob'] = 50
        cbkwargs['outbase'] = '/store/group/lpcmetx/SIDM/ffNtupleV2/Skim/'
    if args.submitter == 'crab' and args.ignorelocality == False:
        cbkwargs['ignoreLocality'] = False

    ## dummy jobs
    if args.jobtype == 'dummy':
        cbkwargs = dict(
            ffConfigName='ffNtupleDummy_cfg.py',
            maxMemory=2000,
            workArea=join(
                os.getenv("CMSSW_BASE"),
                "src/Firefighter/ffConfig/crabGarage/dummyjobs",
                time.strftime("%y%m%d"),
            ),
            outbase="/store/group/lpcmetx/SIDM/dummy/",
        )

    for i, ds in enumerate(tosubd_, start=1):
        print('[{}/{}]'.format(i, len(tosubd_)))
        tosubdff = yaml.load(open(join(os.getenv('CMSSW_BASE'), ds)), Loader=yaml.Loader)

        cb = configBuilder(tosubdff, **cbkwargs)
        for c in cb.build():
            configBuilder.submit(c)


if __name__ == "__main__":
    print(" I am Mr. ffBatchJobSubmitter ".center(79, '+'))
    main()
