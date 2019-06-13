import FWCore.ParameterSet.Config as cms
from Firefighter.ffEvtFilters.LeptonJetPairFilter_cfi import *
from Firefighter.ffEvtFilters.ffTriggerObjectsMatchingFilter_cfi import *
from Firefighter.ffConfig.ffConfigSwitch import switches


###############################################################################
ffBeginEventFilteringSeq = cms.Sequence(triggerObjectMatchingFilter)


###############################################################################
ffEndEventFilteringSeq = cms.Sequence()

ffLeptonJetPairCountFilter = cms.EDFilter(
    "CandViewCountFilter",
    src=cms.InputTag("filteredLeptonJet"),
    minNumber=cms.uint32(2),
)

if switches["region"] != "all":
    if switches["region"] == "single":
        ffEndEventFilteringSeq = cms.Sequence(~ffLeptonJetPairCountFilter)
    else:
        if switches["region"] == "signal":
            ffEndEventFilteringSeq = cms.Sequence(
                ffLeptonJetPairCountFilter + leptonjetpairfilter
            )
        if switches["region"] == "control":
            ffEndEventFilteringSeq = cms.Sequence(
                ffLeptonJetPairCountFilter + ~leptonjetpairfilter
            )
