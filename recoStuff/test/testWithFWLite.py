#!/usr/bin/env python
from __future__ import print_function
import ROOT
from DataFormats.FWLite import Events, Handle
ROOT.gROOT.SetBatch()


events = Events('skimOutputLeptonJetProd.root')
print("len(events): ", events.size())

handle = Handle('std::vector<reco::PFCandidate>')
label = ('dsaMuPFCandFork', 'nonMatched', 'USER')

for i, event in enumerate(events):

    print("---------------------------------")
    print("## Event ", i)
    event.getByLabel(label, handle)
    if not handle.isValid(): continue
    cands = handle.product()

    nCands = len(cands)
    print("Number of candidates: ", nCands)
    for c in cands:

        print()
        print("PdgId       : ", c.pdgId())
        print("charge      ; ", c.charge())
        print("Time        : ", c.time())
        print("TimeError   : ", c.timeError())
        print("isTimeValid : ", c.isTimeValid())
