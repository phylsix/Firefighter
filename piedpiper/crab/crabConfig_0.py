#!/usr/bin/env python

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
config.Data.totalUnits = config.Data.unitsPerJob * 1
config.Data.outLFNDirBase = '/store/user/%s/MCSIDM/GENSIM' % (getUsernameFromSiteDB())
config.Data.publication = True
config.Data.outputDatasetTag = ''
config.Data.outputPrimaryDataset = 'CRAB_PrivateMC'
config.Data.ignoreLocality = True

config.Site.whitelist = ['T3_US_FNALLPC']
config.Site.ignoreGlobalBlacklist = True
config.Site.storageSite = 'T3_US_FNALLPC'


if __name__ == '__main__':

    import yaml
    myconf = yaml.load(open('config-0.yml').read())

    import os
    import sys
    if os.environ['CMSSW_VERSION'].startswith('CMSSW_9'):
        year = 2017
    elif os.environ['CMSSW_VERSION'].startswith('CMSSW_10'):
        year = 2018
    else:
        sys.exit('Wrong release!')

    # you need to take care of copying gridpacks and update genfrag_cfi/gencfg
    config.Data.totalUnits = config.Data.unitsPerJob * myconf['totaljobs']
    config.Data.outputDatasetTag = myconf['nametag']
    config.Data.outLFNDirBase += '/{0}'.format(year)

    from CRABAPI.RawCommand import crabCommand
    crabCommand('submit', config = config)