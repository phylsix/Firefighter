#!/usr/bin/env python
from __future__ import print_function
import os
import sys
from os.path import join
import time
import json
import importlib

PRODUCTIONBASE = join(
    os.getenv("CMSSW_BASE"), "src/Firefighter/ffConfig/python/production/"
)

DATA_L = json.load(open(join(PRODUCTIONBASE, "Autumn18/data/description.json")))
BKGMC_L = json.load(open(join(PRODUCTIONBASE, "Autumn18/bkgmc/description.json")))
SIGMC_L = json.load(
    open(join(PRODUCTIONBASE, "Autumn18/sigmc/private/description.json"))
)

ffds_data = [importlib.import_module(m).ffDataSet for m in DATA_L]
ffds_bkgmc = [importlib.import_module(m).ffDataSet for m in BKGMC_L]
ffds_sigmc = [importlib.import_module(m).ffDataSet for m in SIGMC_L]


def submit(dkind):
    dkind = dkind.lower()
    assert dkind in ["sig", "bkg", "data"]

    if dkind == "sig":
        # submit with condor
        from Firefighter.ffConfig.condorConfigBuilder import configBuilder
        from Firefighter.piedpiper.utils import get_voms_certificate

        print("#########################################")
        print("###    submit sigmc jobs to condor    ###")
        print("#########################################")

        os.system(
            "tar -X EXCLUDEPATTERNS --exclude-vcs -zcf ${CMSSW_VERSION}.tar.gz -C ${CMSSW_BASE}/.. ${CMSSW_VERSION}"
        )
        get_voms_certificate()

        for ds in ffds_sigmc:
            cb = configBuilder(ds, eventRegion="all")
            for c in cb.build():
                configBuilder.submit(c)

    elif dkind == "bkg":
        # submit with condor, for those not available on disk, submit to crab
        # to trigger transfer, dump those dataset as a text file
        from Firefighter.ffConfig.condorConfigBuilder import configBuilder as CondorCB
        from Firefighter.ffConfig.crabConfigBuilder import configBuilder as CrabCB
        from Firefighter.ffConfig.datasetUtils import get_storageSites

        print("#########################################")
        print("### submit bkgmc jobs to condor/crab  ###")
        print("#########################################")

        os.system(
            "tar -X EXCLUDEPATTERNS --exclude-vcs -zcf ${CMSSW_VERSION}.tar.gz -C ${CMSSW_BASE}/.. ${CMSSW_VERSION}"
        )
        get_voms_certificate()

        diskGhosts = []
        for i, ds in enumerate(ffds_bkgmc):
            if any(
                map(lambda d: get_storageSites(d), ds["datasetNames"])
            ):  # condor, this way
                cb = CondorCB(ds, eventRegion="all")
                for c in cb.build():
                    CondorCB.submit(c)
            else:  # crab, this way
                diskGhosts.append(BKGMC_L[i])
                cb = CrabCB(ds, eventRegion="all")
                for c in cb.build():
                    CrabCB.submit(c)

        if diskGhosts:
            print(
                "@@@ {0} bkgmc ffDataSets not available on disk, thus submitted to CRAB.".format(
                    len(diskGhosts)
                )
            )
            for ffd in diskGhosts:
                print("@", ffd)
            logfn = "diskGhosts_{}.log".format(time.strftime("%y%m%d"))
            with open(logfn, "w") as outf:
                outf.write(json.dumps(diskGhosts))
            print("@@@ saved at {}.".format(logfn))

    elif dkind == "data":
        # submit with condor
        from Firefighter.ffConfig.condorConfigBuilder import configBuilder
        from Firefighter.piedpiper.utils import get_voms_certificate

        print("#########################################")
        print("###    submit data jobs to condor    ###")
        print("#########################################")

        os.system(
            "tar -X EXCLUDEPATTERNS --exclude-vcs -zcf ${CMSSW_VERSION}.tar.gz -C ${CMSSW_BASE}/.. ${CMSSW_VERSION}"
        )
        get_voms_certificate()

        for ds in ffds_data:
            cb = configBuilder(ds, eventRegion="control")
            for c in cb.build():
                configBuilder.submit(c)

    else:
        # you should not step into this darkness gently
        sys.exit("dkind *{}* unknown. exit!".format(dkind))


if __name__ == "__main__":
    print("ffJobSubmitter is working..")
    submit(sys.argv[1])
