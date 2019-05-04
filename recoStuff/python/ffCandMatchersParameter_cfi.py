import FWCore.ParameterSet.Config as cms

MatcherByExtrapolateTracks = cms.PSet(
    maxDeltaR=cms.double(0.5),
    maxDeltaLocalPos=cms.double(10),  # cm
    maxDeltaPtRel=cms.double(10),
    requireSameCharge=cms.bool(False),
    sortBy=cms.string("deltaLocalPos"),  # deltaLocalPos, deltaPtRel, deltaR
)

MatcherByOverlapHitPatterns = cms.PSet(
    minOverlapRatio=cms.double(0.75)
)

MatcherByOverlapSegments = cms.PSet(
    maxDeltaR=cms.double(0.1),
    minOverlapRatio=cms.double(0.8),
    maxDeltaT=cms.double(20.), #ns
)