import FWCore.ParameterSet.Config as cms

leptonjetpairfilter = cms.EDFilter(
    "LeptonJetPairFilter",
    src=cms.InputTag("filteredLeptonJet"),
    logic=cms.string("min"),
    dphi=cms.double(2.5),
)
