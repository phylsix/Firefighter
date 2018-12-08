import yaml
myconf = yaml.load(open('config.yml').read())

from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

import time
config.General.requestName = '{0}MCSIDM_GENSIM_{1}'.format(
    getUsernameFromSiteDB(),
    time.strftime('%y%m%d-%H%M%S')
    )
config.General.workArea = 'crabWorkArea'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'PrivateMC'
config.JobType.psetName = '../cfg/SIDM_GENSIM_cfg.py'
config.JobType.inputFiles = [
    '../data/run_generic_tarball_cvmfs.sh',
    '../cfg/gridpack.tar.xz'
]
config.JobType.numCores = 4
config.JobType.maxMemoryMB = 4000
config.JobType.disableAutomaticOutputCollection = False

config.Data.splitting = 'EventBased'
config.Data.unitsPerJob = 1000
config.Data.totalUnits = config.Data.unitsPerJob * myconf['totaljobs']
config.Data.outLFNDirBase = '/store/user/%s/MCSIDM/GENSIM' % (getUsernameFromSiteDB())
config.Data.publication = True
config.Data.outputDatasetTag = myconf['nametag']
config.Data.outputPrimaryDataset = 'CRAB_PrivateMC'
config.Data.ignoreLocality = True

config.Site.whitelist = ['T3_US_FNALLPC']
config.Site.ignoreGlobalBlacklist = True
config.Site.storageSite = 'T3_US_FNALLPC'
