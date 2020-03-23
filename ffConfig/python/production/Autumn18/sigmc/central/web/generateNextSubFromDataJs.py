#!/usr/bin/env python
from __future__ import print_function
import os
import json
from datetime import datetime

import yaml

def getNextSubmitDatasets():
    f = os.path.join(os.getenv('CMSSW_BASE'),
    'src/Firefighter/ffConfig/python/production/Autumn18/sigmc/central/web/data.js')
    storeInfo = json.loads(open(f).read().replace('var data=', ''))['store']
    res = []
    for entry in storeInfo:
        if entry['submitdir']=='':
            res.append(entry['name'].encode('utf-8'))
    return res


def getNextSubmitYamls(ds):
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
    f = '/publicweb/w/wsi/public/lpcdm/sigprodmon/data.js'
    storeInfo = json.loads(open(f).read().replace('var data=', ''))['store']
    res = []
    for entry in storeInfo:
        if entry['status']=='VALID' and not entry['ondisk']:
            res.append(entry['name'].encode('utf-8'))
    return res


def getOnDiskDatasets():
    f = '/publicweb/w/wsi/public/lpcdm/sigprodmon/data.js'
    storeInfo = json.loads(open(f).read().replace('var data=', ''))['store']
    res = []
    for entry in storeInfo:
        if entry['status']=='VALID' and entry['ondisk']:
            res.append(entry['name'].encode('utf-8'))
    return res



def main():
    outputf = os.path.join(
        os.getenv('CMSSW_BASE'),
        'src/Firefighter/ffConfig/python/batchYmls',
        'submission_{}.yml'.format(datetime.today().strftime('%y%m%d'))
    )
    tosubs = getNextSubmitDatasets()
    with open(outputf, 'w') as f:
        f.write(yaml.dump( getNextSubmitYamls(tosubs), default_flow_style=False ))
    print('Saved to:', outputf)

    print('-'*50)

    dummies = [d for d in getNextDummyDatasets() if d not in tosubs]
    outputf = os.path.join(
        os.getenv('CMSSW_BASE'),
        'src/Firefighter/ffConfig/python/batchYmls',
        'dummysub_{}.yml'.format(datetime.today().strftime('%y%m%d'))
    )
    with open(outputf, 'w') as f:
        f.write(yaml.dump( getNextSubmitYamls(dummies), default_flow_style=False ))
    print('Saved to:', outputf)

    ondiskones = getOnDiskDatasets()
    outputf = os.path.join(
        os.getenv('CMSSW_BASE'),
        'src/Firefighter/ffConfig/python/batchYmls/centralSig_onDisk.yml',
    )
    with open(outputf, 'w') as f:
        f.write(yaml.dump( getNextSubmitYamls(ondiskones), default_flow_style=False ))
    print('Saved to:', outputf)

if __name__ == "__main__":
    main()