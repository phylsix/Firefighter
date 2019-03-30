import FWCore.ParameterSet.Config as cms

mckinematicfilter = cms.EDFilter(
    "MCKinematicFilter",
    GenParticles=cms.InputTag("genParticles"),
    pdgId=cms.vint32(11, 13),
    minPt=cms.double(5.0),
)
