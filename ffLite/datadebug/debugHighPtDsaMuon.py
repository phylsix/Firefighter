#!/usr/bin/env python
"""
When looking at the eta-phi energy map of leptonJets of 2018
data in control region, some hot spot was observed. After initial
investigation, it turns out to be some mis-measured displacedStandAlone
muons giving unphyiscal super high pT, and get clustered into one
of the leptonJets. This is bad and should be avoided.

After skimming those events and observed them in cmsShow, they basically
fall into 3 categories:

1. A reco::Muon is close to the DSA, parallel, and reco::Muon should
    have a better measurement - DSA's detIDs are subsets of reco::Muons
2. A very soft (<5GeV) reco::Muon nearby, but DSA's measurement should
    not be too reliable.
3. Events containing large cosmic muons.

The number of such events is not large (O(10)), nevertheless, we should
come up with something to mitigate this. A brutal way is to mask all DSA
muon whose pT>5000GeV from being clusterd -- could work. But these
observations are also reminding us to make our matching criterian more
robust. Like the overlaping rule.

For cosmic muons events, number of parallel DSA could be a metric, *parallel*
can be defined as |cosAlpha|, 3D opening angle of two tracks' momentum, > 0.99,
if this number > 9, event can be tagged as cosmic events. things like this.

For mismeasured DSA tracks, number of hits and normalized chi2 need to be checked.

Skimed data AOD events are:

/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14/src/Firefighter/ffConfig/crabGarage/190825/results/pickevents_merged_ABC.root
/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14/src/Firefighter/ffConfig/crabGarage/190825/results/pickevents_merged_D.root
"""

from __future__ import print_function
import os
import sys
from collections import defaultdict
import ROOT
from DataFormats.FWLite import Events, Handle
from Firefighter.ffLite.utils import delta_r

ROOT.gROOT.SetBatch()


def inspect_highpt_dsamuon(e, lhmap, counter=defaultdict(int)):

    print(" inspect_highpt_dsamuon ".center(50, "~"))
    e.getByLabel(*lhmap['dsamuons'])
    assert lhmap['dsamuons'][1].isValid()
    dsamuons = lhmap['dsamuons'][1].product()

    e.getByLabel(*lhmap['recomuons'])
    assert lhmap['recomuons'][1].isValid()
    recomuons = lhmap['recomuons'][1].product()

    for dsa in dsamuons:
        if dsa.pt() < 5000:
            continue
        if dsa.pt() > 13000:
            counter['nonphysical'] += 1
        print("pt, eta, phi:", round(dsa.pt(), 3), round(dsa.eta(), 3), round(dsa.phi(), 3))
        print("# valid hits:", dsa.numberOfValidHits())
        print("# valid hits in DT, CSC:", dsa.hitPattern().numberOfValidMuonDTHits(),
                                          dsa.hitPattern().numberOfValidMuonCSCHits())
        # print("# muon stations:", dsa.hitPattern().muonStationsWithValidHits())
        # print("normalized chi2:", round(dsa.normalizedChi2(), 3))
        print("dxy, dz:", round(dsa.dxy(), 3), round(dsa.dz(), 3))

        dsaStationDets = set()
        dsahitpattern = dsa.hitPattern()
        for ih in range(dsahitpattern.numberOfValidMuonHits()):
            hit = dsahitpattern.getHitPattern(0, ih)
            if not dsahitpattern.muonHitFilter(hit): continue
            if not dsahitpattern.validHitFilter(hit): continue
            dsaStationDets.add((dsahitpattern.getMuonStation(hit),
                                dsahitpattern.getSubStructure(hit)))
        print("Sta/Det:", *sorted(dsaStationDets))

        ## closest recoMuon
        # mindr_, idx = 999., -1
        # for i, rm in enumerate(recomuons):
        #     deltar_ = delta_r(dsa, rm)
        #     if deltar_ < mindr_:
        #         mindr_ = deltar_
        #         idx = i
        # recomu = recomuons[idx]
        # print("+++ closest reco::Muon")
        # print("pt, eta, phi:", round(recomu.pt(), 3), round(recomu.eta(), 3), round(recomu.phi(), 3))
        # print("distance:", mindr_)
        # print("has innerTrack/outerTrack:", recomu.innerTrack().isNonnull(), recomu.outerTrack().isNonnull())
        # recomuStationDets = set([(mcm.station(), mcm.detector()) for mcm in recomu.matches()])
        for recomu in recomuons:
            # if delta_r(dsa, recomu) > 0.5: continue

            recomuStationDets = set()
            if recomu.outerTrack().isNonnull():
                recomuhitpattern = recomu.outerTrack().hitPattern()
                for ih in range(recomuhitpattern.numberOfValidMuonHits()):
                    hit = recomuhitpattern.getHitPattern(0, ih)
                    if not recomuhitpattern.muonHitFilter(hit): continue
                    if not recomuhitpattern.validHitFilter(hit): continue
                    recomuStationDets.add((recomuhitpattern.getMuonStation(hit),
                                        recomuhitpattern.getSubStructure(hit)))
            if recomu.outerTrack().isNull() or not recomuStationDets:
                recomuStationDets = set([(mcm.station(), mcm.detector()) for mcm in recomu.matches()])
            issubset_ = dsaStationDets.issubset(recomuStationDets)
            if issubset_:
                print("Sta/Det:", *sorted(recomuStationDets))
                counter['issubset'] += 1
                if dsa.pt()<13000:
                    counter['physical&isubset'] += 1
                print("Distance:", delta_r(dsa, recomu))
                if recomu.innerTrack().isNonnull():
                    counter['issubset&innerTrack'] += 1
                    if dsa.pt()<13000:
                        counter['physical&issubset&innerTrack'] += 1
                break

        # print("dsa is subset of recomu:", issubset_)
        # if issubset_:

        # if issubset_ and recomu.innerTrack().isNonnull():
        #     counter['issubset&innerTrack'] += 1
    return counter


def parallel_dsamuons(e, lhmap):

    print(" parallel_dsamuons ".center(50, "~"))
    e.getByLabel(*lhmap['dsamuons'])
    assert lhmap['dsamuons'][1].isValid()
    dsamuons = lhmap['dsamuons'][1].product()

    e.getByLabel(*lhmap['cosmicmuons'])
    assert lhmap['cosmicmuons'][1].isValid()
    cosmicmuons = lhmap['cosmicmuons'][1].product()

    def cosAlpha(vec0, vec1):
        dotprod = vec0.X()*vec1.X() + vec0.Y()*vec1.Y() + vec0.Z()*vec1.Z()
        return dotprod/(vec0.R()*vec1.R())

    dsaabscosalphas = []
    for i in range(len(dsamuons)):
        for j in range(i+1, len(dsamuons)):
            cosalpha = cosAlpha(dsamuons[i].momentum(), dsamuons[j].momentum())
            dsaabscosalphas.append(abs(cosalpha))

    print("# dsa parallel", len([x for x in dsaabscosalphas if x>0.99]))

    cosmicabscosalphas = []
    for i in range(len(cosmicmuons)):
        for j in range(i+1, len(cosmicmuons)):
            cosalpha = cosAlpha(cosmicmuons[i].momentum(), cosmicmuons[j].momentum())
            cosmicabscosalphas.append(abs(cosalpha))
    print("# cosmic parallel", len([x for x in cosmicabscosalphas if x>0.99]))


def main():

    # inputfilename = '/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14/src/Firefighter/ffConfig/crabGarage/190825/results/pickevents_merged_ABC.root'
    inputfilename = '/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14/src/Firefighter/ffConfig/crabGarage/190825/results/pickevents_merged_D.root'
    if len(sys.argv) > 1:
        inputfilename = sys.argv[1]
    assert os.path.isfile(inputfilename)

    lableHandleMap = {
        'recomuons': (("muons", "", "RECO"), Handle("vector<reco::Muon>")),
        'dsamuons': (("displacedStandAloneMuons", "", "RECO"), Handle("vector<reco::Track>")),
        'cosmicmuons': (('cosmicMuons', '', "RECO"), Handle("vector<reco::Track>"))
    }

    cosmicevents = [171885587, 588925175, 638569038, 1194219475, 1843404323, 767319972, 578254197]
    cosmicevents.extend([99710070, 131158592, 310216566, 12032414, 1382227313, 499511766, 499511766, 599103865, 988893070, 1645464771, 200924923])

    events = Events(inputfilename)
    print("InputFile:", inputfilename)
    print("num_events:", events.size())

    counter = defaultdict(int)
    for i, event in enumerate(events):
        _run = event.object().id().run()
        _lumi = event.object().luminosityBlock()
        _event = event.object().id().event()

        if _event in cosmicevents: continue
        print("  {} : {} : {}  ".format(_run, _lumi, _event).center(79, "*"))
        counter['total'] += 1

        ## tasks
        inspect_highpt_dsamuon(event, lableHandleMap, counter=counter)
        # parallel_dsamuons(event, lableHandleMap)

        print("_"*79)

    print("  counter summary  ".center(80, '+'))
    for k in counter:
        if k == 'total': continue
        print('{}: {}/{}'.format(k, counter[k], counter['total']))


if __name__ == "__main__":
    main()