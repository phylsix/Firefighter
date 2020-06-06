#!/usr/bin/env python
from __future__ import print_function
import os
import json
import urllib2
from datetime import datetime

import yaml

def getNextSubmitDatasets():
    """list datasets whose submitDir is emtpy"""

    f = os.path.join(os.getenv('CMSSW_BASE'),
    'src/Firefighter/ffConfig/python/production/Autumn18/sigmc/central/web/data.js')
    storeInfo = json.loads(open(f).read().replace('var data=', ''))['store']
    res = []
    for entry in storeInfo:
        if entry['submitdir']=='':
            res.append(entry['name'].encode('utf-8'))
    return res


def getTapeRecallDatasets():
    """list datasets whose job status is TAPERECALL"""

    f = os.path.join(os.getenv('CMSSW_BASE'),
    'src/Firefighter/ffConfig/python/production/Autumn18/sigmc/central/web/data.js')
    storeInfo = json.loads(open(f).read().replace('var data=', ''))['store']
    res = []
    for entry in storeInfo:
        if entry['jobstatus']=='TAPERECALL':
            res.append(entry['name'].encode('utf-8'))
    return res


def getTransferDatasets():
    """list datasets whose job status is TAPERECALL"""

    f = os.path.join(os.getenv('CMSSW_BASE'),
    'src/Firefighter/ffConfig/python/production/Autumn18/sigmc/central/web/data.js')
    storeInfo = json.loads(open(f).read().replace('var data=', ''))['store']
    for entry in storeInfo:
        if entry['jobstatus']!='COMPLETED':
            # print(entry['name'])
            print(entry['yamlname'])


def getNextSubmitYamls(ds):
    """translate datasets into corresponding yamls"""

    yamlInStore = json.load(
        open(os.path.join(
            os.getenv('CMSSW_BASE'),
            'src/Firefighter/ffConfig/python/production/Autumn18/sigmc/central/description.json'
        )))
    res = []
    for y in yamlInStore:
        fn = os.path.join(os.getenv('CMSSW_BASE'), y)
        content = yaml.safe_load(open(fn))
        if any([ d in ds for d in content['datasetNames'] ]):
            res.append(y.encode('utf-8'))
    return res


def getNextDummyDatasets():
    """list datasets not fully on disk"""

    if os.getenv('USER')=='wsi':
        f = '/publicweb/w/wsi/public/lpcdm/sigprodmon/data.js'
        storeInfo = json.loads(open(f).read().replace('var data=', ''))['store']
    else:
        msg = urllib2.urlopen('https://home.fnal.gov/~wsi/public/lpcdm/sigprodmon/data.js').read()
        storeInfo = json.loads(msg.replace('var data=', ''))['store']
    res = []
    for entry in storeInfo:
        if entry['ondisk']: continue
        transfering = any(['%' in site for site in entry['sitelist']])
        if not transfering: res.append(entry['name'].encode('utf-8'))
    return res


def getOnDiskDatasets():
    """list datasets which is VALID and on disk"""

    f = '/publicweb/w/wsi/public/lpcdm/sigprodmon/data.js'
    storeInfo = json.loads(open(f).read().replace('var data=', ''))['store']
    res = []
    for entry in storeInfo:
        if entry['status']=='VALID' and entry['ondisk']:
            res.append(entry['name'].encode('utf-8'))
    return res



def main():

    # # ones not submit yet
    # outputf = os.path.join(
    #     os.getenv('CMSSW_BASE'),
    #     'src/Firefighter/ffConfig/python/batchYmls',
    #     'submission_{}.yml'.format(datetime.today().strftime('%y%m%d'))
    # )
    # tosubs = getNextSubmitDatasets()
    # with open(outputf, 'w') as f:
    #     f.write(yaml.dump( getNextSubmitYamls(tosubs), default_flow_style=False ))
    # print('Saved to:', outputf)

    # print('-'*50)

    # # on tape (really on tape)
    dummies = getNextDummyDatasets()
    outputf = os.path.join(
        os.getenv('CMSSW_BASE'),
        'src/Firefighter/ffConfig/python/batchYmls',
        'dummysub_{}.yml'.format(datetime.today().strftime('%y%m%d'))
    )
    with open(outputf, 'w') as f:
        f.write(yaml.dump( getNextSubmitYamls(dummies), default_flow_style=False ))
    print('Saved to:', outputf)

    # # ondisk
    # ondiskones = getOnDiskDatasets()
    # outputf = os.path.join(
    #     os.getenv('CMSSW_BASE'),
    #     'src/Firefighter/ffConfig/python/batchYmls/centralSig_onDisk.yml',
    # )
    # with open(outputf, 'w') as f:
    #     f.write(yaml.dump( getNextSubmitYamls(ondiskones), default_flow_style=False ))
    # print('Saved to:', outputf)

    # # taperecall
    # taperecallones = getTapeRecallDatasets()
    # outputf = os.path.join(
    #     os.getenv('CMSSW_BASE'),
    #     'src/Firefighter/ffConfig/python/batchYmls/centralSig_tapeRecall.yml',
    # )
    # with open(outputf, 'w') as f:
    #     f.write(yaml.dump( getNextSubmitYamls(taperecallones), default_flow_style=False ))
    # print('Saved to:', outputf)

    #getTransferDatasets()

if __name__ == "__main__":
    main()
