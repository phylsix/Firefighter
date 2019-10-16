#!/usr/bin/env python
"""
One idea to detect cosmic muon pair is following:
As cosmics are straight lines transversing CMS from top to bottom,
we would expect this quantity to be small:
    pi - deltaphi(phi1, phi0) + |eta0 + eta1|

skimmed data AOD events:

/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14/src/Firefighter/ffConfig/crabGarage/190825/results/pickevents_merged_ABC.root
/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14/src/Firefighter/ffConfig/crabGarage/190825/results/pickevents_merged_D.root
"""
from __future__ import print_function

import os
import sys
from pprint import pprint

import numpy as np
import ROOT
from DataFormats.FWLite import Events, Handle
from Firefighter.ffLite.utils import delta_phi

ROOT.gROOT.SetBatch()

def metric_value(p0, p1):
    return np.pi - delta_phi(p0, p1) + abs(p0.eta()+p1.eta())

def check_dsamuons(e, lhmap, histmap, debug=True):

    if debug:
        print(" check_dsamuons ".center(50, '~'))
    label = 'samuons'
    e.getByLabel(*lhmap[label])
    assert lhmap[label][1].isValid()
    dsamuons = lhmap[label][1].product()

    result = {}
    for i, mu0 in enumerate(dsamuons):
        if mu0.pt()<20: continue
        if abs(mu0.eta())>2.4: continue
        if mu0.hitPattern().numberOfValidMuonHits()<2: continue
        if mu0.normalizedChi2()>10: continue
        metrics = []
        for j, mu1 in enumerate(dsamuons):
            if j<=i: continue
            metrics.append(metric_value(mu0, mu1))
        if metrics:
            minkey = np.array(metrics).argmin()
            minval = np.array(metrics).min()
            result[(i, i+minkey+1)] = minval
            histmap['linear'].Fill(minval)
    if debug and result:
        pprint(result)


def main():

    inputfilename = [
        # '/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14/src/Firefighter/ffConfig/crabGarage/190825/results/pickevents_merged_ABC.root',
        '/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14/src/Firefighter/ffConfig/crabGarage/190825/results/pickevents_merged_D.root',
    ]
    # inputfilename = [
    #     '/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14/src/Firefighter/ffConfig/crabGarage/190825/fromCitron/Run2018A.root',
    #     '/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14/src/Firefighter/ffConfig/crabGarage/190825/fromCitron/Run2018B.root',
    #     '/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14/src/Firefighter/ffConfig/crabGarage/190825/fromCitron/Run2018D.root',
    # ]
    # inputfilename = [
    #     'root://cmsxrootd.fnal.gov///store/data/Run2018A/Cosmics/AOD/06Jun2018-v1/80000/FEBEAF7F-FD71-E811-86DA-782BCB3BCA77.root'
    # ]

    debug=True

    if len(sys.argv) > 1:
        inputfilename = sys.argv[1]
    # assert os.path.isfile(inputfilename)

    lableHandleMap = {
        'recomuons': (("muons", "", "RECO"), Handle("vector<reco::Muon>")),
        'dsamuons': (("displacedStandAloneMuons", "", "RECO"), Handle("vector<reco::Track>")),
        'samuons': (("standAloneMuons", "", "RECO"), Handle("vector<reco::Track>")),
        'cosmicmuons': (('cosmicMuons', '', "RECO"), Handle("vector<reco::Track>"))
    }

    cosmicevents = [171885587, 588925175, 638569038, 1194219475, 1843404323, 767319972, 578254197]
    cosmicevents.extend([99710070, 131158592, 310216566, 12032414, 1382227313, 499511766, 499511766, 599103865, 988893070, 1645464771, 200924923])

    events = Events(inputfilename)
    print("InputFile:", inputfilename)
    print("num_events:", events.size())

    histmap = {}
    histmap['linear'] = ROOT.TH1F('linearity', ';\pi-\Delta\Phi_{0,1}+|\eta_{0}+\eta_{1}|;counts', 50, 0, 1)
    for i, event in enumerate(events):
        if debug and i>100: break
        if not debug and i%10000==0: print(i)
        _run = event.object().id().run()
        _lumi = event.object().luminosityBlock()
        _event = event.object().id().event()

        if _event not in cosmicevents: continue
        if debug:
            print("  {} : {} : {}  ".format(_run, _lumi, _event).center(79, "*"))

        ## tasks
        check_dsamuons(event, lableHandleMap, histmap, debug=debug)

        if debug:
            print("_"*79)
    c = ROOT.TCanvas('c','', 500, 400)
    for h in histmap:
        histmap[h].Draw()
        c.SaveAs('{}_wsi.pdf'.format(h))
        c.Clear()

if __name__ == "__main__":
    main()
