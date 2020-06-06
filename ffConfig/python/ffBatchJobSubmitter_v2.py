#!/usr/bin/env python
"""submit jobs for a list of dataset
$cmd: python ffBatchJobSubmitter.py batchdatasets.yml -s crab -t ntuple
"""
from __future__ import print_function

import argparse
import os, sys, time
from os.path import join

import yaml
from Firefighter.ffConfig.datasetUtils import query_yes_no


## parser
parser = argparse.ArgumentParser(description="Submit many datasets in a BATCH.")
parser.add_argument("datasets", type=str, nargs=1, help='YAML contains list of datasets')
parser.add_argument("--submitter", "-s", default="condor", type=str, choices=["condor", "crab"])
parser.add_argument("--ignorelocality", dest='ignorelocality', action='store_true', help='Only take effect when submit with crab, ignore locality.')
parser.add_argument("--enforcelocality", dest='ignorelocality', action='store_false', help='Only take effect when submit with crab, enforce locality. DEFAULT')
parser.set_defaults(ignorelocality=False)
parser.add_argument("--jobtype", "-t", default="ntuple", type=str, choices=["ntuple", "skim", "ntuplefromskim", "dummy"])
parser.add_argument("--eventregion", "-r", default="all", type=str, choices=["all", "single", "signal", "proxy", "muonType"])
parser.add_argument("--unitsperjob", "-u", default=1, type=int)
args = parser.parse_args()
assert(os.path.exists(args.datasets[0]))

tosubd_ = yaml.load(open(args.datasets[0]), Loader=yaml.Loader)

print("++ submit jobs for:")
print(*['    '+s for s in tosubd_], sep="\n")
print("{:30}{}".format("++ submit jobs with:", args.submitter))
print("{:30}{}".format("++ submit jobs type:", args.jobtype))
print("{:30}{}".format("++ submit jobs eventRegion:", args.eventregion))
print("{:30}{}".format("++ submit jobs with unit:", str(args.unitsperjob)+' (ntuplefromskim will be overwritten to 50)'))


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
        unitsPerJob=args.unitsperjob,
    )
    ## update CMSSW config file and outputbase for skim jobs
    if args.jobtype == 'skim':
        cbkwargs.update(dict(
            ffConfigName='ffFullSkimFromAOD_cfg.py',
            outbase='/store/group/lpcmetx/SIDM/Skim/',
            ))
    ## update unitsPerJob for ntuple jobs with skimmed files as source
    if args.jobtype == 'ntuplefromskim':
        cbkwargs['unitsPerJob'] = 50
        cbkwargs['outbase'] = '/store/group/lpcmetx/SIDM/ffNtupleV2/Skim/'

    if args.submitter == 'crab' and args.ignorelocality == False:
        cbkwargs['ignoreLocality'] = False

    if eventRegion_ in ['proxy', 'muonType']:
        cbkwargs['outbase'] += '{}/'.format(eventRegion_)

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
    print(args)
    if not query_yes_no('Is args set correctly?'):
        sys.exit('No? Okay, exiting..')

    print(" I am Mr. ffBatchJobSubmitter ".center(79, '+'))
    main()
