import FWCore.ParameterSet.Config as cms

forkCandAgainstDsaMuon = cms.EDProducer(
    "ForkCandAgainstDsaMuon",
    src=cms.InputTag("particleFlow"),
    matched=cms.InputTag("dSAmuPFCand"),
    algorithm=cms.string("byPropagatingSrc"),
    srcTrack=cms.string("tracker"),
    srcState=cms.string("atVertex"),
    srcPreselection=cms.string("pt>2 && abs(eta)<2.4"),
    matchedTrack=cms.string(
        "muon"
    ),  # this is not super relevant in case of PFCandidate, only affect getTrack()
    matchedState=cms.string("innermost"),
    maxDeltaR=cms.double(
        0.3
    ),  # this makes almost sense with the current CRAFT-based alignment
    maxDeltaLocalPos=cms.double(
        100
    ),  # set it to 1m, so it's not really cutting anything
    maxDeltaPtRel=cms.double(10),
    sortBy=cms.string("deltaR"),
)
