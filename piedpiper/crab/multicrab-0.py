#!/usr/bin/env python
from __future__ import print_function

import os
import yaml
import time

from CRABAPI.RawCommand import crabCommand

from Firefighter.piedpiper.utils import *
from crabConfig_0 import *

verbose = False
alwaysDoCmd = True

if os.environ["CMSSW_BASE"] not in os.path.abspath(__file__):
    print("$CMSSW_BASE: ", os.environ["CMSSW_BASE"])
    print("__file__: ", os.path.abspath(__file__))
    sys.exit("Inconsistant release environment!")

BASEDIR = os.path.join(os.environ["CMSSW_BASE"], "src/Firefighter/piedpiper")
print(BASEDIR)

CONFIG_NAME = os.path.join(BASEDIR, "crab/multicrabConfig-0.yml")


def main():

    multiconf = yaml.load(open(CONFIG_NAME).read())

    gridpacks = multiconf["gridpacks"]
    njobs = multiconf["njobs"]
    year = multiconf["year"]

    config.Data.totalUnits = config.Data.unitsPerJob * njobs
    config.Data.outLFNDirBase += "/{0}".format(year)

    # loop through
    donelist = list()
    for gridpack in gridpacks:

        print(gridpack)
        gridpack_name = os.path.basename(gridpack)
        mbs, mdp, ctau = get_param_from_gridpackname(gridpack_name)
        nametag = gridpack_name.split("_slc")[0]
        if "4Mu" in nametag:
            config.Data.outputPrimaryDataset = "SIDM_XXTo2ATo4Mu"
        elif "2Mu2e" in nametag:
            config.Data.outputPrimaryDataset = "SIDM_XXTo2ATo2Mu2e"

        config.Data.outputDatasetTag = "mXX-{0}_mA-{1}_ctau-{2}_GENSIM_{3}".format(
            floatpfy(mbs), floatpfy(mdp), floatpfy(ctau), year
        )
        config.General.requestName = "_".join(
            [
                getUsernameFromSiteDB(),
                "GENSIM",
                str(year),
                nametag,
                time.strftime("%y%m%d-%H%M%S"),
            ]
        )

        if gridpack.startswith("root://"):
            cpcmd = "xrdcp -f {0} {1}".format(
                gridpack, os.path.join(BASEDIR, "cfg/gridpack.tar.xz")
            )
        else:
            cpcmd = "cp {0} {1}".format(
                gridpack, os.path.join(BASEDIR, "cfg/gridpack.tar.xz")
            )

        if verbose:
            print(cpcmd)
            print(
                ">> ",
                os.path.join(
                    BASEDIR, "python/externalLHEProducer_and_PYTHIA8_Hadronizer_cff.py"
                ),
            )
            print(get_gentemplate(year).format(CTAU=ctau))
            print("------------------------------------------------------------")

        doCmd = (
            True
            if alwaysDoCmd
            else raw_input("OK to go? [y/n]").lower() in ["y", "yes"]
        )
        if doCmd:
            # 1. copy gridpack
            os.system(cpcmd)
            # 2. write genfrag_cfi
            with open(
                os.path.join(
                    BASEDIR, "python/externalLHEProducer_and_PYTHIA8_Hadronizer_cff.py"
                ),
                "w",
            ) as genfrag_cfi:
                genfrag_cfi.write(get_gentemplate(year).format(CTAU=ctau))
            # 3. write gen_cfg
            cfgcmd = get_command("GEN-SIM", year)
            os.system(cfgcmd)
            # 4. crab submit
            crabCommand("submit", config=config)
            donelist.append(gridpack)

    print("submitted: ", len(donelist))
    for x in donelist:
        print(x)
    print("------------------------------------------------------------")

    undonelist = [x for x in gridpacks if x not in donelist]
    print("unsubmitted: ", len(undonelist))
    for x in undonelist:
        print(x)
    if undonelist:
        with open("unsubmitted-0.yml.log", "w") as outf:
            yaml.dump(
                {"gridpacks": undonelist, "njobs": njobs, "year": year},
                outf,
                default_flow_style=False,
            )


if __name__ == "__main__":
    main()
