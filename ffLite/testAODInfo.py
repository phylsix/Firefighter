#!/usr/bin/env python
from __future__ import print_function
import os
import sys
import ROOT
from DataFormats.FWLite import Events, Handle
import Firefighter.ffLite.utils as fu
from Firefighter.ffConfig.dataSample import samples

ROOT.gROOT.SetBatch()

###############################################################################
dataType = sys.argv[1]
try:
    fn = samples[dataType]
except KeyError:
    msg = 'ERROR: sample for key "{}" not found!\nAvailable keys: {}'.format(
        dataType, list(samples.keys())
    )
    sys.exit(msg)
###############################################################################


def pfCandStuff(event, hl):
    h, l = hl["pfcand"]
    event.getByLabel(l, h)
    if not h.isValid():
        return

    pfcand = h.product()
    print("[PFCand] <pdgId>", [c.pdgId() for c in pfcand])


###############################################################################


def cosmicInAODStuff(event, hl):

    event.getByLabel(hl["cosmicOneLegInAOD"][1], hl["cosmicOneLegInAOD"][0])
    if not hl["cosmicOneLegInAOD"][0].isValid():
        return
    cosmicMuonOneLeg = hl["cosmicOneLegInAOD"][0].product()
    print("[cosmicOneLegInAOD]")
    for c in cosmicMuonOneLeg:
        c.bestTrackRef().hitPattern().print(0)


###############################################################################


def cosmicStuff(event, hl):
    """Only functional for Cosmics dataset

    Parameters
    ----------
    event : [type]
        event
    hl : dict
        stores all the (handle, label)
    """
    event.getByLabel(hl["cosmicMuons"][1], hl["cosmicMuons"][0])
    if not hl["cosmicMuons"][0].isValid():
        return
    event.getByLabel(hl["cosmicMuonsOneLeg"][1], hl["cosmicMuonsOneLeg"][0])
    if not hl["cosmicMuonsOneLeg"][0].isValid():
        return

    cosmicMuons = hl["cosmicMuons"][0].product()
    cosmicMuonsOneLeg = hl["cosmicMuonsOneLeg"][0].product()

    print(
        "[cosmicMuons      ] <time>",
        [
            "{:.3f}/{:.3f}>{}".format(
                m.time().timeAtIpInOut, m.time().timeAtIpOutIn, m.time().direction()
            )
            for m in cosmicMuons
            if m.isTimeValid()
        ],
    )

    print(
        "[cosmicMuonsOneLeg] <time>",
        [
            "{:.3f}/{:.3f}>{}".format(
                m.time().timeAtIpInOut, m.time().timeAtIpOutIn, m.time().direction()
            )
            for m in cosmicMuonsOneLeg
            if m.isTimeValid()
        ],
    )


###############################################################################


def beamhaloStuff(e, hl):

    e.getByLabel(hl["beamhalo"][1], hl["beamhalo"][0])
    if not hl["beamhalo"][0].isValid():
        return

    beamhalo = hl["beamhalo"][0].product()

    print("[beamhalo] <CSCTightHaloId2015>", beamhalo.CSCTightHaloId2015())
    print("[beamhalo] <GlobalTightHaloId2016>", beamhalo.GlobalTightHaloId2016())
    print(
        "[beamhalo] <GlobalSuperTightHaloId2016>", beamhalo.GlobalSuperTightHaloId2016()
    )


###############################################################################


def debugRecoMuon(e, hl):

    e.getByLabel(hl["recomuon"][1], hl["recomuon"][0])
    if not hl["recomuon"][0].isValid():
        return

    recomuon = hl["recomuon"][0].product()
    print("[recoMuon] <charge>", *[m.charge() for m in recomuon])
    print("[recoMuon] <p3>", *[fu.formatP3(m) for m in recomuon])


###############################################################################


def debugDsaMuon(e, hl):

    e.getByLabel(hl["dsa"][1], hl["dsa"][0])
    if not hl["dsa"][0].isValid():
        return

    dsamuon = hl["dsa"][0].product()
    print("[dSAMuon] <charge>", *[m.charge() for m in dsamuon])
    print("[dSAMuon] <p3>", *[fu.formatP3(m) for m in dsamuon])


###############################################################################


def main():

    handlesAndLabels = {
        "pfcand": (
            Handle("std::vector<edm::FwdPtr<reco::PFCandidate>>"),
            ("particleFlowPtrs", "", "RECO"),
        ),
        "cosmicInAOD": (
            Handle("std::vector<reco::Muon>"),
            ("muonsFromCosmics", "", "RECO"),
        ),
        "cosmicOneLegInAOD": (
            Handle("std::vector<reco::Muon>"),
            ("muonsFromCosmics1Leg", "", "RECO"),
        ),
        "beamhalo": (Handle("reco::BeamHaloSummary"), ("BeamHaloSummary", "", "RECO")),
        "recomuon": (Handle("std::vector<reco::Muon>"), ("muons", "", "RECO")),
        "dsa": (
            Handle("std::vector<reco::Track>"),
            ("displacedStandAloneMuons", "", "RECO"),
        ),
    }

    handlesAndLabels_cosmicsPD = {
        # only in Cosmics
        "cosmicMuons": (Handle("std::vector<reco::Muon>"), ("muons", "", "RECO")),
        "cosmicMuonsOneLeg": (
            Handle("std::vector<reco::Muon>"),
            ("muons1Leg", "", "RECO"),
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

        # pfCandStuff(event, handlesAndLabels)
        # cosmicStuff(event, handlesAndLabels_cosmicsPD)
        # cosmicInAODStuff(event, handlesAndLabels)
        # beamhaloStuff(event, handlesAndLabels)
        debugRecoMuon(event, handlesAndLabels)
        debugDsaMuon(event, handlesAndLabels)

        wentThroughEvents.append((_run, _lumi, _event))

    print("+" * 79)
    print("Processed run|lumi|event", *wentThroughEvents, sep="\n")


if __name__ == "__main__":
    main()
