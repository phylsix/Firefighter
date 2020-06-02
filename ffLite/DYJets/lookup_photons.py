#!/usr/bin/env python
from __future__ import print_function
import os
import sys
import ROOT
from DataFormats.FWLite import Events, Handle

ROOT.gROOT.SetBatch()



events = Events(sys.argv[1])

for event in events:
    lh = (("gedPhotons", "", "RECO"), Handle("vector<reco::Photon>"))
    event.getByLabel(*lh)
    photons = lh[1].product()
    for i, g in enumerate(photons):
        print(i, g.pt(), g.hasPixelSeed(), g.hadronicOverEm())
    print('-'*80)