#!/usr/bin/env python
from __future__ import print_function

import os
import sys
import time

import yaml
from CRABAPI.RawCommand import crabCommand
from crabConfig_2 import *
from Firefighter.piedpiper.utils import *

doCmd = True
CONFIG_NAME = sys.argv[1]
assert os.path.isfile(CONFIG_NAME)


def main():

    # safety check
    if os.environ["CMSSW_BASE"] not in os.path.abspath(__file__):
        print("$CMSSW_BASE: ", os.environ["CMSSW_BASE"])
        print("__file__: ", os.path.abspath(__file__))
        sys.exit("Inconsistant release environment!")

    # load config
    multiconf = yaml.load(open(CONFIG_NAME).read())

    inputdatasets = multiconf["premixdigihltdatasets"]
    year = multiconf["year"]
    config.Data.outLFNDirBase = "/store/group/lpcmetx/MCSIDM/AODSIM/{0}".format(year)

    memreq = 15100 if year == 2018 else 6000
    config.JobType.maxMemoryMB = memreq
    config.JobType.numCores = 4 if year == 2016 else 8

    donelist = list()
    for ds in inputdatasets:

        print("dataset: ", ds)
        config.Data.inputDataset = ds

        ## outputDatasetTag: mXX-100_mA-5_lxy-0p3_ctau-0p375_AODSIM_2018
        nametag = "-".join(ds.split("/")[2].split("-")[1:-1])
        nametag = nametag.replace("PREMIXRAWHLT", "AODSIM")
        config.Data.outputDatasetTag = nametag

        ## requestName
        primaryDatasetTag = ds.split("/")[1]
        config.General.requestName = "_".join(
            [primaryDatasetTag, config.Data.outputDatasetTag, time.strftime("%y%m%d-%H%M%S")]
        )

        if doCmd:
            crabCommand("submit", config=config)
            donelist.append(ds)

    print("submitted: ", len(donelist))
    for x in donelist:
        print(x)
    print("------------------------------------------------------------")

    undonelist = [x for x in inputdatasets if x not in donelist]
    print("unsubmitted: ", len(undonelist))
    for x in undonelist:
        print(x)
    if undonelist:
        with open("unsubmitted-2.yml.log", "w") as outf:
            yaml.dump(
                {"premixdigihltdatasets": undonelist, "year": year}, outf, default_flow_style=False
            )


if __name__ == "__main__":
    main()
