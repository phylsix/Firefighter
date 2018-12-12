#!/usr/bin/env python
from __future__ import print_function
import yaml
import os
import shutil
from Firefighter.piedpiper.utils import *
from Firefighter.piedpiper.template import genTemplate


def generate_gencfi(config):

    gridpack_fullpath = config['gridpack']
    gridpack_name = os.path.basename(gridpack_fullpath)

    shutil.copy(gridpack_fullpath, 'gridpack.tar.xz')
    mbs, mdp, ctau = get_param_from_gridpackname(gridpack_name)

    genfrag_cfi = open('../python/externalLHEProducer_and_PYTHIA8_Hadronizer_cff.py', 'w')
    genfrag_cfi.write(genTemplate.format(CTAU=ctau))
    genfrag_cfi.close()

def get_command(step, year):

    if step.upper() not in ['GEN-SIM', 'PREMIX-RAW-HLT', 'AODSIM'] \
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
                'Firefighter/piedpiper/python/externalLHEProducer_and_PYTHIA8_Hadronizer_cff.py',
                '--fileout file:SIDM_GENSIM.root',
                '--mc',
                '-s LHE,GEN,SIM',
                '--era Run2_{0}',
                '--nThreads 4',
                '--conditions auto:phase1_{0}_realistic',
                '--beamspot Realistic25ns13TeVEarly{0}Collision',
                '--datatier GEN-SIM',
                '--eventcontent RAWSIM',
                '-n 10',
                '--no_exec',
                '--python_filename SIDM_GENSIM_cfg.py',
                '--customise Configuration/DataProcessing/Utils.addMonitoring'
            ]
        ).format(year)

    if step == 'PREMIX-RAW-HLT':

        if year == '2017':

            cmd = ' '.join(
                [
                    'cmsDriver.py',
                    'step1',
                    '--filein file:SIDM_GENSIM.root',
                    '--fileout file:SIDM_PREMIXRAWHLT.root',
                    '--mc',
                    '-s DIGIPREMIX_S2,DATAMIX,L1,DIGI2RAW,HLT:@relval{0}',
                    '--era Run2_{0}',
                    '--nThreads 8',
                    '--conditions auto:phase1_{0}_realistic',
                    '--beamspot Realistic25ns13TeVEarly{0}Collision',
                    '--datatier GEN-SIM-RAW',
                    '--eventcontent PREMIXRAW',
                    '--datamix PreMix',
                    '-n -1',
                    '--pileup_input "dbs:/Neutrino_E-10_gun/RunIISummer17PrePremix-MCv2_correctPU_94X_mc2017_realistic_v9-v1/GEN-SIM-DIGI-RAW"',
                    '--no_exec',
                    '--python_filename SIDM_PREMIXRAWHLT_cfg.py',
                    '--customise Configuration/DataProcessing/Utils.addMonitoring'
                ]
            ).format(year)

        elif year == '2018':

            cmd = ' '.join(
                [
                    'cmsDriver.py',
                    'step1',
                    '--filein file:SIDM_GENSIM.root',
                    '--fileout file:SIDM_PREMIXRAWHLT.root',
                    '--mc',
                    '-s DIGI,DATAMIX,L1,DIGI2RAW,HLT:@relval{0}',
                    '--procModifiers premix_stage2',
                    '--era Run2_{0}',
                    '--nThreads 8',
                    '--conditions auto:phase1_{0}_realistic',
                    '--beamspot Realistic25ns13TeVEarly{0}Collision',
                    '--datatier GEN-SIM-RAW',
                    '--eventcontent PREMIXRAW',
                    '--geometry DB:Extended',
                    '--datamix PreMix',
                    '-n -1',
                    '--pileup_input "dbs:/Neutrino_E-10_gun/RunIISummer17PrePremix-PUFull18_102X_upgrade2018_realistic_v11-v1/GEN-SIM-DIGI-RAW"',
                    '--no_exec',
                    '--python_filename SIDM_PREMIXRAWHLT_cfg.py',
                    '--customise Configuration/DataProcessing/Utils.addMonitoring'
                ]
            ).format(year)

    if step == 'AODSIM':

        if year == '2017':

            cmd = ' '.join(
                [
                    'cmsDriver.py',
                    'step2',
                    '--filein file:SIDM_PREMIXRAWHLT.root',
                    '--fileout file:SIDM_AODSIM.root',
                    '--mc',
                    '--runUnscheduled',
                    '-s RAW2DIGI,RECO,EI',
                    '--era Run2_{0}',
                    '--nThreads 8',
                    '--conditions auto:phase1_{0}_realistic',
                    '--datatier AODSIM',
                    '--eventcontent AODSIM',
                    '-n -1',
                    '--no_exec',
                    '--python_filename SIDM_AODSIM_cfg.py',
                    '--customise Configuration/DataProcessing/Utils.addMonitoring'
                ]
            ).format(year)

        elif year == '2018':

            cmd = ' '.join(
                [
                    'cmsDriver.py',
                    'step2',
                    '--filein file:SIDM_PREMIXRAWHLT.root',
                    '--fileout file:SIDM_AODSIM.root',
                    '--mc',
                    '--runUnscheduled',
                    '-s RAW2DIGI,L1Reco,RECO,RECOSIM,EI',
                    '--procModifiers premix_stage2',
                    '--era Run2_{0}',
                    '--nThreads 8',
                    '--conditions auto:phase1_{0}_realistic',
                    '--datatier AODSIM',
                    '--eventcontent AODSIM',
                    '-n -1',
                    '--no_exec',
                    '--python_filename SIDM_AODSIM_cfg.py',
                    '--customise Configuration/DataProcessing/Utils.addMonitoring'
                ]
            ).format(year)

    return cmd




if __name__ == "__main__":

    ## Load config file
    config = yaml.load(
            open(os.path.join(
                os.path.dirname(os.path.abspath(__file__)),
                'config.yml'
                )).read()
            )
    # print(config)

    ## Generate gen cfi fragment file
    #generate_gencfi(config)

    ## Get year
    cmsswrel = os.environ.get('CMSSW_VERSION', '')
    if not cmsswrel: sys.exit('cmsenv first.')

    if 'CMSSW_9' in cmsswrel:
        year = '2017'
    elif 'CMSSW_10' in cmsswrel:
        year = '2018'
    else:
        sys.exit('Unsupported CMSSW release: {0}'.format(cmsswrel))


    ## Generate all configs
    print('==== GEN-SIM step ====')
    cmd = get_command('GEN-SIM', year)
    print(cmd)
    #os.system(cmd)

    print('==== PREMIX-RAW-HLT step ====')
    cmd = get_command('PREMIX-RAW-HLT', year)
    print(cmd)
    #get_voms_certificate()
    #os.system(cmd)

    print('==== AODSIM step ====')
    cmd = get_command('AODSIM', year)
    print(cmd)
    #os.system(cmd)
