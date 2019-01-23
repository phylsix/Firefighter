#!/usr/bin/env python
from __future__ import print_function
import ROOT
from DataFormats.FWLite import Events, Handle
ROOT.gROOT.SetBatch()

def formatPoint(p):
    return (
        round(p.X(), 3),
        round(p.Y(), 3),
        round(p.Z(), 3)
    )

events = Events('skimOutputLeptonJetProd.root')
print("len(events): ", events.size())

handle = Handle('std::vector<reco::PFCandidate>')
label = ('dsaMuPFCandFork', 'nonMatched', 'USER')
tkHandle = Handle('std::vector<reco::Track>')
tkLabel = ('displacedStandAloneMuons', '', 'RECO')

gtkHandle = Handle('std::vector<reco::Track>')
gtkLabel = ('generalTracks', '', 'RECO')

selectedPFCandHdl = Handle('std::vector<reco::PFCandidate>')
selectedPFCandLbl = ('selectedPFCands', '', 'USER')

genParHdl = Handle('std::vector<reco::GenParticle>')
genParLbl = ('genParticles', '', 'HLT')

for i, event in enumerate(events, 1):

    if i>20: break

    print("="*70, end='\n\n')
    print("## Event ", i)

    event.getByLabel(genParLbl, genParHdl)
    if genParHdl.isValid():
        genparticles = genParHdl.product()
        print(genParLbl)
        print('{:5} ({:^12}) {:6} {:25}'.format(
            'PdgId',
            'eta, phi',
            'Charge',
            'vertex'
        ))
        for p in genparticles:
            if abs(p.pdgId())<9 or not p.isHardProcess(): continue
            if abs(p.pdgId()) not in (11,13, 32): continue
            print('{:5} ({:5}, {:5}) {:6} {:^25}'.format(
                p.pdgId(),
                round(p.eta(), 3),
                round(p.phi(), 3),
                p.charge(),
                str(formatPoint(p.vertex()))
            ))
        print('*'*20, end='\n\n')

    event.getByLabel(tkLabel, tkHandle)
    if tkHandle.isValid():
        dsamu = tkHandle.product()
        if len(dsamu):
            print(tkLabel)
            print('{:5} ({:^12}) {:^25} {:^25}'.format(
                'pT',
                'eta, phi',
                'refPoint',
                'innerPos'
            ))
            for tk in dsamu:
                print('{:5} ({:5}, {:5}) {:^25} {:^25}'.format(
                    round(tk.pt(), 3),
                    round(tk.eta(), 3),
                    round(tk.phi(), 3),
                    str(formatPoint(tk.referencePoint())),
                    str(formatPoint(tk.innerPosition()))
                ))
            print('*'*20, end='\n\n')
        #if len(dsamu):
        #    print(tkLabel, (dsamu.id().processIndex(), dsamu.id().productIndex()))

    event.getByLabel(gtkLabel, gtkHandle)
    if gtkHandle.isValid():
        gtks = gtkHandle.product()
        if len(gtks):
            print(gtkLabel)
            for igtk, gtk in enumerate(gtks, 1):
                if igtk>5: break
                print('vtx ({:.3f}, {:.3f}, {:.3f})'.format(
                    gtk.referencePoint().X(),
                    gtk.referencePoint().Y(),
                    gtk.referencePoint().Z()
                ))
            print('*'*20, end='\n\n')

    event.getByLabel(selectedPFCandLbl, selectedPFCandHdl)
    if selectedPFCandHdl.isValid():
        selectedPFCands = selectedPFCandHdl.product()
        if len(selectedPFCands):
            print(selectedPFCandLbl)
            for ic, cand in enumerate(selectedPFCands,1):
                if ic>5: break
                if cand.trackRef().isNull(): continue
                print('vtx ({:.3f}, {:.3f}, {:.3f})'.format(
                    cand.trackRef().referencePoint().X(),
                    cand.trackRef().referencePoint().Y(),
                    cand.trackRef().referencePoint().Z()
                ))
            print('*'*20, end='\n\n')



    event.getByLabel(label, handle)
    if handle.isValid():
        cands = handle.product()
        if len(cands):
            print(label)
            print('{:5} {:6} {:>8} {:>8} {:^12} {:>8} {:>8}'.format(
                'PdgId',
                'Charge',
                'Time',
                'TimeError',
                'IsTimeValid',
                'parType',
                'hdlId'
                ))
            for c in cands:

                print('{:5} {:6} {:>8} {:>8} {:^12} {:>8} {:>8}'.format(
                    c.pdgId(),
                    c.charge(),
                    c.time(),
                    c.timeError(),
                    str(c.isTimeValid()),
                    c.particleId(),
                    (c.trackRef().id().processIndex(), c.trackRef().id().productIndex()) if c.trackRef().isNonnull() else -1
                    ))
            print('*'*20, end='\n\n')