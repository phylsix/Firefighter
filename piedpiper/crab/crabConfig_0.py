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
config.General.transferLogs = True

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
config.Data.outLFNDirBase = '/store/group/lpcmetx/MCSIDM/GENSIM'
config.Data.publication = True
config.Data.outputPrimaryDataset = ''
config.Data.outputDatasetTag = ''
config.Data.ignoreLocality = True

config.Site.whitelist = ['T3_US_FNALLPC', 'T2_US_*', 'T3_US_*']
config.Site.ignoreGlobalBlacklist = True
config.Site.storageSite = 'T3_US_FNALLPC'


if __name__ == '__main__':

    import yaml
    myconf = yaml.load(open('config-0.yml').read())

    import os
    import sys
    cmsrel = os.environ['CMSSW_VERSION']

    if cmsrel.startswith('CMSSW_8'):
        year = 2016
    elif cmsrel.startswith('CMSSW_9'):
        year = 2017
    elif cmsrel.startswith('CMSSW_10'):
        year = 2018
    else:
        sys.exit('Wrong release!')

    # you need to take care of copying gridpacks and update genfrag_cfi/gencfg
    config.Data.totalUnits = config.Data.unitsPerJob * myconf['totaljobs']
    config.Data.outputPrimaryDataset = myconf['pd']
    config.Data.outputDatasetTag = '{0}_{1}'.format(myconf['nametag'], year)
    config.Data.outLFNDirBase += '/{0}'.format(year)
    config.General.requestName = '_'.join([
        getUsernameFromSiteDB(),
        'GENSIM',
        str(year),
        myconf['nametag'],
        time.strftime('%y%m%d-%H%M%S')
    ])

    from CRABAPI.RawCommand import crabCommand
    crabCommand('submit', config = config)
