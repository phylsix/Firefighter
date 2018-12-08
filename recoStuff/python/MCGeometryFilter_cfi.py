import FWCore.ParameterSet.Config as cms

mcgeometryfilter = cms.EDFilter(
    "MCGeometryFilter",
    GenParticles = cms.InputTag('genParticles'),
    boundR = cms.double(740.),
    boundZ = cms.double(960.),
    maxEta = cms.double(2.4)
)