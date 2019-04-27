import FWCore.ParameterSet.Config as cms

cosmicmuonfilter = cms.EDFilter("CosmicMuonFilter", muons=cms.InputTag("muons"))
