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
import os
import sys
import time
from os.path import join

import yaml

PRODUCTIONBASE = join(os.getenv("CMSSW_BASE"), "src/Firefighter/ffConfig/python/production/")
DATA_L = json.load(open(join(PRODUCTIONBASE, "Autumn18/data/description.json")))
BKGMC_L = json.load(open(join(PRODUCTIONBASE, "Autumn18/bkgmc/description.json")))
SIGMC_L = json.load(open(join(PRODUCTIONBASE, "Autumn18/sigmc/private/description.json")))

## parser
parser = argparse.ArgumentParser(description="Submit jobs in a GRAND way.")
parser.add_argument("datasettype", type=str, nargs="*",
                    choices=["sigmc", "bkgmc", "data"],
                    help="Type of dataset",)
parser.add_argument("--submitter", "-s", default="condor", type=str,
                    choices=["condor", "crab"])
parser.add_argument("--jobtype", "-t", default="ntuple", type=str, choices=["ntuple", "skim", "ntuplefromskim"])
args = parser.parse_args()

## modify data source dir if run with skimmed AOD
if args.jobtype == 'ntuplefromskim':
    if args.datasettype == 'sigmc':
        sys.exit('No sigmc skimmed files available. -wsi 11/01/19')
    if args.submitter == 'crab':
        sys.exit('Skimmed source can only be run with condor. (no real dataset name in DAS)')
    DATA_L = json.load(open(join(PRODUCTIONBASE, "Skim2LJ18/data/description.json")))
    BKGMC_L = json.load(open(join(PRODUCTIONBASE, "Skim2LJ18/bkgmc/description.json")))

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

    # common kwargs for config builders
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

    if submitter == "crab":
        from Firefighter.ffConfig.crabConfigBuilder import configBuilder as CrabCB

        coremsg = "submit {} jobs for {} to crab".format(jobtype, dkind)
        print(buildbanner(coremsg))

        _eventregion = "all"
        if dkind == "data":
            _eventregion = "control"  # only control for data now.

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

        _eventregion = "all"
        if dkind == "data":
            _eventregion = "control"  # only control for data now.

        os.system("tar -X EXCLUDEPATTERNS --exclude-vcs -zcf `basename ${CMSSW_BASE}`.tar.gz -C ${CMSSW_BASE}/.. `basename ${CMSSW_BASE}`")
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

    print(" I am Mr. ffGrandJobSubmitter ".center(79, '+'))
    for d in args.datasettype:
        submit(d, submitter=args.submitter, jobtype=args.jobtype)
