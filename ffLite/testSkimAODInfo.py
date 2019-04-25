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


def debugPFCand(event, hl):
    event.getByLabel(hl["pfcand"][1], hl["pfcand"][0])
    if not hl["pfcand"][0].isValid():
        return

    pfcand = hl["pfcand"][0].product()
    print("[PFCand] <pdgId>", [c.pdgId() for c in pfcand])


def debugDsaAsMuon(e, hl):
    e.getByLabel(hl["muonsFromdSA"][1], hl["muonsFromdSA"][0])
    if not hl["muonsFromdSA"][0].isValid():
        return
    data = hl["muonsFromdSA"][0].product()

    print("[muonsFromdSA] - <pfP4 | p4 | time | trackValid>")
    for mu in data:
        pfp4Info = fu.formatP4(mu.pfP4())
        p4Info = fu.formatP4(mu)
        if mu.isTimeValid():
            tInfo = "{:.3f}/{}".format(mu.time().timeAtIpInOut, mu.time().nDof)
        else:
            tInfo = "-/-"
        if mu.track().isNonnull():
            tkValInfo = mu.track().id()
        else:
            tkValInfo = None
        muMatches = mu.matches()
        matchInfo = str(
            [
                "({}/{}/{})".format(mm.detector(), mm.station(), round(mm.dist(),3))
                for mm in muMatches
            ]
        )
        print("\t", pfp4Info, p4Info, tInfo, tkValInfo, matchInfo)


def debugDsaAsPFCand(e, hl):
    e.getByLabel(hl["pfcandsFromMuondSA"][1], hl["pfcandsFromMuondSA"][0])
    if not hl["pfcandsFromMuondSA"][0].isValid():
        return
    data = hl["pfcandsFromMuondSA"][0].product()

    print("[pfcandsFromMuondSA] - <p4 | muonRef | trackRef>")
    for mu in data:
        p4Info = fu.formatP4(mu)
        mrInfo = (
            fu.formatP4(mu.muonRef()) if mu.muonRef() else "<None, None, None, None>"
        )
        tkInfo = fu.formatP3(mu.trackRef()) if mu.trackRef() else "<None, None, None>"
        print("\t", p4Info, mrInfo, tkInfo)


def main():

    handlesAndLabels = {
        "pfcand": (
            Handle("std::vector<edm::FwdPtr<reco::PFCandidate>>"),
            ("particleFlowIncDSA", "", "FF"),
        ),
        "muonsFromdSA": (
            Handle("std::vector<reco::Muon>>"),
            ("muonsFromdSA", "", "FF"),
        ),
        "pfcandsFromMuondSA": (
            Handle("std::vector<reco::PFCandidate>"),
            ("pfcandsFromMuondSA", "", "FF"),
        ),
    }

    events = Events(fn)
    print("Sample's event size: ", events.size())

    wentThroughEvents = []
    for i, event in enumerate(events, 1):

        if i > 20:
            break

        print("\n", "-" * 75, [i])

        _run = event.object().id().run()
        _lumi = event.object().luminosityBlock()
        _event = event.object().id().event()

        # debugPFCand(event, handlesAndLabels)
        debugDsaAsMuon(event, handlesAndLabels)
        # debugDsaAsPFCand(event, handlesAndLabels)

        wentThroughEvents.append((_run, _lumi, _event))

    print("+" * 79)
    print("Processed run|lumi|event", *wentThroughEvents, sep="\n")


if __name__ == "__main__":
    main()
