import FWCore.ParameterSet.Config as cms

jetconstituentsubtractor = cms.EDProducer(
    "JetConstituentSubtractor",
    jet=cms.InputTag('ffLeptonJetCHS'),
    cut=cms.string(' && '.join([
        'particleId!=1',
        'particleId!=5',
    ])))
