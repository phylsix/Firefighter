import FWCore.ParameterSet.Config as cms
from Firefighter.ffEvtFilters.LeptonJetMuonTypeFilter_cfi import *
from Firefighter.ffEvtFilters.ProxyEventFilter_cfi import *
from Firefighter.ffEvtFilters.ffTriggerObjectsMatchingFilter_cfi import *
from Firefighter.ffEvtFilters.ffCosmicEventFilter_cfi import *
from Firefighter.recoStuff.ffLeptonJetFiltering_cff import ffLeptonJetCountFilter as _ljcntfilter

###############################################################################
# begining event filters

ffBeginEventFilteringSeq = cms.Sequence(
    ffcosmiceventfilter
    + triggerObjectMatchingFilter
)

###############################################################################
# leptonjet counting filter

ffLeptonJetSingleCountFilter = _ljcntfilter.clone()
ffLeptonJetPairCountFilter = _ljcntfilter.clone(minNumber=cms.uint32(2),)

###############################################################################
# ending event filters

ffEndEventFilteringSeq = cms.Sequence()

ffEndEventFilteringSeq_LJge1 = cms.Sequence(ffLeptonJetSingleCountFilter)
ffEndEventFilteringSeq_LJge2 = cms.Sequence(ffLeptonJetPairCountFilter)

## old definition- deprecated.
# ffEndEventFilteringSeq_signal = cms.Sequence(ffLeptonJetPairCountFilter + leptonjetpairfilter)
# ffEndEventFilteringSeq_control = cms.Sequence(ffLeptonJetPairCountFilter + ~leptonjetpairfilter)

ffEndEventFilteringSeq_muontype = cms.Sequence(leptonjetmuontypefilter)
ffEndEventFilteringSeq_proxy = cms.Sequence(proxyeventfilter)