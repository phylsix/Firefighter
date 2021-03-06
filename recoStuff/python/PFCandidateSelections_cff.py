import FWCore.ParameterSet.Config as cms

from CommonTools.ParticleFlow.pfNoPileUpJME_cff import *
# from Firefighter.ffConfig.ffConfigSwitch import switches

# src for ak4pfCHS
pfNoPileUpJMESeq = cms.Sequence(
    goodOfflinePrimaryVertices + pfPileUpJME + pfNoPileUpJME
)

filteredPFCandsFwdPtr = cms.EDFilter(
    "PFCandidateFwdPtrCollectionStringFilter",
    src=cms.InputTag("pfNoPileUpJME"),
    cut=cms.string("abs(eta)<2.5"),
    makeClones=cms.bool(False),
)

# if switches["recoStuff"]["usingCHS"] == False:
#     filteredPFCandsFwdPtr.src = cms.InputTag("particleFlowPtrs")
#     filteredPFCandsFwdPtr.cut = cms.string(
#         " && ".join(["abs(eta)<2.5", "particleId!=1", "particleId!=5", "particleId!=6"])
#     )

filteredPFCands = cms.EDProducer(
    "PFCandidateProductFromFwdPtrProducer", src=cms.InputTag("filteredPFCandsFwdPtr")
)


# if switches["recoStuff"]["usingCHS"] == False:
#     filteringPFCands = cms.Sequence(filteredPFCandsFwdPtr + filteredPFCands)
# else:
#     filteringPFCands = cms.Sequence(
#         pfNoPileUpJMESeq + filteredPFCandsFwdPtr + filteredPFCands
#     )

filteringPFCands = cms.Sequence()
# if switches["recoStuff"]["usingCHS"] == False:
#     filteringPFCands = cms.Sequence(filteredPFCandsFwdPtr)
# else:
#     filteringPFCands = cms.Sequence(pfNoPileUpJMESeq + filteredPFCandsFwdPtr)
filteringPFCands_hadFree = cms.Sequence(filteredPFCandsFwdPtr)
filteringPFCands_CHS = cms.Sequence(pfNoPileUpJMESeq + filteredPFCandsFwdPtr)