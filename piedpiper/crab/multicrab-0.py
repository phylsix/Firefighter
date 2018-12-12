#!/usr/bin/env python
from __future__ import print_function
import os
import yaml
import time

from Firefighter.piedpiper.utils import *
from Firefighter.piedpiper.template import genTemplate, singlecrabConfigGEN

verbose = False
alwaysDoCmd = True

def main():

    # set BASEDIR
    if os.environ['CMSSW_BASE'] not in os.path.abspath(__file__):
        print('$CMSSW_BASE: ', os.environ['CMSSW_BASE'])
        print('__file__: ', os.path.abspath(__file__))
        sys.exit('Inconsistant release environment!')

    BASEDIR = os.path.join(os.environ['CMSSW_BASE'], 'src/Firefighter/piedpiper')
    print(BASEDIR)

    # load config
    multiconf = yaml.load(open('multicrabConfig-0.yml').read())

    gridpacks = multiconf['gridpacks']
    njobs = multiconf['njobs']

    # loop through
    donelist = list()
    for gridpack in gridpacks:

        print(gridpack)
        gridpack_name = os.path.basename(gridpack)
        mbs, mdp, ctau  = get_param_from_gridpackname(gridpack_name)

        if gridpack.startswith('root://'):
            cmd1 = 'xrdcp -f {0} {1}'.format(gridpack,
                                             os.path.join(BASEDIR, 'cfg/gridpack.tar.xz'))
        else:
            cmd1 = 'cp {0} {1}'.format(gridpack,
                                       os.path.join(BASEDIR, 'cfg/gridpack.tar.xz'))

        cmd2 = 'crab submit -c crabConfig-0.py'

        if verbose:

            print(cmd1)

            print('>> ', os.path.join(BASEDIR, 'python/externalLHEProducer_and_PYTHIA8_Hadronizer_cff.py'))
            print(genTemplate.format(CTAU=ctau))
            print('>> ', os.path.join(BASEDIR, 'crab/config.yml'))
            print(singlecrabConfigGEN.format(NT=gridpack_name.split('_slc')[0], NJ=njobs))

            print(cmd2)
            print('------------------------------------------------------------')

        doCmd = True if alwaysDoCmd else raw_input('OK to go? [y/n]').lower() in ['y', 'yes']

        if doCmd:

            os.system(cmd1)

            with open(os.path.join(BASEDIR, 'python/externalLHEProducer_and_PYTHIA8_Hadronizer_cff.py'), 'w') as genfrag_cfi:
                genfrag_cfi.write(genTemplate.format(CTAU=ctau))

            with open(os.path.join(BASEDIR, 'crab/config.yml'), 'w') as singlecrabConf:
                singlecrabConf.write(singlecrabConfigGEN.format(NT=gridpack_name.split('_slc')[0], NJ=njobs))

            os.system(cmd2)
            time.sleep(10) # give some time to crab
            donelist.append(gridpack)

    print('submitted: ', len(donelist))
    for x in donelist: print(x)
    print('------------------------------------------------------------')

    undonelist = [x for x in gridpacks if x not in donelist]
    print('unsubmitted: ', len(undonelist))
    for x in undonelist: print(x)
    if undonelist:
        with open('unsubmitted.yml', 'w') as outf:
            yaml.dump({'gridpacks': undonelist, 'njobs': njobs}, outf, default_flow_style=False)


if __name__ == "__main__":
    main()
