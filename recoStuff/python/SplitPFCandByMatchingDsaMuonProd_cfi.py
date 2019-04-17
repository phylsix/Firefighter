import FWCore.ParameterSet.Config as cms

from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi import *


splitPFCandByMatchingDsaMuon = cms.EDProducer(
    "SplitPFCandByMatchingDsaMuonProd",
    src=cms.InputTag("particleFlowPtrs"),
    matched=cms.InputTag("dSAmuPFCand"),
    srcPreselection=cms.string("pt>2 && abs(eta)<2.4 && charge!=0"),
    matcherByTkParams=cms.PSet(
        maxDeltaR=cms.double(0.5),
        maxDeltaLocalPos=cms.double(10),  # cm
        maxDeltaPtRel=cms.double(10),
        sortBy=cms.string("deltaLocalPos"),  # deltaLocalPos, deltaPtRel, deltaR
    ),
    matcherByMuParams=cms.PSet(
        recoMuons=cms.InputTag("muons"), minHitsOverlapRatio=cms.double(0.75)
    ),
)
