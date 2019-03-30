#!/usr/bin/env python
from __future__ import print_function
import os
import yaml
import time

from CRABAPI.RawCommand import crabCommand
from Firefighter.piedpiper.utils import *
from crabConfig_1 import *

doCmd = True
CONFIG_NAME = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "multicrabConfig-1.yml"
)


def main():

    # safety check
    if os.environ["CMSSW_BASE"] not in os.path.abspath(__file__):
        print("$CMSSW_BASE: ", os.environ["CMSSW_BASE"])
        print("__file__: ", os.path.abspath(__file__))
        sys.exit("Inconsistant release environment!")

    # load config
    multiconf = yaml.load(open(CONFIG_NAME).read())

    inputdatasets = multiconf["gensimdatasets"]
    year = multiconf["year"]
    # config.Data.outLFNDirBase += '/{0}'.format(year)
    config.Data.outLFNDirBase = "/store/group/lpcmetx/MCSIDM/PREMIXRAWHLT/{0}".format(
        year
    )

    memreq = 15100 if year == 2018 else 6000
    config.JobType.maxMemoryMB = memreq
    config.JobType.numCores = 4 if year == 2016 else 8

    donelist = list()
    for ds in inputdatasets:

        mxx, ma, ctau = get_param_from_dataset(ds)
        nametag = "mXX-{0}_mA-{1}_ctau-{2}_PREMIXRAWHLT_{3}".format(
            floatpfy(mxx), floatpfy(ma), floatpfy(ctau), year
        )
        # this is fix for previous non-careful naming convention
        if "CRAB_PrivateMC" in ds:
            pd = ds.split("/")[-2].split("_")[1].replace("Bs", "XX").replace("Dp", "A")
            nametag = pd + "_" + nametag
        print("dataset: ", ds)
        print("nametag: ", nametag)
        config.Data.inputDataset = ds
        config.Data.outputDatasetTag = nametag
        config.General.requestName = "_".join(
            ["PREMIXRAWHLT", str(year), nametag, time.strftime("%y%m%d-%H%M%S")]
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
        with open("unsubmitted-1.yml.log", "w") as outf:
            yaml.dump(
                {"gensimdatasets": undonelist, "year": year},
                outf,
                default_flow_style=False,
            )


if __name__ == "__main__":
    main()
