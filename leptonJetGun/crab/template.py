#!/usr/bin/env python

template_GEN = """
from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

import time
config.General.requestName = 'wsiLJGun_%s' % time.strftime('%y%m%d-%H%M%S')
config.General.workArea = 'crabWorkArea'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'PrivateMC'
config.JobType.psetName = '{FRAGNAME}'
config.JobType.numCores = 4
config.JobType.maxMemoryMB = 4000
config.JobType.disableAutomaticOutputCollection = False

config.Data.splitting = 'EventBased'
config.Data.unitsPerJob = {PERJOB}
config.Data.totalUnits = config.Data.unitsPerJob * {NJOBS}
config.Data.outLFNDirBase = '/store/user/%s/MCLJGun' % (getUsernameFromSiteDB())
config.Data.publication = True
config.Data.outputDatasetTag = '{OUTPUTTAG}'
config.Data.outputPrimaryDataset = 'CRAB_PrivateMC'
config.Data.ignoreLocality = True

config.Site.whitelist = ['T3_US_FNALLPC']
config.Site.ignoreGlobalBlacklist = True
config.Site.storageSite = 'T3_US_FNALLPC'
"""

template_general = """
from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

import time
config.General.requestName = 'wsiLJGun_%s' % time.strftime('%y%m%d-%H%M%S')
config.General.workArea = 'crabWorkArea'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '{FRAGNAME}'
config.JobType.numCores = {CORE}
config.JobType.maxMemoryMB = {MEMINMB}
config.JobType.disableAutomaticOutputCollection = False

config.Data.inputDataset = '{INPUTDATASET}'
config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.outLFNDirBase = '/store/user/%s/MCLJGun' % (getUsernameFromSiteDB())
config.Data.publication = True
config.Data.outputDatasetTag = '{OUTPUTTAG}'
config.Data.ignoreLocality = True

config.Site.whitelist = ['T3_US_FNALLPC', 'T2_CH_CERN', 'T2_US_Purdue']
config.Site.ignoreGlobalBlacklist = True
config.Site.storageSite = 'T3_US_FNALLPC'
"""