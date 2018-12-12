#!/usr/bin/env python
from __future__ import print_function
import os
import yaml
import time

from Firefighter.piedpiper.utils import *
from Firefighter.piedpiper.template import singlecrabConfigRECO


verbose = True
doCmd = False


def main():

    # set BASEDIR
    if os.environ['CMSSW_BASE'] not in os.path.abspath(__file__):
        print('$CMSSW_BASE: ', os.environ['CMSSW_BASE'])
        print('__file__: ', os.path.abspath(__file__))
        sys.exit('Inconsistant release environment!')

    BASEDIR = os.path.join(os.environ['CMSSW_BASE'], 'src/Firefighter/piedpiper')
    print(BASEDIR)

    # load config
    multiconf = yaml.load(open('multicrabConfig-2.yml').read())

    inputdatasets = multiconf['premixdigihltdatasets']

    donelist = list()
    for ds in inputdatasets:

        print("dataset: ", ds)
        nametag = get_nametag_from_dataset(ds)
        cmd = 'crab submit -c crabConfig-2.py'

        if verbose:
            print(">> ", os.path.join(BASEDIR, 'crab/config-2.yml'))
            print(singlecrabConfigRECO.format(DS=ds, NT=nametag))
            print(cmd)
            print('-----------------------------------------------------')

        if doCmd:
            with open(os.path.join(BASEDIR, 'crab/config-2.yml'), 'w') as conf:
                conf.write(singlecrabConfigRECO.format(DS=ds, NT=nametag))
            os.system(cmd)
            time.sleep(10)
            donelist.append(ds)

    print('submitted: ', len(donelist))
    for x in donelist: print(x)
    print('------------------------------------------------------------')

    undonelist = [x for x in inputdatasets if x not in donelist]
    print('unsubmitted: ', len(undonelist))
    for x in undonelist: print(x)
    if undonelist:
        with open('unsubmitted-1.yml', 'w') as outf:
            yaml.dump({'premixdigihltdatasets': undonelist}, outf, default_flow_style=False)

if __name__ = '__main__':
    main()
