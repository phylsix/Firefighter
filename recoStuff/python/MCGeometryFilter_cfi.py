import FWCore.ParameterSet.Config as cms

mcgeometryfilter = cms.EDFilter(
    "MCGeometryFilter",
    GenParticles=cms.InputTag("genParticles"),
    pdgId=cms.vint32(11, 13),
    boundR=cms.double(740.0),
    boundZ=cms.double(960.0),
    maxEta=cms.double(2.4),
)
