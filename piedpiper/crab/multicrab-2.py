#!/usr/bin/env python
from __future__ import print_function
import os
import yaml
import time

from Firefighter.piedpiper.utils import *

from crabConfig_2 import *

doCmd = True
CONFIG_NAME = os.path.join(
    os.path.dirname(
        os.path.abspath(__file__)
    ),
    'multicrabConfig-2.yml'
)


def main():

    # safety check
    if os.environ['CMSSW_BASE'] not in os.path.abspath(__file__):
        print('$CMSSW_BASE: ', os.environ['CMSSW_BASE'])
        print('__file__: ', os.path.abspath(__file__))
        sys.exit('Inconsistant release environment!')

    # load config
    multiconf = yaml.load(open(CONFIG_NAME).read())

    inputdatasets = multiconf['premixdigihltdatasets']
    year          = multiconf['year']
    manualdatasets = multiconf['manual']
    # config.Data.outLFNDirBase += '/{0}'.format(year)
    config.Data.outLFNDirBase = '/store/group/lpcmetx/MCSIDM/AODSIM/{0}'.format(year)

    if year == 2017: memreq = 6000
    elif year == 2018: memreq = 15100
    config.JobType.maxMemoryMB = memreq

    donelist = list()
    for ds in inputdatasets:

        nametag = get_nametag_from_dataset(ds)
        print("dataset: ", ds)
        print("nametag: ", nametag)
        config.Data.inputDataset = ds
        config.Data.outputDatasetTag = nametag
        config.General.requestName = '_'.join([
            getUsernameFromSiteDB(),
            'AODSIM',
            str(year),
            nametag,
            time.strftime('%y%m%d-%H%M%S')
        ])

        if doCmd:
            from CRABAPI.RawCommand import crabCommand
            crabCommand('submit', config = config)
            time.sleep(5)
            donelist.append(ds)


    print('submitted: ', len(donelist))
    for x in donelist: print(x)
    print('------------------------------------------------------------')

    undonelist = [x for x in inputdatasets if x not in donelist]
    print('unsubmitted: ', len(undonelist))
    for x in undonelist: print(x)
    if undonelist:
        with open('unsubmitted-2.yml.log', 'w') as outf:
            yaml.dump({'premixdigihltdatasets': undonelist, 'year': year}, outf, default_flow_style=False)

if __name__ == '__main__':
    main()