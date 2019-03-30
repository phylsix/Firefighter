import FWCore.ParameterSet.Config as cms

hltfilter = cms.EDFilter(
    "HLTFilter",
    TriggerEvent=cms.InputTag("hltTriggerSummaryAOD", "", "HLT"),
    TriggerResults=cms.InputTag("TriggerResults", "", "HLT"),
    TriggerPaths=cms.vstring(
        "HLT_DoubleL2Mu23NoVtx_2Cha",
        "HLT_DoubleL2Mu23NoVtx_2Cha_NoL2Matched",
        "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed",
        "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed_NoL2Matched",
    ),
    HltProcName=cms.string("HLT"),
)
