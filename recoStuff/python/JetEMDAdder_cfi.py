import FWCore.ParameterSet.Config as cms

jetemdadder = cms.EDProducer(
    "JetEMDAdder",
    src=cms.InputTag("ffLeptonJet"),
    matched=cms.InputTag("ffLeptonJetSubjets"),
    associationMap=cms.InputTag("ffLeptonJetSLeptonJetMap"),
)
