#!/usr/bin/env python
"""submit jobs in group of *data*, *bkg*, or *sig*

default submission choice is condor first, for dataset not available on disk,
will fall back to crab. If submitter argument option is provided, it will be override.

usage: python ffGrandJobSubmitter.py sigmc -s crab
   or, python ffGrandJobSubmitter.py -h
"""
from __future__ import print_function

import argparse
import json
import os, sys, time
from os.path import join

import yaml
from Firefighter.ffConfig.datasetUtils import query_yes_no

PRODUCTIONBASE = join(os.getenv("CMSSW_BASE"), "src/Firefighter/ffConfig/python/production/")
DATA_L = json.load(open(join(PRODUCTIONBASE, "Autumn18/data/description.json")))
BKGMC_L = json.load(open(join(PRODUCTIONBASE, "Autumn18/bkgmc/description.json")))
SIGMC_L = json.load(open(join(PRODUCTIONBASE, "Autumn18/sigmc/private/description.json")))

## parser
parser = argparse.ArgumentParser(description="Submit jobs in a GRAND way.")
parser.add_argument("datasettype", type=str, nargs="*", choices=["sigmc", "bkgmc", "data"], help="Type of dataset",)
parser.add_argument("--submitter", "-s", default="condor", type=str, choices=["condor", "crab"])
parser.add_argument("--ignorelocality", dest='ignorelocality', action='store_true', help='Only take effect when submit with crab, ignore locality.')
parser.add_argument("--enforcelocality", dest='ignorelocality', action='store_false', help='Only take effect when submit with crab, enforce locality. DEFAULT')
parser.set_defaults(ignorelocality=False)
parser.add_argument("--jobtype", "-t", default="ntuple", type=str, choices=["ntuple", "skim", "ntuplefromskim"])
parser.add_argument("--eventregion", "-r", default="all", type=str, choices=["all", "signal", "control", "proxy", "muonType"])
args = parser.parse_args()

## modify data source dir if run with skimmed AOD
if args.jobtype == 'ntuplefromskim':
    if args.datasettype == 'sigmc':
        sys.exit('No sigmc skimmed files available. -wsi 11/01/19')
    if args.submitter == 'crab':
        sys.exit('Skimmed source can only be run with condor. (no real dataset name in DAS)')
    if args.eventregion != 'signal':
        sys.exit('Skimmed source is prepared for *signal* event region.')
    DATA_L = json.load(open(join(PRODUCTIONBASE, "Skim2LJ18/data/description.json")))
    BKGMC_L = json.load(open(join(PRODUCTIONBASE, "Skim2LJ18/bkgmc/description.json")))
    # if args.eventregion!='all':
    #     if args.eventregion=='proxy' and len(args.datasettype)==1 and 'data' in args.datasettype:
    #         DATA_L = json.load(open(join(PRODUCTIONBASE, "SkimProxy18/data/description.json")))
    #     else:
    #         sys.exit('ntupleforskim for non-*all* for non-*data* is not available yet. - wsi 6/1/20')

## all datasets
ffds = {
    "data":  [yaml.load(open(join(os.getenv('CMSSW_BASE'), m)), Loader=yaml.Loader) for m in DATA_L],
    "bkgmc": [yaml.load(open(join(os.getenv('CMSSW_BASE'), m)), Loader=yaml.Loader) for m in BKGMC_L],
    "sigmc": [yaml.load(open(join(os.getenv('CMSSW_BASE'), m)), Loader=yaml.Loader) for m in SIGMC_L],
}

# ------------------------------------------------------------------------------


def buildbanner(msg):
    """make a banner like:

        #########################################
        ###    submit sigmc jobs to condor    ###
        #########################################

    """

    midmsg = "###    {}    ###".format(msg)
    headtail = "#" * len(midmsg)
    return "\n".join([headtail, midmsg, headtail])


# ------------------------------------------------------------------------------


def submit(dkind, submitter="condor", jobtype="ntuple"):

    ## common kwargs for config builders
    commonCBkwargs = dict(
        ffConfigName='ffNtupleFromAOD_v2_cfg.py',
        outbase='/store/group/lpcmetx/SIDM/ffNtupleV2/',
    )
    if jobtype == 'skim':
        commonCBkwargs = dict(
            ffConfigName='ffFullSkimFromAOD_cfg.py',
            outbase='/store/group/lpcmetx/SIDM/Skim/',
        )
    if jobtype == 'ntuplefromskim':
        commonCBkwargs['unitsPerJob'] = 50
        commonCBkwargs['outbase'] = '/store/group/lpcmetx/SIDM/ffNtupleV2/Skim/'
    if args.submitter == 'crab' and args.ignorelocality == False:
        commonCBkwargs['ignoreLocality'] = False

    _eventregion = args.eventregion #"all"
    if _eventregion in ['proxy', 'muonType']:
        commonCBkwargs['outbase'] += '{}/'.format(_eventregion)

    print('*** common config builder kwargs ***')
    for k, v in commonCBkwargs.items():
        print('{:20}{}'.format(k,v))
    print('*** **************************** ***')

    if not query_yes_no('Is args set correctly?'):
        sys.exit('No? Okay, exiting..')

    ## split by submitter
    if submitter == "crab":
        from Firefighter.ffConfig.crabConfigBuilder import configBuilder as CrabCB

        coremsg = "submit {} jobs for {} to crab".format(jobtype, dkind)
        print(buildbanner(coremsg))

        for i, ds in enumerate(ffds[dkind]):
            print("----> submitting {0}/{1}".format(i+1, len(ffds[dkind])))
            cb = CrabCB(ds, eventRegion=_eventregion, **commonCBkwargs)
            for c in cb.build():
                CrabCB.submit(c)

    elif submitter == "condor":
        # submit with condor, for those not available on disk, submit to crab
        # to trigger transfer, dump those dataset as a text file
        from Firefighter.ffConfig.condorConfigBuilder import configBuilder as CondorCB
        from Firefighter.ffConfig.crabConfigBuilder import configBuilder as CrabCB
        from Firefighter.ffConfig.datasetUtils import get_storageSites
        from Firefighter.piedpiper.utils import get_voms_certificate

        coremsg = "submit {} jobs for {} to condor(crab)".format(jobtype, dkind)
        print(buildbanner(coremsg))

        os.system("cd $CMSSW_BASE/src/Firefighter && scram b -j12 && cd -")
        os.system("tar -X EXCLUDEPATTERNS --exclude-vcs -zcf `basename ${CMSSW_BASE}`.tar.gz -C ${CMSSW_BASE}/.. `basename ${CMSSW_BASE}`")
        os.system("cd $CMSSW_BASE/src/Firefighter && scram b -j12 && cd -") # ensure tarball is copied to $CMSSW_base/python
        get_voms_certificate()

        if dkind == "bkgmc":  # only background mc has dataset not on disk
            diskGhosts = []
            for i, ds in enumerate(ffds[dkind]):
                print("----> submitting {0}/{1}".format(i+1, len(ffds[dkind])))
                if args.jobtype=='ntuplefromskim' or any(map(lambda d: get_storageSites(d), ds["datasetNames"])):  # condor, this way
                    cb = CondorCB(ds, eventRegion=_eventregion, **commonCBkwargs)
                    for c in cb.build():
                        CondorCB.submit(c)
                else:  # crab, this way
                    diskGhosts.append(BKGMC_L[i])
                    cb = CrabCB(ds, eventRegion=_eventregion, **commonCBkwargs)
                    for c in cb.build():
                        CrabCB.submit(c)
            if diskGhosts:
                print("@@@ {0} bkgmc ffDataSets not available on disk, thus submitted to CRAB.".format(len(diskGhosts)))
                for ffd in diskGhosts:
                    print("@", ffd)
                logfn = "diskGhosts_{}.log".format(time.strftime("%y%m%d"))
                with open(logfn, "w") as outf:
                    outf.write(json.dumps(diskGhosts, indent=4))
                print("@@@ saved at {}.".format(logfn))
        else:
            for i, ds in enumerate(ffds[dkind]):
                print("----> submitting {0}/{1}".format(i+1, len(ffds[dkind])))
                cb = CondorCB(ds, eventRegion=_eventregion, **commonCBkwargs)
                for c in cb.build():
                    CondorCB.submit(c)
    else:
        # you should not step into this darkness gently
        sys.exit("submitter *{}* unknown. exit!".format(submitter))


if __name__ == "__main__":
    print(args)

    print(" I am Mr. ffGrandJobSubmitter ".center(79, '+'))
    for d in args.datasettype:
        submit(d, submitter=args.submitter, jobtype=args.jobtype)
