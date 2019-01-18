#!/usr/bin/env python
from __future__ import print_function
import ROOT
import math
from functools import reduce
import numpy as np
from DataFormats.FWLite import Events, Handle
ROOT.gROOT.SetBatch()

def getVertex(par):
    return (
        round(par.vertex().X(), 3),
        round(par.vertex().Y(), 3),
        round(par.vertex().Z(), 3)
    )

def vertexRho(par):
    return math.hypot(
        par.vertex().X(),
        par.vertex().Y()
    )

def vertexMag(par):
    return reduce(math.hypot, getVertex(par))

def vertexDistance(vtx0, vtx1):
    diff = np.array(vtx0) - np.array(vtx1)
    return reduce(math.hypot, diff)

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
    print('{:5} {:6} {:>8} {:>8} {:>8} {:^6} {:^8} {:^10} {:<15} {:^30} {:>8} {:>8} {:>8}'.format(
        'pdgId',
        'status',
        'pT',
        'eta',
        'phi',
        'isHP',
        'mom0.pId',
        'nDaughters',
        'dauPids',
        'vtxCoord',
        'vxy',
        'vl',
        'distMom'
        ))
    for g in gen:

        if abs(g.pdgId())<9 or not g.isHardProcess(): continue
        if abs(g.pdgId()) not in (11,13, 32): continue
        print(
            '{:5} {:6} {:>8} {:>8} {:>8} {:^6} {:^8} {:^10} {:<15} {:^30} {:>8} {:>8} {:>8}'.format(
                g.pdgId(),
                g.status(),
                round(g.pt(),3),
                round(g.eta(),3),
                round(g.phi(),3),
                str(g.isHardProcess()),
                g.mother(0).pdgId(),
                g.numberOfDaughters(),
                str([g.daughter(i).pdgId() for i in range(g.numberOfDaughters())]),
                str(getVertex(g)),
                round(vertexRho(g), 3),
                round(vertexMag(g), 3),
                round(vertexDistance(getVertex(g), getVertex(g.mother(0))), 3)
            )
        )
