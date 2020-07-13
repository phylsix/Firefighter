#!/usr/bin/env python
"""GlobalTag from: https://twiki.cern.ch/twiki/bin/viewauth/CMS/PdmVAnalysisSummaryTable
"""

import json
import shlex
import subprocess
from os.path import join, isfile, basename

import yaml
from Firefighter.ffConfig.datasetUtils import ffdatasetsignal, sigmc_ctau2lxy


def get_valid_datasets():
    cmd = "dasgoclient -query='dataset status=VALID dataset=/SIDM*/*/AODSIM'"
    return subprocess.check_output(shlex.split(cmd)).split()

def parseSignalDataset(ds):
    """convert from a dataset name (str) to a tuple
    e.g. /SIDM_XXTo2ATo4Mu_mXX-500_mA-1p2_ctau-9p6_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM
    --> ('4mu', '500', '1.2', '9.6')
    """
    if not ds.startswith('/SIDM'):
        print("cannot recognize dataset name:", ds, 'empty tuple returned.')
        return ()
    identifier = ds.split('/')[1].split('_')
    channel, mxx, ma, ctau = None, None, None, None
    for tag in identifier:
        if tag.startswith('XXTo2A'):
            if '4Mu' in tag: channel = '4mu'
            if '2Mu2E' in tag: channel = '2mu2e'
        if tag.startswith('mXX'):
            mxx = tag.split('-')[-1]
        if tag.startswith('mA'):
            ma = tag.split('-')[-1].replace('p', '.')
        if tag.startswith('ctau'):
            ctau = tag.split('-')[-1].replace('p', '.')

    if not all([channel, mxx, ma, ctau]):
        print('channel: {}, mxx: {}, ma: {}, ctau: {}'.format(channel, mxx, ma, ctau), 'empty tuple returned.')
        return ()

    return (channel, mxx, ma, ctau)



if __name__ == "__main__":

    datasets = {}

    ##########################################################
    ###  Test `sigmc_ctau2lxy`
    # from collections import defaultdict
    # ch2mu2e, ch4mu = defaultdict(list), defaultdict(list)
    # for d in get_valid_datasets():
    #     channel, mxx, ma, ctau = parseSignalDataset(d)
    #     lxy = sigmc_ctau2lxy(float(mxx), float(ma), float(ctau))
    #     if channel == '2mu2e': ch2mu2e[(mxx, ma)].append(lxy)
    #     if channel == '4mu':   ch4mu[(mxx, ma)].append(lxy)

    # for k, v in ch2mu2e.items():
    #     if sorted(v) == [0.3, 3, 30, 150, 300]: continue
    #     print( k, sorted(v) )
    # print('---')
    # for k, v in ch4mu.items():
    #     if sorted(v) == [0.3, 3, 30, 150, 300]: continue
    #     print( k, sorted(v) )
    ##########################################################

    for d in get_valid_datasets():
        channel, mxx, ma, ctau = parseSignalDataset(d)
        lxy = sigmc_ctau2lxy(float(mxx), float(ma), float(ctau))
        if channel == '2mu2e':
            dtag = 'XXTo2ATo2Mu2E_mXX-{}_mA-{}_lxy-{}_ctau-{}'.format(mxx,
            ma.replace('.', 'p'), str(lxy).replace('.', 'p'), ctau.replace('.', 'p'))
        elif channel == '4mu':
            dtag = 'XXTo2ATo4Mu_mXX-{}_mA-{}_lxy-{}_ctau-{}'.format(mxx,
            ma.replace('.', 'p'), str(lxy).replace('.', 'p'), ctau.replace('.', 'p'))
        if dtag in datasets:
            datasets[dtag].append(d)
        else:
            datasets[dtag] = [d]


    saved = None
    if isfile('description.json'):
        try:
            _saved = json.load(open('descripton.json'))
            saved = [basename(d) for d in _saved]
        except:
            pass

    for dtag in datasets:
        fragName = dtag + ".yml"
        if saved and fragName in saved: continue
        flist = []
        for ds in datasets[dtag]:
            subflist = subprocess.check_output(
                shlex.split('dasgoclient -query="file dataset={0}"'.format(ds))
            ).split()
            subflist = [f for f in subflist if f]
            flist.append(sorted(subflist))

        ffds = ffdatasetsignal()
        ffds.datasetNames = datasets[dtag]
        ffds.fileList = flist
        with open(fragName, "w") as f:
            f.write(ffds.dump())


    # summary
    with open('description.json', 'w') as f:
        f.write(json.dumps(sorted(
            [join('src/Firefighter/ffConfig/python/production/Autumn18/sigmc/central/', dtag + '.yml') for dtag in datasets]
            ), indent=4))
    with open('description.yml', 'w') as f:
        f.write(yaml.dump(sorted(
            [join('src/Firefighter/ffConfig/python/production/Autumn18/sigmc/central/', dtag + '.yml') for dtag in datasets]
            ), default_flow_style=False))
