import FWCore.ParameterSet.Config as cms

leptonjetpairfilter = cms.EDFilter(
    "LeptonJetPairFilter",
    src=cms.InputTag("filteredLeptonJet"),
    minDPhi=cms.double(2.5),
)
