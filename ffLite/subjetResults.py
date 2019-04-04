#!/usr/bin/env python
from __future__ import print_function
import sys
import ROOT
from DataFormats.FWLite import Events, Handle
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

    jetHdl, jetLbl = Handle("std::vector<reco::PFJet>"), ("ffLeptonJet", "", "FF")
    sjetHdl, sjetLbl = (
        Handle("std::vector<reco::BasicJet>"),
        ("ffLeptonJetSubjets", "", "FF"),
    )
    subjetHdl, subjetLbl = (
        Handle("std::vector<reco::PFJet>"),
        ("ffLeptonJetSubjets", "SubJets", "FF"),
    )
    jetSjetMapHdl, jetSjetMapLbl = (
        Handle("edm::Association<edm::View<reco::Jet> >"),
        ("ffLeptonJetSLeptonJetMap", "", "FF"),
    )

    events = Events(fn)
    print("Sample's event size: ", events.size())

    for i, event in enumerate(events, 1):
        if i > 10:
            break

        event.getByLabel(jetLbl, jetHdl)
        assert jetHdl.isValid()
        event.getByLabel(sjetLbl, sjetHdl)
        assert sjetHdl.isValid()
        event.getByLabel(subjetLbl, subjetHdl)
        assert subjetHdl.isValid()
        event.getByLabel(jetSjetMapLbl, jetSjetMapHdl)
        assert jetSjetMapHdl.isValid()

        jets = jetHdl.product()
        sjets = sjetHdl.product()
        subjets = subjetHdl.product()
        jetsjetMap = jetSjetMapHdl.product()

        print("\n+++ Event {} +++".format(i))
        print("-" * 79)

        print("jet size:\t", len(jets))
        print("sjet size:\t", len(sjets))
        print("subjet size:\t", len(subjets))

        print(dir(sjetHdl))
        print(dir(sjets))
        print(dir(jetsjetMap))


if __name__ == "__main__":
    main()
