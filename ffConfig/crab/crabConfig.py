import yaml
myconf = yaml.load(open('config.yml').read())

from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

import time
config.General.requestName = '{0}{1}_ffNtuple_{2}'.format(
    getUsernameFromSiteDB(),
    myconf['jobtype'],
    time.strftime('%y%m%d-%H%M%S')
    )
config.General.workArea = 'crabWorkArea'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '../cfg/ffNtupleFromAOD_cfg.py'
config.JobType.numCores = 4
config.JobType.maxMemoryMB = 2500
config.JobType.disableAutomaticOutputCollection = False

config.Data.inputDataset = myconf['inputdataset']
config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.outLFNDirBase = '/store/user/%s/%s/ffNtuple' % (getUsernameFromSiteDB(), myconf['jobtype'])
config.Data.publication = True
config.Data.outputDatasetTag = myconf['nametag']
config.Data.ignoreLocality = False

#config.Site.whitelist = ['T3_US_FNALLPC', 'T2_US_*', 'T3_US_*']
config.Site.ignoreGlobalBlacklist = True
config.Site.storageSite = 'T3_US_FNALLPC'
