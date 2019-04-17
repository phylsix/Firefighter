import FWCore.ParameterSet.Config as cms

pfcandmerger = cms.EDProducer(
    "PFCandMerger",
    src=cms.VInputTag(
        cms.InputTag("particleFlow"), cms.InputTag("dSAmuPFCandFork", "nonMatched")
    ),
)

pfcandfwdptrmerger = cms.EDProducer(
    "PFCandFwdPtrMerger",
    src=cms.VInputTag(
        cms.InputTag("particleFlowPtrs"), cms.InputTag("dSAmuPFCandFork", "nonMatched")
    ),
)
