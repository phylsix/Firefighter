#!/usr/bin/env python
from __future__ import print_function
import ROOT
from DataFormats.FWLite import Events, Handle
ROOT.gROOT.SetBatch()


events = Events('../cfg/ljGun_AODSIM.root')

handle = Handle('std::vector<reco::GenParticle>')
label = ('genParticles')

for i, event in enumerate(events):

    print("---------------------------------")
    print("## Event ", i)
    event.getByLabel(label, handle)
    if not handle.isValid(): continue
    genpar = handle.product()

    for p in genpar:

        print(p.pdgId(), end=' ')
    print()
