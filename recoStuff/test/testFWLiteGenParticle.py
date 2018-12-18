#!/usr/bin/env python
from __future__ import print_function
import ROOT
from DataFormats.FWLite import Events, Handle
ROOT.gROOT.SetBatch()


# events = Events('skimOutputLeptonJetProd.root')
# events = Events('root://cmseos.fnal.gov//store/user/wsi/MCSIDM/AODSIM/CRAB_PrivateMC/SIDM_BsTo2DpTo4Mu_MBs-200_MDp-1p2_ctau-0p48/181205_161241/0000/SIDM_AODSIM_1.root')
events = Events('root://cmseos.fnal.gov//store/user/wsi/MCSIDM/CRAB_PrivateMC/SIDM_BsTo2DpTo2Mu2e_MBs-400_MDp-1p2_ctau-24/181027_064400/0000/GENSIM_1.root')

print("len(events): ", events.size())

handle = Handle('std::vector<reco::GenParticle>')
label = ('genParticles', '', 'SIM')

for i, event in enumerate(events, 1):

    if i>10: break
    print("---------------------------------")
    print("## Event ", i)
    event.getByLabel(label, handle)
    if not handle.isValid(): continue
    gen = handle.product()

    nGen = len(gen)
    print("Number of gen particles: ", nGen)
    print('pdgId\tstatus\tpt\teta\tphi\tisHP\tmother[0].pdgId')
    for g in gen:
        
        # if abs(g.pdgId())<9 or not g.isHardProcess(): continue
        if abs(g.pdgId()) not in (11,13, 32): continue
        print(g.pdgId(), g.status(), round(g.pt(),3), round(g.eta(),3), round(g.phi(),3), g.isHardProcess(), g.mother(0).pdgId(), sep='\t')
