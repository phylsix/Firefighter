import FWCore.ParameterSet.Config as cms

pfcandmerger = cms.EDProducer(
    "PFCandMerger",
    src=cms.VInputTag(
        cms.InputTag("particleFlow"), cms.InputTag("dSAmuPFCandFork", "nonMatched")
    ),
)
