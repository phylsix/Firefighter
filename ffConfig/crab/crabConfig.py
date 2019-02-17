#!/usr/bin/env python
import os
import sys
import time

from CRABClient.UserUtilities import config, getUsernameFromSiteDB
from Firefighter.piedpiper.utils import ffDataset, adapt_config_with_dataset

BASEDIR = os.path.join(
    os.environ['CMSSW_BASE'], 'src', 'Firefighter', 'ffConfig')

if os.environ['CMSSW_VERSION'].startswith('CMSSW_8'):
    year = 2016
elif os.environ['CMSSW_VERSION'].startswith('CMSSW_9'):
    year = 2017
elif os.environ['CMSSW_VERSION'].startswith('CMSSW_10'):
    year = 2018
else:
    sys.exit('Wrong release!')

config = config()

config.General.requestName = '{0}_ffNtuple_{1}'.format(
    getUsernameFromSiteDB(),
    time.strftime('%y%m%d-%H%M%S')
)
config.General.workArea = 'crabWorkArea'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = os.path.join(BASEDIR, 'cfg')
config.JobType.numCores = 2
config.JobType.maxMemoryMB = 2500
config.JobType.disableAutomaticOutputCollection = False

config.Data.inputDataset = ''
config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.outLFNDirBase = '/store/group/lpcmetx/MCSIDM/ffNtuple/{}'.format(
    year)
config.Data.publication = False
config.Data.outputDatasetTag = ''
config.Data.ignoreLocality = True

config.Site.whitelist = ['T3_US_FNALLPC', 'T2_US_*', 'T3_US_*']
config.Site.ignoreGlobalBlacklist = True
config.Site.storageSite = 'T3_US_FNALLPC'


if __name__ == '__main__':

    import yaml
    myconf = yaml.load(open(sys.argv[1]).read())

    thisData = ffDataset(myconf['dataset'], year)
    config = adapt_config_with_dataset(config, thisData)

    from CRABAPI.RawCommand import crabCommand
    crabCommand('submit', config=config)
