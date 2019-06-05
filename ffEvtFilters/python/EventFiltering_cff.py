import FWCore.ParameterSet.Config as cms
from Firefighter.ffEvtFilters.LeptonJetPairFilter_cfi import *
from Firefighter.ffEvtFilters.ffTriggerObjectsMatchingFilter_cfi import *
from Firefighter.ffEvtFilters.ffEventSwitcher import switches

assert switches["region"] in ["all", "signal", "control"]


###############################################################################
ffBeginEventFilteringSeq = cms.Sequence(triggerObjectMatchingFilter)


###############################################################################
ffEndEventFilteringSeq = cms.Sequence()

if switches["region"] != "all":
    ffEventFilteringSeq = cms.Sequence(leptonjetpairfilter)
    if switches["region"] == "signal":
        leptonjetpairfilter.logic = cms.string("min")
    if switches["region"] == "control":
        leptonjetpairfilter.logic = cms.string("max")
