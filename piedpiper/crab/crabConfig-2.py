import yaml
myconf = yaml.load(open('config.yml').read())

from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

import time
config.General.requestName = '{0}MCSIDM_AODSIM_{1}'.format(
    getUsernameFromSiteDB(),
    time.strftime('%y%m%d-%H%M%S')
    )
config.General.workArea = 'crabWorkArea'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '../cfg/SIDM_AODSIM_cfg.py'
config.JobType.numCores = 8
config.JobType.maxMemoryMB = 6000
config.JobType.disableAutomaticOutputCollection = False

config.Data.inputDataset = '/CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-200_MDp-1p2_ctau-0p48-354cda32a6a404e25b0eb21bb1bef952/USER'
config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.outLFNDirBase = '/store/user/%s/MCSIDM/AODSIM' % (getUsernameFromSiteDB())
config.Data.publication = True
config.Data.outputDatasetTag = myconf['nametag']
config.Data.ignoreLocality = True

config.Site.whitelist = ['T3_US_FNALLPC', 'T2_CH_CERN', 'T2_US_Purdue']
config.Site.ignoreGlobalBlacklist = True
config.Site.storageSite = 'T3_US_FNALLPC'
