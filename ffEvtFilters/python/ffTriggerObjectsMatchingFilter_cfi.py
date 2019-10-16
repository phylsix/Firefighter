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
        #"HLT_Mu38NoFiltersNoVtxDisplaced_Photon38_CaloIdL",
    ),
    srcCut=cms.string(" && ".join([
        "pt>2.",
        "abs(eta)<=2.4",
        "(hitPattern.cscStationsWithValidHits+hitPattern.dtStationsWithValidHits)>1",
        "(hitPattern.numberOfValidMuonCSCHits+hitPattern.numberOfValidMuonDTHits)>12",
        "(ptError/pt)<1",
    ])),
    triggerResults=cms.InputTag("TriggerResults", "", "HLT"),
    triggerEvent=cms.InputTag("hltTriggerSummaryAOD", "", "HLT"),
    tracks=cms.InputTag("displacedStandAloneMuons"),
    minDr=cms.double(0.3),
    minCounts=cms.int32(2),
    taggingMode=cms.bool(True),
)
