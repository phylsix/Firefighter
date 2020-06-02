#!/usr/bin/env python
from __future__ import print_function
import os
import sys
import ROOT
from DataFormats.FWLite import Events, Handle

ROOT.gROOT.SetBatch()

events_to_inspect = '''\
317649 172 253595583
321774 1 154583
'''



events = Events(sys.argv[1])

for event in events:
    _run = event.object().id().run()
    _lumi = event.object().luminosityBlock()
    _event = event.object().id().event()
    tag = '{} {} {}'.format(_run, _lumi, _event)
    if tag not in events_to_inspect: continue

    lh = (("cosmicMuons", "", "RECO"), Handle("vector<reco::Track>"))
    event.getByLabel(*lh)
    cosmics = lh[1].product()
    for i, c1 in enumerate(cosmics):
        c1momentum = ROOT.TVector3(c1.px(), c1.py(), c1.pz())
        for j, c2 in enumerate(cosmics):
            if j<=i: continue
            c2momentum = ROOT.TVector3(c2.px(), c2.py(), c2.pz())
            cosalpha = c1momentum.Dot(c2momentum)
            cosalpha /= c1momentum.Mag()*c2momentum.Mag()
            if abs(cosalpha)<0.99: continue
            print( (i,j), abs(cosalpha) )
    print('-'*80)