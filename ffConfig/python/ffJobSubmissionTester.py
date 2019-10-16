#!/usr/bin/env python
from __future__ import print_function
import os
from os.path import join
import json

import yaml

PRODUCTIONBASE = join(os.getenv("CMSSW_BASE"), "src/Firefighter/ffConfig/python/production/")

DATA_L = json.load(open(join(PRODUCTIONBASE, "Autumn18/data/description.json")))
BKGMC_L = json.load(open(join(PRODUCTIONBASE, "Autumn18/bkgmc/description.json")))
SIGMC_L = json.load(open(join(PRODUCTIONBASE, "Autumn18/sigmc/private/description.json")))

ffds_data  = [yaml.load(open(join(os.getenv('CMSSW_BASE'), m)), Loader=yaml.Loader) for m in DATA_L],
ffds_bkgmc = [yaml.load(open(join(os.getenv('CMSSW_BASE'), m)), Loader=yaml.Loader) for m in BKGMC_L],
ffds_sigmc = [yaml.load(open(join(os.getenv('CMSSW_BASE'), m)), Loader=yaml.Loader) for m in SIGMC_L],

# print(ffds_data[0].keys())
# print(ffds_bkgmc[0].keys())
# print(ffds_sigmc[0].keys())


def test_crab():
    print("### test crab ###\n")
    from Firefighter.ffConfig.crabConfigBuilder import configBuilder

    ## test data ##
    test_dataset = ffds_data[0]
    cb = configBuilder(test_dataset)
    print("\n### test data ###")
    for c in cb.build():
        print(c)

    ## test bkgmc ##
    test_dataset = ffds_bkgmc[0]
    cb = configBuilder(test_dataset)
    print("\n### test bkgmc ###")
    for c in cb.build():
        print(c)

    ## test sigmc ##
    test_dataset = ffds_sigmc[0]
    cb = configBuilder(test_dataset)
    print("\n### test sigmc ###")
    for c in cb.build():
        print(c)


def test_condor():
    print("### test condor ###\n")
    from Firefighter.ffConfig.condorConfigBuilder import configBuilder

    ## test data ##
    test_dataset = ffds_data[0]
    cb = configBuilder(test_dataset)
    print("\n### test data ###")
    for c in cb.build():
        print(open(c).read())

    ## test bkgmc ##
    test_dataset = ffds_bkgmc[0]
    cb = configBuilder(test_dataset)
    print("\n### test bkgmc ###")
    for c in cb.build():
        print(open(c).read())

    ## test sigmc ##
    test_dataset = ffds_sigmc[0]
    cb = configBuilder(test_dataset)
    print("\n### test sigmc ###")
    for c in cb.build():
        print(open(c).read())


def test_condor_submit():
    from Firefighter.ffConfig.condorConfigBuilder import configBuilder
    from Firefighter.piedpiper.utils import get_voms_certificate

    # test_dataset = ffds_sigmc[0]
    # test_dataset = ffds_bkgmc[0]
    # test_dataset = ffds_data[2]

    test_dataset["maxEvents"] = 100
    cb = configBuilder(
        test_dataset, outbase="/store/user/wsi/SIDM/ffNtuple/test/", eventRegion="all"
    )
    print("\n### test condor submit ###")

    os.system(
        "tar -X EXCLUDEPATTERNS --exclude-vcs -zcf `basename ${CMSSW_BASE}`.tar.gz -C ${CMSSW_BASE}/.. `basename ${CMSSW_BASE}`"
    )
    get_voms_certificate()

    for c in cb.build():
        print(open(c).read())
        configBuilder.submit(c)


def test_crab_submit():
    from Firefighter.ffConfig.crabConfigBuilder import configBuilder

    # test_dataset = ffds_sigmc[0]
    test_dataset = ffds_bkgmc[0]
    test_dataset["maxEvents"] = 100
    cb = configBuilder(
        test_dataset, outbase="/store/user/wsi/SIDM/ffNtuple/test/", eventRegion="all"
    )
    print("\n### test crab submit ###")
    for c in cb.build():
        print(c)
        configBuilder.submit(c)


if __name__ == "__main__":
    print(">>> I am Mr. ffJobSumissionTester <<<")
    # test_crab()
    # test_condor()
    # test_condor_submit()
    # test_crab_submit()
