import FWCore.ParameterSet.Config as cms

leptonjetmuontypefilter = cms.EDFilter(
    "LeptonJetMuonTypeFilter",
    src=cms.InputTag("filteredLeptonJet"),
    minCount=cms.uint32(1),
)
