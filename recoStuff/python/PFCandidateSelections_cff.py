import FWCore.ParameterSet.Config as cms

from CommonTools.ParticleFlow.pfNoPileUpJME_cff import *
from Firefighter.recoStuff.ffRecoSwitcher import switches

# src for ak4pfCHS
pfNoPileUpJMESeq = cms.Sequence(
    goodOfflinePrimaryVertices + pfPileUpJME + pfNoPileUpJME
)

filteredPFCandsFwdPtr = cms.EDFilter(
    "PFCandidateFwdPtrCollectionStringFilter",
    src=cms.InputTag("pfNoPileUpJME"),
    cut=cms.string("abs(eta)<2.5"),
    makeClones=cms.bool(True),
)

if switches["usingCHS"] == False:
    filteredPFCandsFwdPtr.src = cms.InputTag("particleFlowPtrs")
    filteredPFCandsFwdPtr.cut = cms.string(
        " && ".join(["abs(eta)<2.5", "particleId!=1", "particleId!=5"])
    )

filteredPFCands = cms.EDProducer(
    "PFCandidateProductFromFwdPtrProducer", src=cms.InputTag("filteredPFCandsFwdPtr")
)


if switches["usingCHS"] == False:
    filteringPFCands = cms.Sequence(filteredPFCandsFwdPtr + filteredPFCands)
else:
    filteringPFCands = cms.Sequence(
        pfNoPileUpJMESeq + filteredPFCandsFwdPtr + filteredPFCands
    )