#!/usr/bin/env python
from __future__ import print_function
import subprocess
import shlex
import os
import json
import shutil
from datetime import datetime

import yaml

def injectedSignalDatasets():
    cmd = 'dasgoclient -query="dataset status=VALID dataset=/SIDM*/*/AODSIM"'
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


def checkExistOnEOS(ds):
    """check if a given dataset name(str) has been processed
    e.g. /SIDM_XXTo2ATo4Mu_mXX-500_mA-1p2_ctau-9p6_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM
    """
    EOSBASE = '/store/group/lpcmetx/SIDM/ffNtupleV2/2018/'
    toCheck = os.path.join(EOSBASE, ds[1:]).rsplit('/', 1)[0]
    cmd = 'eos root://cmseos.fnal.gov ls -s {}'.format(toCheck)
    out, err = subprocess.Popen(shlex.split(cmd),stdout = subprocess.PIPE, stderr= subprocess.PIPE).communicate()

    return err==''


def getLastNtupleFiles(ds):
    EOSBASE = '/store/group/lpcmetx/SIDM/ffNtupleV2/2018/'
    toCheck = os.path.join(EOSBASE, ds[1:]).rsplit('/', 1)[0]
    cmd = 'eos root://cmseos.fnal.gov ls {}'.format(toCheck)
    timestamps = subprocess.check_output(shlex.split(cmd)).split()
    latest = sorted(timestamps, key=lambda x: datetime.strptime(x, "%y%m%d_%H%M%S"))[-1]
    lastdir = os.path.join(toCheck, latest)
    cmd = 'eos root://cmseos.fnal.gov find -f --xurl {}'.format(lastdir)
    res = [f for f in subprocess.check_output(shlex.split(cmd)).split() if '/failed' not in f]
    return latest, res


def translateDStoYaml(d):
    """translate dataset into corresponding yaml"""

    yamlInStore = json.load(
        open(os.path.join(
            os.getenv('CMSSW_BASE'),
            'src/Firefighter/ffConfig/python/production/Autumn18/sigmc/central/description.json'
        )))
    res = ''
    for y in yamlInStore:
        fn = os.path.join(os.getenv('CMSSW_BASE'), y)
        content = yaml.safe_load(open(fn))
        if d in content['datasetNames']:
            res = y.encode('utf-8')
            break
    return res


def fetchFilterEfficiency(d):
    """get gen filter efficiencies given a dataset d"""

    fn = os.path.join(
        os.getenv('CMSSW_BASE'),
        'src/Firefighter/ffConfig/python/production/Autumn18/sigmc/central/genFilterEfficiencies.yml'
    )
    effstore = yaml.safe_load(open(fn))
    res = ''
    pd = d.split('/')[1]
    for y, e in effstore.items():
        m = y.split('_') # ['XXTo2ATo4Mu', 'mXX-800', 'mA-1p2', 'lxy-0p3', 'ctau-0p012']
        m.pop(3)
        y_ = '_'.join(m)
        if y_ in pd:
            res = e
            break
    return res


def jobStatusFromSubmissionDirs():

    def getDSN(d):
        """get primary dataset name from crab submit dir name
        e.g. crab_2018_SIDM_XXTo2ATo2Mu2E_mXX-1000_mA-1p2_ctau-0p96_TuneCP5_13TeV-madgraph-pythia8_v1_200320-232512
        ==>  SIDM_XXTo2ATo2Mu2E_mXX-1000_mA-1p2_ctau-0p96_TuneCP5_13TeV-madgraph-pythia8
        """
        return d.split('pythia8')[0].split('_', 2)[-1]+'pythia8'

    def getDST(d):
        """get crab submit time from crab submit dir name
        e.g. crab_2018_SIDM_XXTo2ATo2Mu2E_mXX-1000_mA-1p2_ctau-0p96_TuneCP5_13TeV-madgraph-pythia8_v1_200320-232512
        ==>  200320-232512
        """
        return d.rsplit('_', 1)[-1]

    def checkGarage(garage, dates):
        for date in dates:
            fc = os.path.join(garage, date, 'crabcompleted.json')
            fnc = os.path.join(garage, date, 'crabnoncompleted.json')
            print(datetime.fromtimestamp(os.path.getmtime(fc)).strftime('%Y-%m-%d %H:%M:%S'), fc)
            print(datetime.fromtimestamp(os.path.getmtime(fc)).strftime('%Y-%m-%d %H:%M:%S'), fnc)

            for d in json.load(open(fc)):
                pd, st = getDSN(d), getDST(d)
                if pd in res and datetime.strptime(st, '%y%m%d-%H%M%S') < datetime.strptime(res[pd]['time'], '%y%m%d-%H%M%S'): continue
                res[pd] = {
                    'submitdir': os.path.join(garage, date, d),
                    'status': 'COMPLETED',
                    'time': st,
                }
            for d, stat in json.load(open(fnc)).items():
                if stat=='SUBMITTEDFAILED': continue
                pd, st = getDSN(d), getDST(d)
                if pd in res and datetime.strptime(st, '%y%m%d-%H%M%S') < datetime.strptime(res[pd]['time'], '%y%m%d-%H%M%S'): continue
                res[pd] = {
                    'submitdir': os.path.join(garage, date, d),
                    'status': stat,
                    'time': st,
                }

    res = {}
    #dates = ['200320', '200321', '200323', '200325', '200330', '200402', '200507']
    dates = ['200618', '200622', '200626', '200629']
    mycrabgarage = '/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14_EGamma/src/Firefighter/ffConfig/crabGarage/'
    checkGarage(mycrabgarage, dates)
    #dates = ['200320', '200321', '200322', '200324']
    #mycrabgarage = '/uscms_data/d3/ranchen/lpcdm/CMSSW_10_2_14_EGamma/src/Firefighter/ffConfig/crabGarage/'
    #checkGarage(mycrabgarage, dates)


    return res


def AODSiteList(ds):
    """return [AOD sitelist, ondisk]"""
    f = '/publicweb/w/wsi/public/lpcdm/sigprodmon/data.js'
    storeInfo = json.loads(open(f).read().replace('var data=', ''))['store']
    res = [[], False]
    for entry in storeInfo:
        if entry['status']=='VALID' and entry['name']==ds:
            res = [entry['sitelist'], entry['ondisk']]
            break
    return res


def assembleInfoForDataset(ds, jobStatuses):
    """assemble info for a given dataset"""
    # jobStatuses = jobStatusFromSubmissionDirs()
    pd = ds.split('/')[1]
    _fromJobDir = jobStatuses.get(pd, {})
    res = dict(
        name=ds, identifier=parseSignalDataset(ds), status=checkExistOnEOS(ds),
        jobstatus=_fromJobDir.get('status', ''),
        submitdir=_fromJobDir.get('submitdir', ''),
        lastcrabtime=_fromJobDir.get('time', ''),
        yamlname=translateDStoYaml(ds),
    )
    _fromEOS = dict(lasteostime='', ntuplefiles=[])
    if res['status']:
        t, fs = getLastNtupleFiles(ds)
        _fromEOS['lasteostime'] = t
        _fromEOS['ntuplefiles'] = fs
    res.update(_fromEOS)

    res['genfiltereff'] =  fetchFilterEfficiency(ds)

    sitelist, ondisk = AODSiteList(ds)
    res['sitelist'] = sitelist
    res['ondisk'] = ondisk

    return res


def main():
    js = jobStatusFromSubmissionDirs()
    store_info = [assembleInfoForDataset(d, js) for d in injectedSignalDatasets()]
    total_info = {'updateTime': datetime.now().strftime('%Y-%m-%d %H:%M:%S'), 'store': store_info}
    with open('data.js', 'w') as outf:
        outf.write('var data={}'.format(json.dumps(total_info, sort_keys=True, indent=4)))
    shutil.copy('data.js', '/publicweb/w/wsi/public/lpcdm/sigsamplemon/')


if __name__ == "__main__":
    main()
