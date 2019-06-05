import FWCore.ParameterSet.Config as cms

triggerObjectMatchingFilter = cms.EDFilter(
    "ffTriggerObjectsMatchingFilter",
    processName=cms.string("HLT"),
    triggerNames=cms.vstring(
        "HLT_DoubleL2Mu23NoVtx_2Cha",
        "HLT_DoubleL2Mu23NoVtx_2Cha_NoL2Matched",
        "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed",
        "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed_NoL2Matched",
        "HLT_DoubleL2Mu25NoVtx_2Cha_Eta2p4",
        "HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed_Eta2p4",
    ),
    triggerResults=cms.InputTag("TriggerResults", "", "HLT"),
    triggerEvent=cms.InputTag("hltTriggerSummaryAOD", "", "HLT"),
    tracks=cms.InputTag("displacedStandAloneMuons"),
    minDr=cms.double(0.3),
    minCounts=cms.uint32(2),
)
