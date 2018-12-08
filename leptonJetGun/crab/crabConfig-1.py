
from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

import time
config.General.requestName = 'wsiLJGun_%s' % time.strftime('%y%m%d-%H%M%S')
config.General.workArea = 'crabWorkArea'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '../cfg/ljGun_DIGIRAWHLT_cfg.py'
config.JobType.numCores = 8
config.JobType.maxMemoryMB = 6000
config.JobType.disableAutomaticOutputCollection = False

config.Data.inputDataset = '/CRAB_PrivateMC/wsi-LJGunMuMu-5d1256f7e57f31522790da86237509df/USER'
config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.outLFNDirBase = '/store/user/%s/MCLJGun' % (getUsernameFromSiteDB())
config.Data.publication = True
config.Data.outputDatasetTag = 'LJGunMuMu'
config.Data.ignoreLocality = True

config.Site.whitelist = ['T3_US_FNALLPC', 'T2_CH_CERN', 'T2_US_Purdue']
config.Site.ignoreGlobalBlacklist = True
config.Site.storageSite = 'T3_US_FNALLPC'
