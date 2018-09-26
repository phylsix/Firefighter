import FWCore.ParameterSet.Config as cms

jetmasssculpt = cms.EDAnalyzer("jetMassSculpting",
                               src=cms.InputTag("ak4PFJets", "", "RECO")
                               )