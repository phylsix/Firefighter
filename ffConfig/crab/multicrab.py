#!/usr/bin/env python
from __future__ import print_function

import os
import sys
import yaml
import time

from CRABAPI.RawCommand import crabCommand
from Firefighter.piedpiper.utils import *
from crabConfig import *


doCmd = True
CONFIG_NAME = sys.argv[1]

def main():

    # safety check
    if os.environ['CMSSW_BASE'] not in os.path.abspath(__file__):
        print('$CMSSW_BASE: ', os.environ['CMSSW_BASE'])
        print('__file__: ', os.path.abspath(__file__))
        sys.exit('Inconsistant release environment!')

    # load config
    multiconf = yaml.load(open(CONFIG_NAME).read())

    datasets = multiconf['aodsimdatasets']
    config.Data.outLFNDirBase += '/{0}'.format(year)

    donelist = list()
    for ds in datasets:

        signalMC = ds.endswith('USER')

        pd = ds.split('/')[1]
        nametag = get_nametag_from_dataset(ds)
        reqNameItems = [str(year), pd, time.strftime('%y%m%d-%H%M%S')]

        if signalMC:
            print('+++++++++++++++++++++')
            print('===== SIGNAL MC =====')
            print('+++++++++++++++++++++')
            nametag = nametag.replace('AODSIM', 'ffNtuple')
            reqNameItems[1] += nametag
        else:
            print('--------------------------')
            print('===== DATA or BKG MC =====')
            print('--------------------------')
            nametag += '_ffNtuple'
            config.Data.inputDBS = 'global'


        print("dataset: ", ds)
        print("nametag: ", nametag)
        print("primarydataset: ", pd)

        config.Data.inputDataset = ds
        config.Data.outputDatasetTag = nametag
        config.General.requestName = '_'.join( reqNameItems )

        if doCmd:
            crabCommand('submit', config = config)
            donelist.append(ds)

    print('submitted: ', len(donelist))
    for x in donelist: print(x)
    print('------------------------------------------------------------')

    undonelist = [x for x in datasets if x not in donelist]
    print('unsubmitted: ', len(undonelist))
    for x in undonelist: print(x)
    if undonelist:
        with open('unsubmitted.yml.log', 'w') as outf:
            yaml.dump({'aodsimdatasets': undonelist, 'year': year}, outf, default_flow_style=False)


if __name__ == '__main__':
    main()
