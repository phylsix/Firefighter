import FWCore.ParameterSet.Config as cms
from Firefighter.ffEvtFilters.LeptonJetPairFilter_cfi import *
from Firefighter.ffEvtFilters.ffTriggerObjectsMatchingFilter_cfi import *
from Firefighter.ffEvtFilters.ffCosmicEventFilter_cfi import *
from Firefighter.recoStuff.ffLeptonJetFiltering_cff import ffLeptonJetCountFilter as _ljcntfilter

###############################################################################

ffBeginEventFilteringSeq = cms.Sequence(
    ffcosmiceventfilter
    + triggerObjectMatchingFilter
)

###############################################################################

ffLeptonJetSingleCountFilter = _ljcntfilter.clone()
ffLeptonJetPairCountFilter = _ljcntfilter.clone(minNumber=cms.uint32(2),)

###############################################################################

ffEndEventFilteringSeq = cms.Sequence()

ffEndEventFilteringSeq_single = cms.Sequence(~ffLeptonJetPairCountFilter)
ffEndEventFilteringSeq_signal = cms.Sequence(ffLeptonJetPairCountFilter + leptonjetpairfilter)
ffEndEventFilteringSeq_control = cms.Sequence(ffLeptonJetPairCountFilter + ~leptonjetpairfilter)
