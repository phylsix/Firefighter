#!/usr/bin/env python
from __future__ import print_function
import os
import sys
import yaml
from tqdm import tqdm
import ROOT
from DataFormats.FWLite import Events, Handle

ROOT.gROOT.SetBatch()

# events_to_inspect = '''\
# 2mu2e 1:57823:131488809
# 2mu2e 1:78664:178881086
# 4mu  1:82610:187854136
# 4mu  1:71514:162622754
# 2mu2e 1:19441:44208166
# 2mu2e 1:70304:159869961
# 4mu  1:18481:42025231'''

events_to_inspect = '''\
2mu2e 1:90061:204798108
2mu2e 1:75879:172548337
2mu2e 1:57823:131488809
2mu2e 1:96800:220122374
2mu2e 1:58349:132683492
2mu2e 1:90350:205453932
2mu2e 1:49206:111893014
2mu2e 1:42661:97010313
2mu2e 1:71334:162211873
2mu2e 1:72428:164700367
2mu2e 1:78664:178881086
2mu2e 1:88539:201337467
2mu2e 1:60986:138681185
2mu2e 1:19441:44208166
2mu2e 1:62505:142134674
2mu2e 1:70304:159869961
2mu2e 1:75509:171706907
2mu2e 1:76489:173934403
2mu2e 1:45909:104394887
2mu2e 1:59905:136223608
2mu2e 1:3682:8371771
'''

f_dy_lt50 = os.path.join(os.getenv('CMSSW_BASE'), 'src/Firefighter/ffConfig/python/production/Skim2LJ18/bkgmc/DYJetsToLL_M-10to50_TuneCP5_13TeV-madgraphMLM-pythia8.yml')
f_dy_gt50 = os.path.join(os.getenv('CMSSW_BASE'), 'src/Firefighter/ffConfig/python/production/Skim2LJ18/bkgmc/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8.yml')
files = []
files.extend( yaml.load(open(f_dy_gt50), Loader=yaml.Loader)['fileList'][0] )

res = []
for f in tqdm(files):
    events = Events(f)
    for event in events:
        _run = event.object().id().run()
        _lumi = event.object().luminosityBlock()
        _event = event.object().id().event()
        tag = '{}:{}:{}'.format(_run, _lumi, _event)
        if tag in events_to_inspect:
            res.append((tag, f))
            # print(tag, f)

for tag, f in res: print(tag, f)