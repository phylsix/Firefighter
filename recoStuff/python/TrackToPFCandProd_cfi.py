import FWCore.ParameterSet.Config as cms

from SimGeneral.HepPDTESSource.pythiapdt_cfi import *

PFCandsFromTracks = cms.EDProducer(
    "TrackToPFCandidateProd",
    src=cms.InputTag("displacedStandAloneMuons"),
    particleType=cms.string("mu+"),
)
