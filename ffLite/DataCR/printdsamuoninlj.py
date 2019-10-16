#!/usr/bin/env python
"""print information of dsamuon from leptonjet sources"""
from __future__ import print_function
from DataFormats.FWLite import Events, Handle

def printmuon(mu, event):

    print("pT: {:.3}GeV (eta, phi): ({:.3}, {:.3})".format(mu.pt(), mu.eta(), mu.phi()))
    mutk = mu.outerTrack()
    ncsc = mutk.hitPattern().cscStationsWithValidHits()
    ndt = mutk.hitPattern().dtStationsWithValidHits()
    ncschits = mutk.hitPattern().numberOfValidMuonCSCHits()
    ndthits = mutk.hitPattern().numberOfValidMuonDTHits()
    print("N(DT+CSC):", ndt+ncsc)
    print("N(DT+CSC)hits: ", ndthits+ncschits)
    print("ptErr/pt: {:.3}".format(mutk.ptError()/mutk.pt()))
    print("normalizedChi2: {:.3}".format(mutk.normalizedChi2()))

    overlapratioHdl = Handle("edm::ValueMap<float>")
    event.getByLabel(("dsamuonExtra", "maxSegmentOverlapRatio", "FF"), overlapratioHdl)
    assert(overlapratioHdl.isValid())
    print("overlap ratio:", overlapratioHdl.product().get(mu.id(), mu.key()))

    localdrHdl = Handle("edm::ValueMap<float>")
    event.getByLabel(("dsamuonExtra", "minExtrapolateInnermostLocalDr", "FF"), localdrHdl)
    assert(localdrHdl.isValid())
    print("local dR:", localdrHdl.product().get(mu.id(), mu.key()))

    printmuDetId(mu)
    print("++++++++++++++++++++++++++++++")


def printmuDetId(mu):

    mutk = mu.outerTrack()
    if not mutk.isNonnull(): return
    try:
        hp = mutk.hitPattern()
        dets = []
        for i in range(hp.numberOfAllHits(0)):
            pattern = hp.getHitPattern(0, i)
            if not hp.muonHitFilter(pattern): continue
            substructure = hp.getSubStructure(pattern)
            muonstation = hp.getMuonStation(pattern)
            dtsl, cscrng = -1, -1
            if hp.muonDTHitFilter(pattern):
                dtsl = hp.getDTSuperLayer(pattern)
            if hp.muonCSCHitFilter(pattern):
                cscrng = hp.getCSCRing(pattern)
            if dtsl==-1 and cscrng==-1: continue
            hittype = hp.getHitType(pattern)
            if hittype!=0: continue
            dets.append((substructure, muonstation, dtsl, cscrng))
        for i, t in enumerate(list(set(dets))):
            print("[{}] Sub:MS:DT:CSC -- {}".format(i, t))
    except:
        pass



def dumpevent(f):

    events = Events(f)
    for i, event in enumerate(events, 1):
        _run = event.object().id().run()
        _lumi = event.object().luminosityBlock()
        _event = event.object().id().event()

        eventId = '{}-{}-{}'.format(_run, _lumi, _event)
        print(eventId.center(60, '_'))

        dsaofljHdl = Handle("vector<edm::FwdPtr<reco::PFCandidate> >")
        event.getByLabel(("leptonjetSourceDSAMuon", "inclusive", "FF"), dsaofljHdl)
        assert(dsaofljHdl.isValid())

        for mucand in dsaofljHdl.product():
            mu = mucand.muonRef()
            printmuon(mu, event)

        pfmuofljHdl = Handle("vector<edm::FwdPtr<reco::PFCandidate> >")
        event.getByLabel(("leptonjetSourcePFMuon", "inclusive", "FF"), pfmuofljHdl)
        assert(pfmuofljHdl.isValid())
        for mucand in pfmuofljHdl.product():
            mu = mucand.muonRef()
            printmuDetId(mu)


if __name__ == "__main__":

    f = 'ffSkimV2_DoubleMuon2018C_CR.root'
    print(f)
    dumpevent(f)

    # f = 'ffSkimV2_DoubleMuon2018D_CR.root'
    # print(f)
    # dumpevent(f)