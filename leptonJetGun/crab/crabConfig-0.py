
from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

import time
config.General.requestName = 'wsiLJGun_%s' % time.strftime('%y%m%d-%H%M%S')
config.General.workArea = 'crabWorkArea'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'PrivateMC'
config.JobType.psetName = '../cfg/ljGun_GENSIM_cfg.py'
config.JobType.numCores = 8
config.JobType.maxMemoryMB = 4000
config.JobType.disableAutomaticOutputCollection = False

config.Data.splitting = 'EventBased'
config.Data.unitsPerJob = 1000
config.Data.totalUnits = config.Data.unitsPerJob * 10
config.Data.outLFNDirBase = '/store/user/%s/MCLJGun' % (getUsernameFromSiteDB())
config.Data.publication = True
config.Data.outputDatasetTag = 'LJGunMuMu'
config.Data.outputPrimaryDataset = 'CRAB_PrivateMC'
config.Data.ignoreLocality = True

config.Site.whitelist = ['T3_US_FNALLPC']
config.Site.ignoreGlobalBlacklist = True
config.Site.storageSite = 'T3_US_FNALLPC'