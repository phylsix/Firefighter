import FWCore.ParameterSet.Config as cms

ffcosmiceventfilter = cms.EDFilter(
    "ffCosmicEventFilter",
    src=cms.InputTag("cosmicMuons"),
    minCosAlpha=cms.double(0.99),
    minPt=cms.double(2.),
    maxPairCount=cms.int32(8),
    taggingMode=cms.bool(True),
)