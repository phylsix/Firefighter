import FWCore.ParameterSet.Config as cms

ffcosmiceventfilter = cms.EDFilter(
    "ffCosmicEventFilter",
    src=cms.InputTag("cosmicMuons"),
    minCosAlpha=cms.double(0.99),
    trackSelection=cms.string("pt>5 && abs(eta)<1.2"),
    maxPairCount=cms.int32(6),
    taggingMode=cms.bool(True),
)