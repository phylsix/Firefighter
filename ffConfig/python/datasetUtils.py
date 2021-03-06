#!/usr/bin/env python
"""Utility functions associated with dataset names, helper to build configs
"""
import shlex
import subprocess
import sys
import yaml


class ffdatassetbase(object):
    def __init__(self):
        self.datasetNames_ = []
        self.maxEvents_ = -1
        self.globalTag_ = '102X_upgrade2018_realistic_v21'
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


class ffdatasetbackground(ffdatassetbase):
    def __init__(self):
        ffdatassetbase.__init__(self)

class ffdatasetdata(ffdatassetbase):
    def __init__(self):
        ffdatassetbase.__init__(self)
        self.globalTag_ = '102X_dataRun2_v13'
        self.lumiMask_ = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions18/13TeV/ReReco/Cert_314472-325175_13TeV_17SeptEarlyReReco2018ABC_PromptEraD_Collisions18_JSON.txt'

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
    if dataset.endswith('USER') or dataset.startswith('/SIDM'):
        return 'sigmc'
    elif dataset.endswith('AODSIM'):
        return 'bkgmc'
    elif dataset.endswith('AOD'):
        return 'data'
    else:
        raise ValueError("unknown type for dataset: {}".format(dataset))

###############################################################################

def sigmc_ctau2lxy(mxx, ma, ctau):

    rawval = mxx / 2 / ma * 0.76 * ctau / 10
    rawval_str = '{:.1E}'.format(rawval)
    sigval, power = rawval_str.split('E')
    sigval = float(sigval)
    if sigval - int(sigval) > 0.5:
        sigval = int(sigval) + 1
    elif sigval-int(sigval)<0.5:
        sigval = int(sigval)

    power = float('1E' + power)
    lxy = round(sigval * power, 1)
    if lxy - int(lxy) < 1E-5:
        return int(lxy)
    else:
        return lxy

###############################################################################

def query_yes_no(question, default="yes"):
    """Ask a yes/no question via raw_input() and return their answer.
    "question" is a string that is presented to the user.
    "default" is the presumed answer if the user just hits <Enter>.
        It must be "yes" (the default), "no" or None (meaning
        an answer is required of the user).
    The "answer" return value is one of "yes" or "no".
    """

    valid = {"yes":True,   "y":True,  "ye":True,
             "no":False,     "n":False}
    if default == None:
        prompt = " [y/n] "
    elif default == "yes":
        prompt = " [Y/n] "
    elif default == "no":
        prompt = " [y/N] "
    else:
        raise ValueError("invalid default answer: '%s'" % default)

    while True:
        sys.stdout.write(question + prompt)
        choice = raw_input().lower()
        if default is not None and choice == '':
            return valid[default]
        elif choice in valid:
            return valid[choice]
        else:
            sys.stdout.write("Please respond with 'yes' or 'no' "\
"(or 'y' or 'n').\n")
