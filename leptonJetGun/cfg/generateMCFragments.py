#!/usr/bin/env python
from __future__ import print_function
import os
import sys

def get_command(step, year):

    if step.upper() not in ['GEN-SIM', 'DIGI-RAW-HLT', 'AODSIM'] \
        or str(year) not in ['2017', '2018']:
        print('Unsupported parameter for get_command(step, year):')
        print('-- step: {0}'.format(step))
        print('-- year: {0}'.format(year))
        sys.exit()
    
    step = step.upper()
    year = str(year)

    cmd = ''

    if step == 'GEN-SIM':

        cmd = ' '.join(
            [
                'cmsDriver.py',
                'Firefighter/leptonJetGun/python/ljGunProd_cfi.py',
                '--fileout file:ljGun_GENSIM.root',
                '--mc',
                '-s GEN,SIM',
                '--era Run2_{0}',
                '--nThreads 8',
                '--conditions auto:phase1_{0}_realistic',
                '--beamspot Realistic25ns13TeVEarly{0}Collision',
                '--datatier GEN-SIM',
                '--eventcontent RAWSIM',
                '-n 10',
                '--no_exec',
                '--python_filename ljGun_GENSIM_cfg.py',
                '--customise Configuration/DataProcessing/Utils.addMonitoring'
            ]
        ).format(year)
    
    if step == 'DIGI-RAW-HLT':

        cmd = ' '.join(
            [
                'cmsDriver.py',
                'step1',
                '--filein file:ljGun_GENSIM.root',
                '--fileout file:ljGun_DIGIRAWHLT.root',
                '--mc',
                '-s DIGI,L1,DIGI2RAW,HLT:@relval{0}',
                '--era Run2_{0}',
                '--nThreads 8',
                '--conditions auto:phase1_{0}_realistic',
                '--beamspot Realistic25ns13TeVEarly{0}Collision',
                '--datatier GEN-SIM-RAW',
                '--eventcontent RAWSIM',
                '-n 10',
                '--geometry DB:Extended',
                '--no_exec',
                '--python_filename ljGun_DIGIRAWHLT_cfg.py',
                '--customise Configuration/DataProcessing/Utils.addMonitoring'
            ]
        ).format(year)
    
    if step == 'AODSIM':

        cmd = ' '.join(
            [
                'cmsDriver.py',
                'step2',
                '--filein file:ljGun_DIGIRAWHLT.root',
                '--fileout file:ljGun_AODSIM.root',
                '--mc',
                '--runUnscheduled',
                '-s RAW2DIGI,RECO,EI',
                '--era Run2_{0}',
                '--nThreads 8',
                '--conditions auto:phase1_{0}_realistic',
                '--datatier AODSIM',
                '--eventcontent AODSIM',
                '-n 10',
                '--no_exec',
                '--python_filename ljGun_AODSIM_cfg.py',
                '--customise Configuration/DataProcessing/Utils.addMonitoring'
            ]
        ).format(year)
    
    return cmd


if __name__ == "__main__":
    
    cmsswrel = os.environ.get('CMSSW_VERSION', '')
    if not cmsswrel: sys.exit('cmsenv first.')
    
    if 'CMSSW_9' in cmsswrel:
        year = '2017'
    elif 'CMSSW_10' in cmsswrel:
        year = '2018'
    else:
        sys.exit('Unsupported CMSSW release: {0}'.format(cmsswrel))
    
    
    print('==== GEN-SIM step ====')
    cmd = get_command('GEN-SIM', year)
    print(cmd)
    os.system(cmd)

    print('==== DIGI-RAW-HLT step ====')
    cmd = get_command('DIGI-RAW-HLT', year)
    print(cmd)
    os.system(cmd)

    print('==== AODSIM step ====')
    cmd = get_command('AODSIM', year)
    print(cmd)
    os.system(cmd)