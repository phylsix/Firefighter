#!/usr/bin/env python
from __future__ import print_function
import os
import sys
import ROOT
from DataFormats.FWLite import Events, Handle
import Firefighter.ffLite.utils as fu
from Firefighter.ffConfig.dataSample import skimmedSamples

ROOT.gROOT.SetBatch()

dataType = sys.argv[1]
try:
    fn = skimmedSamples[dataType]
except KeyError:
    msg = 'ERROR: sample for key "{}" not found!\nAvailable keys: {}'.format(
        dataType, list(skimmedSamples.keys())
    )
    sys.exit(msg)


def main():

    handlesAndLabels = {
        "pfcand": (
            Handle("std::vector<edm::FwdPtr<reco::PFCandidate>>"),
            ("particleFlowIncDSA", "", "FF"),
        )
    }

    events = Events(fn)
    print("Sample's event size: ", events.size())

    wentThroughEvents = []
    for i, event in enumerate(events, 1):

        if i > 20:
            break

        print("\n", "-" * 75, [i])
        for h, l in handlesAndLabels.values():
            event.getByLabel(l, h)
            if not h.isValid():
                continue

        _run = event.object().id().run()
        _lumi = event.object().luminosityBlock()
        _event = event.object().id().event()

        pfcand = handlesAndLabels["pfcand"][0].product()
        print("[PFCand] <pdgId>", [c.pdgId() for c in pfcand])


if __name__ == "__main__":
    main()
