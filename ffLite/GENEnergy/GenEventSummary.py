#!/usr/bin/env python
from __future__ import print_function
import sys
import ROOT
from DataFormats.FWLite import Events, Handle
from Firefighter.ffConfig.dataSample import samples
from Firefighter.ffLite.utils import getMomId

ROOT.gROOT.SetBatch()
dataType = sys.argv[1]

try:
    fn = samples[dataType]
except KeyError:
    msg = 'ERROR: sample for key "{}" not found!\nAvailable keys: {}'.format(
        dataType, list(samples.keys())
    )
    sys.exit(msg)


def main():

    trigHdl = Handle("edm::TriggerResults")
    trigLbl = ("TriggerResults", "", "HLT")

    genHdl = Handle("std::vector<reco::GenParticle>")
    genLbl = ("genParticles", "", "HLT")

    trigpaths = [
        "HLT_DoubleL2Mu23NoVtx_2Cha",
        "HLT_DoubleL2Mu23NoVtx_2Cha_NoL2Matched",
        "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed",
        "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed_NoL2Matched",
        "HLT_DoubleL2Mu25NoVtx_2Cha",
        "HLT_DoubleL2Mu25NoVtx_2Cha_NoL2Matched",
        "HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed",
        "HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed_NoL2Matched",
        "HLT_DoubleL2Mu25NoVtx_2Cha_Eta2p4",
        "HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed_Eta2p4",
    ]

    events = Events(fn)
    print("Sample's event size: ", events.size())

    wentThroughEvents = []
    lnheadfmt = "{:5} {:6} {:8} {:7} {:7} {:7} {:4}"
    lnfmt = "{:5} {:6} {:8.3f} {:-7.3f} {:-7.3f} {:7} {:4}"
    lnhead_ = ["pid", "status", "energy", "eta", "phi", "mom0pid", "ndau"]
    lnhead = lnheadfmt.format(*lnhead_)
    for i, event in enumerate(events, 1):

        if len(wentThroughEvents) > 10:
            break

        event.getByLabel(trigLbl, trigHdl)
        if not trigHdl.isValid():
            continue

        _run = event.object().id().run()
        _lumi = event.object().luminosityBlock()
        _event = event.object().id().event()

        triggerResults = trigHdl.product()
        names = event.object().triggerNames(triggerResults)
        trigpathsVersioned = map(
            lambda p: [x for x in names.triggerNames() if x.startswith(p)][0], trigpaths
        )
        if not any(
            map(
                lambda p: triggerResults.accept(names.triggerIndex(p)),
                trigpathsVersioned,
            )
        ):
            continue

        event.getByLabel(genLbl, genHdl)
        if not genHdl.isValid():
            continue
        genpars = genHdl.product()

        print("========================")
        print("++ Event {} ++".format(i))
        print("-" * len(lnhead))
        print(lnhead)
        print("-" * len(lnhead))

        for p in genpars:

            # add some filters
            pabspid = abs(p.pdgId())
            if pabspid != 32 and p.numberOfDaughters() != 0:
                continue  # keep darkphotons and final state apriticles
            if pabspid > 999:
                continue  # mostly baryons
            if pabspid in [12, 14, 16]:
                continue  # neutrinos

            pinfo = [
                p.pdgId(),
                p.status(),
                p.energy(),
                p.eta(),
                p.phi(),
                getMomId(p),
                p.numberOfDaughters(),
            ]
            print((lnfmt.format(*pinfo)))

        print("-" * len(lnhead), end="\n\n")
        wentThroughEvents.append("{}:{}:{}".format(_run, _lumi, _event))

    print("*" * 30, " processed events ", "*" * 30)
    for e in wentThroughEvents:
        print(e)


if __name__ == "__main__":
    main()
