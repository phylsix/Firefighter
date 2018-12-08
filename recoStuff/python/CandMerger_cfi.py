import FWCore.ParameterSet.Config as cms

candmerger = cms.EDProducer(
    "CandMerger",
    src=cms.VInputTag(
        cms.InputTag("particleFlow"),
        cms.InputTag("dSAmuPFCandFork", "nonMatched")
    )
)