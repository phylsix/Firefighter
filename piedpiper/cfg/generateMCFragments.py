#!/usr/bin/env python
from __future__ import print_function

import os
import yaml
import shutil

from Firefighter.piedpiper.utils import *


def generate_gencfi(config, year):

    gridpack_fullpath = config['gridpack']
    gridpack_name = os.path.basename(gridpack_fullpath)

    shutil.copy(gridpack_fullpath, 'gridpack.tar.xz')
    mbs, mdp, ctau = get_param_from_gridpackname(gridpack_name)

    genfrag_cfi = open('../python/externalLHEProducer_and_PYTHIA8_Hadronizer_cff.py', 'w')
    genfrag_cfi.write(get_gentemplate(year).format(CTAU=ctau))
    genfrag_cfi.close()


if __name__ == "__main__":

    ## Load config file
    config = yaml.load(
            open(os.path.join(
                os.path.dirname(os.path.abspath(__file__)),
                'config.yml'
                )).read()
            )
    # print(config)

    ## Get year
    cmsswrel = os.environ.get('CMSSW_VERSION', '')
    if not cmsswrel: sys.exit('cmsenv first.')

    if os.environ['CMSSW_BASE'] not in os.path.abspath(__file__):
        sys.exit('Wrong release environment, cmsenv again!')

    if 'CMSSW_8' in cmsswrel:
        year = '2016'
    elif 'CMSSW_9' in cmsswrel:
        year = '2017'
    elif 'CMSSW_10' in cmsswrel:
        year = '2018'
    else:
        sys.exit('Unsupported CMSSW release: {0}'.format(cmsswrel))

    ## Generate gen cfi fragment file
    generate_gencfi(config, year)

    ## Generate all configs
    print('==== GEN-SIM step ====')
    cmd = get_command('GEN-SIM', year)
    print(cmd)
    os.system(cmd)

    print('==== PREMIX-RAW-HLT step ====')
    cmd = get_command('PREMIX-RAW-HLT', year)
    print(cmd)
    get_voms_certificate()
    os.system(cmd)

    print('==== AODSIM step ====')
    cmd = get_command('AODSIM', year)
    print(cmd)
    os.system(cmd)
