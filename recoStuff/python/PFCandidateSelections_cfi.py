import FWCore.ParameterSet.Config as cms


selectedPFCandidates = cms.EDFilter(
    "GenericPFCandidateSelector",
    src=cms.InputTag("particleFlow"),
    cut=cms.string(' && '.join([
        'abs(eta)<2.5',
        'particleId!=1',
        'particleId!=5',
    ]))
)
