#!/usr/bin/env python
from __future__ import print_function
import os
import ROOT
from DataFormats.FWLite import Events, Handle
ROOT.gROOT.SetBatch()


events = Events(
    os.path.join(
        os.getenv('CMSSW_BASE'),
        'src/Firefighter/recoStuff/test',
        'skimOutputLeptonJetProd.root'
    )
)

handle = Handle('std::vector<reco::Track>')
label = ('selectedDsaMuons', '', 'USER')

for i, event in enumerate(events):

    print("---------------------------------")
    print("## Event ", i)
    hIsValid = event.getByLabel(label, handle)
    if not hIsValid:
        continue
    dsamus = handle.product()

    nmu = len(dsamus)
    print("Number of displacedStandAloneMuons: ", nmu)
    for t in dsamus:
        hp = t.hitPattern()
        hp.print(0)
