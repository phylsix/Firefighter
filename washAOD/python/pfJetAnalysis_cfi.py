import FWCore.ParameterSet.Config as cms

pfjetana = cms.EDAnalyzer("pfJetAnalysis",
                          src=cms.InputTag("ak4PFJets", "", "RECO"),
                          anydSAForJetType=cms.bool(True),
                          dsa=cms.InputTag("selectedDsaMuons"),
                          kvfParam=cms.PSet(
                              maxDistance=cms.double(0.01),
                              maxNbrOfIterations=cms.int32(10),
                              doSmoothing=cms.bool(True),
                          ),
                          trigResult=cms.InputTag("TriggerResults", "", "HLT"),
                          trigEvent=cms.InputTag(
                              "hltTriggerSummaryAOD", "", "HLT"),
                          trigPath=cms.string(
                              'HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx'),
                          processName=cms.string('HLT')
                          )
