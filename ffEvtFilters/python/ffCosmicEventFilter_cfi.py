import FWCore.ParameterSet.Config as cms

ffcosmiceventfilter = cms.EDFilter(
    "ffCosmicEventFilter",
    src=cms.InputTag("cosmicMuons"),
    minCosAlpha=cms.double(0.99),
    maxPairCount=cms.int32(10),
    taggingMode=cms.bool(True),
)