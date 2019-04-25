import FWCore.ParameterSet.Config as cms

from SimGeneral.HepPDTESSource.pythiapdt_cfi import *

PFCandsFromMuons = cms.EDProducer(
    "MuonToPFCandidateProd",
    src=cms.InputTag("muonsFromdSA"),
    particleType=cms.string("mu+"),
)
