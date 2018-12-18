import FWCore.ParameterSet.Config as cms


selectedPFCandidates = cms.EDFilter(
    "GenericPFCandidateSelector",
    src = cms.InputTag("particleFlow"),
    cut = cms.string(' && '.join([
        'charge!=0',
        'pt>2.',
        'abs(eta)<2.5'
    ]))
)