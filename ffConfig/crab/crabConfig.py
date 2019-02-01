#!/usr/bin/env python

from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

import time
config.General.requestName = '{0}_ffNtuple_{1}'.format(
    getUsernameFromSiteDB(),
    time.strftime('%y%m%d-%H%M%S')
    )
config.General.workArea = 'crabWorkArea'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '../cfg/ffNtupleFromAOD_cfg.py'
config.JobType.numCores = 2
config.JobType.maxMemoryMB = 2500
config.JobType.disableAutomaticOutputCollection = False

config.Data.inputDataset = ''
config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 10
config.Data.outLFNDirBase = '/store/group/lpcmetx/MCSIDM/ffNtuple'
config.Data.publication = False
config.Data.outputDatasetTag = ''
config.Data.ignoreLocality = True

config.Site.whitelist = ['T3_US_FNALLPC', 'T2_US_*', 'T3_US_*']
config.Site.ignoreGlobalBlacklist = True
config.Site.storageSite = 'T3_US_FNALLPC'


if __name__ == '__main__':

    import yaml
    myconf = yaml.load(open('config_bkg_JpsiToMuMu.yml').read())

    import os
    import sys
    if os.environ['CMSSW_VERSION'].startswith('CMSSW_9'):
        year = 2017
    elif os.environ['CMSSW_VERSION'].startswith('CMSSW_10'):
        year = 2018
    else:
        sys.exit('Wrong release!')

    config.Data.inputDataset = myconf['dataset']
    if not myconf['dataset'].endswith('USER'):
        config.Data.inputDBS = 'global'
    config.Data.outputDatasetTag = myconf['nametag'] + '_ffNtuple'
    config.Data.outLFNDirBase += '/{0}/{1}'.format(year, myconf['nametag'])
    config.General.requestName = '_'.join([
        getUsernameFromSiteDB(),
        'ffNtuple',
        str(year),
        myconf['nametag'],
        time.strftime('%y%m%d-%H%M%S')
    ])

    from CRABAPI.RawCommand import crabCommand
    crabCommand('submit', config = config)
