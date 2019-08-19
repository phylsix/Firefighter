#!/usr/bin/env python
"""Utility functions associated with dataset names, helper to build configs
"""
import shlex
import subprocess
import yaml


class ffdatassetbase(object):
    def __init__(self):
        self.datasetNames_ = []
        self.maxEvents_ = -1
        self.globalTag_ = '102X_upgrade2018_realistic_v19'
        self.fileList_ = []

    @property
    def datasetNames(self):
        return self.datasetNames_

    @datasetNames.setter
    def datasetNames(self, dsnames):
        self.datasetNames_ = dsnames

    @property
    def maxEvents(self):
        return self.maxEvents_

    @maxEvents.setter
    def maxEvents(self, maxevts):
        self.maxEvents_ = maxevts

    @property
    def globalTag(self):
        return self.globalTag_

    @globalTag.setter
    def globalTag(self, globaltag):
        self.globalTag_ = globaltag

    @property
    def fileList(self):
        return self.fileList_

    @fileList.setter
    def fileList(self, filels):
        self.fileList_ = filels

    def dump(self):
        data = {
            'datasetNames': self.datasetNames,
            'maxEvents': self.maxEvents,
            'globalTag': self.globalTag,
            'fileList': self.fileList,
        }

        return yaml.dump(data, default_flow_style=False)


class ffdatasetsignal(ffdatassetbase):
    def __init__(self):
        ffdatassetbase.__init__(self)
        self.globalTag_ = '102X_upgrade2018_realistic_v19'


class ffdatasetbackground(ffdatassetbase):
    def __init__(self):
        ffdatassetbase.__init__(self)
        self.globalTag_ = '102X_upgrade2018_realistic_v19'

class ffdatasetdata(ffdatassetbase):
    def __init__(self):
        ffdatassetbase.__init__(self)
        self.globalTag_ = '102X_dataRun2_v11'
        self.lumiMask_ = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions18/13TeV/ReReco/Cert_314472-325175_13TeV_17SeptEarlyReReco2018ABC_PromptEraD_Collisions18_JSON_MuonPhys.txt'

    @property
    def lumiMask(self):
        return self.lumiMask_

    @lumiMask.setter
    def lumiMask(self, lm):
        self.lumiMask_ = lm

    def dump(self):
        data = {
            'datasetNames': self.datasetNames,
            'maxEvents': self.maxEvents,
            'globalTag': self.globalTag,
            'fileList': self.fileList,
            'lumiMask': self.lumiMask,
        }

        return yaml.dump(data, default_flow_style=False)

###############################################################################

def get_primaryDatasetName(dataset):
    """
    infer primary dataset name from the dataset string
    """

    return dataset.split('/')[1]

###############################################################################

def get_nametag(dataset):
    """
    infer nametag from a dataset string

    **sigmc/private**
    e.g.: /CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-200_MDp-1p2_ctau-0p48-354cda32a6a404e25b0eb21bb1bef952/USER
    returns SIDM_BsTo2DpTo4Mu_MBs-200_MDp-1p2_ctau-0p48
        /SIDM_XXTo2ATo2Mu2E/wsi-mXX-1000_mA-0p25_lxy-0p3_ctau-0p001875_AODSIM_2018-889dc1528a7a51924818b24d55ffac27/USER
    returns mXX-1000_mA-0p25_lxy-0p3_ctau-0p001875

    **bkgmc**
    e.g. /DYJetsToLL_M-10to50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM
    returns RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2

    **data**
    e.g. /DoubleMuon/Run2018A-17Sep2018-v2/AOD
    returns Run2018A-17Sep2018-v2
    """

    if dataset.endswith("USER"):
        taginfo = dataset.split("/")[-2].split("-", 1)[-1].rsplit("-", 1)[0]
        if dataset.startswith("/CRAB_PrivateMC"): ## old dataset name
            return taginfo
        if dataset.startswith("/SIDM"):
            return "_".join(taginfo.split("_")[:-2])
    else:
        return dataset.split("/")[-2]

###############################################################################

def get_storageSites(dataset, diskonly=True):
    """
    Get storage sites by dasgoclient
    """

    dasquery_site = 'dasgoclient -query="site dataset={}"'.format(dataset)
    try:
        dasres_site = subprocess.check_output(shlex.split((dasquery_site)))
        res = [str(s.decode()) for s in dasres_site.split()]
        if diskonly:
            res = [s for s in res if not (s.startswith("T1") and not s.endswith("Disk"))]
        return res
    except Exception as e:
        print("Exception when dasgoclient querying sites..")
        print("Msg: ", str(e))
        return []

###############################################################################

def get_submissionSites(dataset):
    """
    keep only T2&T3 sites' countries that actually hold the dataset
    """
    disksites = []
    storageSites = get_storageSites(dataset)
    for s in storageSites:
        if s.startswith("T2"):
            disksites.append(s)
        elif s.startswith("T3"):
            disksites.append(s)
        elif s.startswith("T1") and s.endswith("Disk"):
            disksites.append(s)
        else:
            pass
    res = ["_".join(s.split("_")[:2]) for s in disksites if not s.startswith("T1")]
    res = [str(s + "_*") for s in set(res)]
    res.extend([str(s) for s in disksites if s.startswith("T1")])
    if res:
        # res.extend(["T2_CH_CERN"])
        if 'T2_US_*' not in res:
            res.append('T2_US_*')
        if 'T3_US_FNALLPC' not in res:
            res.append('T3_US_FNALLPC')
        res = [s for s in res if not s.startswith("T1")]

    return res


###############################################################################


def get_datasetType(dataset):
    if dataset.endswith('USER'):
        return 'sigmc'
    elif dataset.endswith('AODSIM'):
        return 'bkgmc'
    elif dataset.endswith('AOD'):
        return 'data'
    else:
        raise ValueError("unknown type for dataset: {}".format(dataset))