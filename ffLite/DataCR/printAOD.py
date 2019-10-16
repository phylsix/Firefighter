#!/usr/bin/env python
"""print information from AOD"""
from __future__ import print_function
from DataFormats.FWLite import Events, Handle

def printmuon(event):
    dsaHdl = Handle("vector<reco::Track>")
    event.getByLabel(("displacedStandAloneMuons", "", "RECO"), dsaHdl)
    assert(dsaHdl.isValid())

    for mu in dsaHdl.product():
        print("pT: {:.3}GeV (eta, phi): ({:.3}, {:.3})".format(mu.pt(), mu.eta(), mu.phi()))
        print("track.innerPosition().x()-track.vx():", "{:.3}-{:.3}={:.3}".format(mu.innerPosition().x(), mu.vx(), mu.innerPosition().x()-mu.vx()))
        print("track.innerPosition().y()-track.vy():", "{:.3}-{:.3}={:.3}".format(mu.innerPosition().y(), mu.vy(), mu.innerPosition().x()-mu.vy()))


def printsegments(event):

    cscsegHdl = Handle("edm::RangeMap<CSCDetId,edm::OwnVector<CSCSegment,edm::ClonePolicy<CSCSegment> >,edm::ClonePolicy<CSCSegment> >")
    event.getByLabel(("cscSegments", "", "RECO"), cscsegHdl)
    assert(cscsegHdl.isValid())
    print("N(CSC) seg:", len(cscsegHdl.product()))
    dtsegHdl = Handle("edm::RangeMap<DTChamberId,edm::OwnVector<DTRecSegment4D,edm::ClonePolicy<DTRecSegment4D> >,edm::ClonePolicy<DTRecSegment4D> >")
    event.getByLabel(("dt4DSegments", "", "RECO"), dtsegHdl)
    assert(dtsegHdl.isValid())
    print("N(DT) seg:", len(dtsegHdl.product()))


def dumpevent(f):

    events = Events(f)
    for i, event in enumerate(events, 1):
        _run = event.object().id().run()
        _lumi = event.object().luminosityBlock()
        _event = event.object().id().event()

        eventId = '{}-{}-{}'.format(_run, _lumi, _event)
        print(eventId.center(60, '_'))

        # printsegments(event)
        printmuon(event)


if __name__ == "__main__":

    f = '/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14_EGamma/src/Firefighter/ffConfig/crabGarage/191004a/results/pickevents2018C_1.root'
    f = '/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14_EGamma/src/Firefighter/ffConfig/crabGarage/191004a/results/pickevents2018D_merged.root'
    # f = '/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14/src/Firefighter/ffConfig/crabGarage/190825/results/pickevents_merged_ABC.root'
    # f = '/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14/src/Firefighter/ffConfig/crabGarage/190825/results/pickevents_merged_D.root'
    f = 'root://cmseos.fnal.gov///store/user/wsi/QCD_Pt-120to170_MuEnrichedPt5_TuneCP5_13TeV_pythia8/crab_pickEvents/191010_151434/0000/pickedevents_QCD-Pt-120To170ext_3.root'
    print(f)
    dumpevent(f)