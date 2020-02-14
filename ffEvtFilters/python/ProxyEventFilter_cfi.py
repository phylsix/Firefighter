import FWCore.ParameterSet.Config as cms

proxyeventfilter = cms.EDFilter(
    "ProxyEventFilter",
    src=cms.InputTag("filteredLeptonJet"),
    ljsrc=cms.InputTag("leptonjetSources"),
)
