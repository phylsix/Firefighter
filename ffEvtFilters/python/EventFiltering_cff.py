import FWCore.ParameterSet.Config as cms
from Firefighter.ffEvtFilters.LeptonJetPairFilter_cfi import *
from Firefighter.ffEvtFilters.ffTriggerObjectsMatchingFilter_cfi import *
from Firefighter.ffEvtFilters.ffCosmicEventFilter_cfi import *
# from Firefighter.ffConfig.ffConfigSwitch import switches


###############################################################################
ffBeginEventFilteringSeq = cms.Sequence(
    ffcosmiceventfilter
    + triggerObjectMatchingFilter
)


###############################################################################

ffLeptonJetPairCountFilter = cms.EDFilter(
    "CandViewCountFilter",
    src=cms.InputTag("filteredLeptonJet"),
    minNumber=cms.uint32(2),
)

# if switches["region"] != "all":
#     if switches["region"] == "single":
#         ffEndEventFilteringSeq = cms.Sequence(~ffLeptonJetPairCountFilter)
#     else:
#         if switches["region"] == "signal":
#             ffEndEventFilteringSeq = cms.Sequence(
#                 ffLeptonJetPairCountFilter + leptonjetpairfilter
#             )
#         if switches["region"] == "control":
#             ffEndEventFilteringSeq = cms.Sequence(
#                 ffLeptonJetPairCountFilter + ~leptonjetpairfilter
#             )

ffEndEventFilteringSeq = cms.Sequence()



ffEndEventFilteringSeq_single = cms.Sequence(~ffLeptonJetPairCountFilter)
ffEndEventFilteringSeq_signal = cms.Sequence(ffLeptonJetPairCountFilter + leptonjetpairfilter)
ffEndEventFilteringSeq_control = cms.Sequence(ffLeptonJetPairCountFilter + ~leptonjetpairfilter)
