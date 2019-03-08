#!/usr/bin/env python
from __future__ import print_function
import os
import yaml
import time

from CRABAPI.RawCommand import crabCommand
from Firefighter.piedpiper.utils import *
from crabConfig_2 import *

doCmd = True
CONFIG_NAME = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), 'multicrabConfig-2.yml')


def main():

    # safety check
    if os.environ['CMSSW_BASE'] not in os.path.abspath(__file__):
        print('$CMSSW_BASE: ', os.environ['CMSSW_BASE'])
        print('__file__: ', os.path.abspath(__file__))
        sys.exit('Inconsistant release environment!')

    # load config
    multiconf = yaml.load(open(CONFIG_NAME).read())

    inputdatasets = multiconf['premixdigihltdatasets']
    year = multiconf['year']
    # manualdatasets = multiconf['manual']
    # config.Data.outLFNDirBase += '/{0}'.format(year)
    config.Data.outLFNDirBase = '/store/group/lpcmetx/MCSIDM/AODSIM/{0}'.format(
        year)

    memreq = 15100 if year == 2018 else 6000
    config.JobType.maxMemoryMB = memreq
    config.JobType.numCores = 4 if year == 2016 else 8

    donelist = list()
    for ds in inputdatasets:

        nametag = ds.split('/')[-2].split('-',
                                          1)[-1].rsplit('-', 1)[0].replace(
                                              'PREMIXRAWHLT', 'AODSIM')
        pd = ds.split('/')[1]
        # this is fix for previous non-careful naming convention
        if 'CRAB_PrivateMC' in ds:
            nametag = ds.split('/')[-2].rsplit('-', 1)[0].replace(
                'PREMIXRAWHLT', 'AODSIM')
        print("dataset: ", ds)
        print("nametag: ", nametag)
        print("primarydataset: ", pd)
        config.Data.inputDataset = ds
        config.Data.outputDatasetTag = nametag
        config.General.requestName = '_'.join(
            ['AODSIM',
             str(year), pd, nametag,
             time.strftime('%y%m%d-%H%M%S')])

        if doCmd:
            crabCommand('submit', config=config)
            donelist.append(ds)

    print('submitted: ', len(donelist))
    for x in donelist:
        print(x)
    print('------------------------------------------------------------')

    undonelist = [x for x in inputdatasets if x not in donelist]
    print('unsubmitted: ', len(undonelist))
    for x in undonelist:
        print(x)
    if undonelist:
        with open('unsubmitted-2.yml.log', 'w') as outf:
            yaml.dump({
                'premixdigihltdatasets': undonelist,
                'year': year
            },
                      outf,
                      default_flow_style=False)


if __name__ == '__main__':
    main()
