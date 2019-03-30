import FWCore.ParameterSet.Config as cms

from CommonTools.ParticleFlow.pfNoPileUpJME_cff import *

# src for ak4pfCHS
pfNoPileUpJMESeq = cms.Sequence(goodOfflinePrimaryVertices + pfPileUpJME +
                                pfNoPileUpJME)

filteredPFCandsFwdPtr = cms.EDFilter(
    'PFCandidateFwdPtrCollectionStringFilter',
    src=cms.InputTag('pfNoPileUpJME'),
    # src=cms.InputTag('particleFlowPtrs'),
    cut=cms.string(' && '.join([
        'abs(eta)<2.5',
        # 'particleId!=1',
        # 'particleId!=5',
    ])),
    makeClones=cms.bool(True))

filteredPFCands = cms.EDProducer(
    'PFCandidateProductFromFwdPtrProducer',
    src=cms.InputTag('filteredPFCandsFwdPtr'))

filteringPFCands = cms.Sequence(pfNoPileUpJMESeq + filteredPFCandsFwdPtr +
                                filteredPFCands)
