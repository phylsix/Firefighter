import FWCore.ParameterSet.Config as cms
from Firefighter.ffEvtFilters.LeptonJetPairFilter_cfi import *
from Firefighter.ffEvtFilters.ffEventSwitcher import switches

assert switches["region"] in ["all", "signal", "control"]

if switches["region"] == "all":
    ffEventFilteringSeq = cms.Sequence()
else:
    ffEventFilteringSeq = cms.Sequence(leptonjetpairfilter)
    if switches["region"] == "signal":
        leptonjetpairfilter.logic = cms.string("min")
    if switches["region"] == "control":
        leptonjetpairfilter.logic = cms.string("max")
