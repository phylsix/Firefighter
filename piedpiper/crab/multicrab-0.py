#!/usr/bin/env python
from __future__ import print_function

import os
import sys
import time
from os.path import basename, join

import yaml
from CRABAPI.RawCommand import crabCommand
from crabConfig_0 import *
from Firefighter.piedpiper.utils import *

verbose = False
alwaysDoCmd = True

if os.environ["CMSSW_BASE"] not in os.path.abspath(__file__):
    print("$CMSSW_BASE: ", os.environ["CMSSW_BASE"])
    print("__file__: ", os.path.abspath(__file__))
    sys.exit("Inconsistant release environment!")

BASEDIR = join(os.environ["CMSSW_BASE"], "src/Firefighter/piedpiper")
CONFIG_NAME = sys.argv[1]
assert os.path.isfile(CONFIG_NAME)


def main():

    multiconf = yaml.load(open(CONFIG_NAME).read())

    gridpacks = multiconf["gridpacks"]
    njobs = multiconf["njobs"]
    year = multiconf["year"]
    lxy = multiconf["lxy"]
    ctaus = multiconf.get("ctaus", None)
    assert len(gridpacks) == len(ctaus)
    ctaumap = dict(zip(gridpacks, ctaus))

    config.Data.totalUnits = config.Data.unitsPerJob * njobs
    config.Data.outLFNDirBase += "/{0}".format(year)

    # loop through
    donelist = list()
    for gridpack in gridpacks:

        print("gridpack:", gridpack)
        #'SIDM_XXTo2ATo4Mu_mXX-1000_mA-0p25_slc6_amd64_gcc481_CMSSW_7_1_30_tarball.tar.xz'
        gridpack_name = basename(gridpack)

        ## outputPrimaryDataset: SIDM_XXTo2ATo4Mu or SIDM_XXTo2ATo2Mu2e
        config.Data.outputPrimaryDataset = gridpack_name.split("_mXX")[0]

        ## outputDatasetTag: mXX-1000_mA-0p25_lxy-0p3_ctau-0p001875_GENSIM_2018
        mxxma = gridpack_name.split("_", 2)[-1].split("_slc")[0]
        lxystr = str(lxy).replace(".", "p")
        ctaustr = str(ctaumap[gridpack]).replace(".", "p")
        config.Data.outputDatasetTag = "{}_lxy-{}_ctau-{}_GENSIM_{}".format(
            mxxma, lxystr, ctaustr, year
        )

        ## requestName
        config.General.requestName = "_".join(
            [
                config.Data.outputPrimaryDataset,
                config.Data.outputDatasetTag,
                time.strftime("%y%m%d-%H%M%S"),
            ]
        )

        if gridpack.startswith("root://"):
            cpcmd = "xrdcp -f {0} {1}".format(gridpack, join(BASEDIR, "cfg/gridpack.tar.xz"))
        elif gridpack.startswith("http"):
            cpcmd = "wget -q {} -O {}".format(gridpack, join(BASEDIR, "cfg/gridpack.tar.xz"))
        else:
            cpcmd = "cp {0} {1}".format(gridpack, join(BASEDIR, "cfg/gridpack.tar.xz"))

        if verbose:
            print("$", cpcmd)
            print(
                "$ cat", join(BASEDIR, "python/externalLHEProducer_and_PYTHIA8_Hadronizer_cff.py")
            )
            print(get_gentemplate(year).format(CTAU=ctaumap[gridpack]))
            print("------------------------------------------------------------")
            print(config)
            print("------------------------------------------------------------")

        doCmd = True if alwaysDoCmd else raw_input("OK to go? [y/n]").lower() in ["y", "yes"]
        if doCmd:
            # 1. copy gridpack
            os.system(cpcmd)
            # 2. write genfrag_cfi
            with open(
                join(BASEDIR, "python/externalLHEProducer_and_PYTHIA8_Hadronizer_cff.py"), "w"
            ) as genfrag_cfi:
                genfrag_cfi.write(get_gentemplate(year).format(CTAU=ctaumap[gridpack]))
            # 3. write gen_cfg
            cfgcmd = get_command("GEN-SIM", year, rand=False)
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
