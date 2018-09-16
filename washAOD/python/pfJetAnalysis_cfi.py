import FWCore.ParameterSet.Config as cms

pfjetana = cms.EDAnalyzer("pfJetAnalysis",
                          src=cms.InputTag("ak4PFJets", "", "RECO"),
                          anydSAForJetType=cms.bool(True)
                          )