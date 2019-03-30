#!/usr/bin/env python
from __future__ import print_function
import os
import ROOT
from DataFormats.FWLite import Events, Handle

ROOT.gROOT.SetBatch()

events = Events(
    os.path.join(
        os.getenv("CMSSW_BASE"),
        "src/Firefighter/recoStuff/test",
        "skimOutputLeptonJetProd.root",
    )
)

handle = {
    "PFCand": Handle("std::vector<reco::PFCandidate>"),
    "muon": Handle("std::vector<reco::Muon>"),
    "nmDSA": Handle("std::vector<reco::PFCandidate>"),
    "mDSA": Handle("std::vector<reco::PFCandidate>"),
}
label = {
    "PFCand": ("particleFlow", "", "RECO"),
    "muon": ("muons", "", "RECO"),
    "nmDSA": ("dsaMuPFCandFork", "nonMatched", "USER"),
    "mDSA": ("dsaMuPFCandFork", "matched", "USER"),
}

for i, event in enumerate(events):

    print("---------------------------------")
    print("## Event ", i)
    pfValid = event.getByLabel(label["PFCand"], handle["PFCand"])
    muValid = event.getByLabel(label["muon"], handle["muon"])
    nmdsaValid = event.getByLabel(label["nmDSA"], handle["nmDSA"])
    mdsaValid = event.getByLabel(label["mDSA"], handle["mDSA"])
    if not (pfValid and muValid and nmdsaValid and mdsaValid):
        continue

    pfcands = handle["PFCand"].product()
    mus = handle["muon"].product()
    nmDSAs = handle["nmDSA"].product()
    mDSAs = handle["mDSA"].product()

    muInPFCands = [p for p in pfcands if abs(p.pdgId()) == 13]

    print("-- Mu in PFCands        : ", len(muInPFCands))
    print("-- Mu in Muons          : ", len(mus))
    print("-- Mu in nonMatched dSA : ", len(nmDSAs))
    print("-- Mu in matched dSA    : ", len(nmDSAs))

    # for p in pfcands:
    #     if abs(p.pdgId())!=13: continue
    #     print()

    #     if p.trackRef().isNull(): continue
    #     hp = p.trackRef().hitPattern()
    #     hp.print(0)
    pfMuTks = [m.trackRef() for m in muInPFCands if m.trackRef().isNonnull()]

    for mu in mus:
        innerTk = mu.innerTrack()
        outerTk = mu.outerTrack()
        if innerTk.isNull() or outerTk.isNull():
            continue
        if innerTk in pfMuTks:
            print("found 1..")
            outerTk.hitPattern().print(0)
        else:
            print("???")
