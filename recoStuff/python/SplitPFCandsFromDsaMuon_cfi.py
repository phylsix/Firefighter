import FWCore.ParameterSet.Config as cms

from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi import *

import Firefighter.recoStuff.ffCandMatchersParameter_cfi as ffCMP

splitPFCandsFromDsaMuon = cms.EDProducer(
    "SplitPFCandsFromDsaMuon",
    src=cms.InputTag("particleFlowPtrs"),
    matched=cms.InputTag("dSAmuPFCand"),
    srcPreselection=cms.string(
        " && ".join(["pt>2.", "abs(eta)<2.4", "particleId==3", "charge!=0"])
    ),
    MatcherByExtrapolateTracks=ffCMP.MatcherByExtrapolateTracks,
    MatcherByOverlapSegments=ffCMP.MatcherByOverlapSegments,
    # MatcherByOverlapHitPatterns=ffCMP.MatcherByOverlapHitPatterns,

)
