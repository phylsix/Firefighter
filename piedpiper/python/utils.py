#!/usr/bin/env python
from __future__ import print_function
import os
import subprocess


def get_param_from_gridpackname(gpname):
    '''
    infer parameters to generate the gridpack
    e.g. SIDM_BsTo2DpTo2Mu2e_MBs-1000_MDp-1p2_ctau-9p6_slc6_amd64_gcc481_CMSSW_7_1_30_tarball.tar.xz
    
    returns (mbs, mdp, ctau)
    '''

    params = [x for x in gpname.split('_') if '-' in x]
    mbs, mdp, ctau = None, None, None
    for p in params:
        if 'MBs' in p:
            mbs = float(p.split('-')[1].replace('p', '.'))
        if 'MDp' in p:
            mdp = float(p.split('-')[1].replace('p', '.'))
        if 'ctau' in p:
            ctau = float(p.split('-')[1].replace('p', '.'))

    if not all([mbs, mdp, ctau]):
        print('Cannot infer from gridpack name: "{0}"'.format(gpname))

    return (mbs, mdp, ctau)


def get_nametag_from_dataset(dataset):
    '''
    infer nametag from a dataset string
    e.g.: /CRAB_PrivateMC/wsi-SIDM_BsTo2DpTo4Mu_MBs-200_MDp-1p2_ctau-0p48-354cda32a6a404e25b0eb21bb1bef952/USER

    returns SIDM_BsTo2DpTo4Mu_MBs-200_MDp-1p2_ctau-0p48
    '''

    fullstr = [x for x in dataset.split('/') if 'SIDM' in x]
    if not fullstr: return None
    fullstr = fullstr[0]

    nametag = fullstr.split('-',1)[1]
    nametag = nametag.rsplit('-',1)[0]

    return nametag


def check_voms_valid():
    '''
    check if certificate is expired
    '''

    cmd = 'voms-proxy-info -file /tmp/x509up_u{0} -timeleft'.format(os.getuid())
    p = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
    if p[0] and not p[1]:
        return True
    else:
        return False


def get_voms_certificate():
    '''
    make sure VOMS certificate is valid
    '''

    if not check_voms_valid():
        os.system('voms-proxy-init -voms cms -valid 192:00')
