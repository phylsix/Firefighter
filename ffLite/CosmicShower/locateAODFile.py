#!/usr/bin/env python
from __future__ import print_function
import os
import sys
import yaml
from tqdm import tqdm
import ROOT
from DataFormats.FWLite import Events, Handle

ROOT.gROOT.SetBatch()

events_to_inspect = '''\
317649 172 253595583
321774 1 154583
'''

f_data_B = os.path.join(os.getenv('CMSSW_BASE'), 'src/Firefighter/ffConfig/python/production/Skim2LJ18/data/DoubleMuon_Run2018B.yml')
f_data_D = os.path.join(os.getenv('CMSSW_BASE'), 'src/Firefighter/ffConfig/python/production/Skim2LJ18/data/DoubleMuon_Run2018D.yml')
files = []
files.extend( yaml.load(open(f_data_D), Loader=yaml.Loader)['fileList'][0] )

res = []
for f in tqdm(files):
    events = Events(f)
    for event in events:
        _run = event.object().id().run()
        _lumi = event.object().luminosityBlock()
        _event = event.object().id().event()
        tag = '{} {} {}'.format(_run, _lumi, _event)
        if tag in events_to_inspect:
            res.append((tag, f))
            print(tag, f)

for tag, f in res: print(tag, f)