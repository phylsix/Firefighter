#!/usr/bin/env python
"""crabConfigBuilder takes an ffDataSet config and addtional kwargs. It will
spawn path to ready-to-submit crab job configs per dataset.
"""
from __future__ import print_function

import os
import time
from os.path import join

import yaml
from CRABClient.UserUtilities import config as Config
from CRABClient.UserUtilities import getUsernameFromSiteDB
from Firefighter.ffConfig.datasetUtils import (
    get_nametag,
    get_primaryDatasetName,
    get_submissionSites,
)


class configBuilder:
    def __init__(self, ffdataset, **kwargs):
        self.ffdataset_ = ffdataset
        self.specs_ = dict(
            numThreads=1,
            maxMemory=4000,
            reportEvery=10000,
            outputFileName="ffNtuple.root",
            leptonJetCandStrategy="hadronFree",
            eventRegion="control",
            workArea=join(
                os.getenv("CMSSW_BASE"),
                "src/Firefighter/ffConfig/crabGarage/",
                time.strftime("%y%m%d"),
            ),
            splittingMode="FileBased",
            unitsPerJob=10,
            ffConfigName = 'ffNtupleFromAOD_v2_cfg.py',
            outbase="/store/group/lpcmetx/SIDM/ffNtupleV2/",
            year=2018,
        )
        self.specs_.update(kwargs)
        self.specs_["outLFNDirBase"] = join(self.specs_["outbase"], str(self.specs_["year"]))
        self.specs_['psetName'] = join(os.getenv("CMSSW_BASE"),
                                       "src/Firefighter/ffConfig/cfg/",
                                       self.specs_['ffConfigName'])

    def build(self):

        ## create workarea dir ##
        try:
            os.makedirs(self.specs_["workArea"])
        except:
            pass

        ## ffSuperConfig ##
        ffsc = {
            "job-spec": dict(
                numThreads=self.specs_["numThreads"],
                reportEvery=self.specs_["reportEvery"],
            ),
            "condition-spec": dict(globalTag=self.ffdataset_["globalTag"]),
            "data-spec": dict(
                inputFileList=["file:AODSIM.root"],
                maxEvents=self.ffdataset_["maxEvents"],
                outputFileName=self.specs_["outputFileName"],
            ),
            "reco-spec": dict(
                eventRegion=self.specs_["eventRegion"],
                leptonJetCandStrategy=self.specs_["leptonJetCandStrategy"],
            ),
        }

        if 'denomTriggerPaths' in self.specs_:
            ffsc['data-spec']['denomTriggerPaths'] = self.specs_['denomTriggerPaths']

        ## crab ##
        res = []
        for ds in self.ffdataset_["datasetNames"]:
            config = Config()
            config.General.requestName = "{0}_ffNtuple_{1}".format(
                getUsernameFromSiteDB(), time.strftime("%y%m%d-%H%M%S")
            )
            config.General.workArea = self.specs_["workArea"]
            config.General.transferOutputs = True
            config.General.transferLogs = False
            config.JobType.pluginName = "Analysis"
            config.JobType.psetName = self.specs_["psetName"]
            config.JobType.numCores = self.specs_["numThreads"]
            config.JobType.maxMemoryMB = self.specs_["maxMemory"]
            config.JobType.disableAutomaticOutputCollection = False
            config.Data.inputDataset = ds
            config.Data.inputDBS = "phys03"
            config.Data.splitting = self.specs_["splittingMode"]
            config.Data.unitsPerJob = self.specs_["unitsPerJob"]
            config.Data.outLFNDirBase = self.specs_["outLFNDirBase"]
            config.Data.publication = False
            config.Data.outputDatasetTag = get_nametag(ds)
            config.Data.ignoreLocality = True
            config.Site.whitelist = get_submissionSites(ds)
            config.Site.ignoreGlobalBlacklist = True
            config.Site.storageSite = "T3_US_FNALLPC"

            reqNameParts = [
                str(self.specs_["year"]),
                get_primaryDatasetName(ds),
                get_nametag(ds),
                "ffNtuple",
                time.strftime("%y%m%d-%H%M%S"),
            ]

            if ds.endswith("USER"):  # sigmc/private
                ffsc["data-spec"]["dataType"] = "sigmc"
                config.Site.whitelist = ["T3_US_FNALLPC", "T3_US_*", "T2_US_*"]
            elif ds.endswith("AODSIM"):  # bkgmc
                ffsc["data-spec"]["dataType"] = "bkgmc"
                nametagVersionSuffix = reqNameParts[2].rsplit("_")[-1]
                reqNameParts[2] = (
                    nametagVersionSuffix
                    if nametagVersionSuffix.startswith("ext")
                    else nametagVersionSuffix.rsplit("-")[-1]
                )
                config.Data.inputDBS = "global"
            elif ds.endswith("AOD"):  # data
                ffsc["data-spec"]["dataType"] = "data"
                config.Data.inputDBS = "global"
                config.Data.splitting = "LumiBased"
                config.Data.lumiMask = self.ffdataset_["lumiMask"]
                config.Data.unitsPerJob = 100
            config.General.requestName = "_".join(reqNameParts)

            ## construct ffSuperConfig ##
            ffscFn = join(
                self.specs_["workArea"],
                "ffSuperConfig_{}.yml".format("_".join(reqNameParts[:3])),
            )
            with open(ffscFn, "w") as outf:
                outf.write(yaml.dump(ffsc, default_flow_style=False))
            config.JobType.pyCfgParams = ["config={0}".format(ffscFn)]

            ## dump crab config file ##
            ffCrabCfgFn = ffscFn.split(".")[0].replace("ffSuperConfig", "crab") + ".py"
            with open(ffCrabCfgFn, "w") as outf:
                outf.write(str(config))

            res.append(ffCrabCfgFn)

        return res

    @staticmethod
    def submit(crabconfig):
        print("$crab submit -c", crabconfig)
        os.system("crab submit -c {0}".format(crabconfig))
