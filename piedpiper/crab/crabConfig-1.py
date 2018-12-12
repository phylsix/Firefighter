import yaml
myconf = yaml.load(open('config-1.yml').read())

import os
import sys
if os.environ['CMSSW_VERSION'].startswith('CMSSW_9'):
    year = 2017
    memreq = 6000
elif os.environ['CMSSW_VERSION'].startswith('CMSSW_10'):
    year = 2018
    memreq = 15100
else:
    sys.exit('Wrong release!')

from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

import time
config.General.requestName = '{0}MCSIDM_PREMIXRAWHLT_{1}'.format(
    getUsernameFromSiteDB(),
    time.strftime('%y%m%d-%H%M%S')
    )
config.General.workArea = 'crabWorkArea'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '../cfg/SIDM_PREMIXRAWHLT_cfg.py'
config.JobType.numCores = 8
config.JobType.maxMemoryMB = memreq
config.JobType.disableAutomaticOutputCollection = False

config.Data.inputDataset = myconf['dataset']
config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.outLFNDirBase = '/store/user/%s/MCSIDM/PREMIXRAWHLT' % (getUsernameFromSiteDB())
config.Data.publication = True
config.Data.outputDatasetTag = myconf['nametag']
config.Data.ignoreLocality = True

config.Site.whitelist = ['T3_US_FNALLPC', 'T2_CH_CERN', 'T2_US_Purdue']
config.Site.ignoreGlobalBlacklist = True
config.Site.storageSite = 'T3_US_FNALLPC'
