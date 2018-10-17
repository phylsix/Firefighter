import FWCore.ParameterSet.Config as cms

trigSelfEffiForMuon = cms.EDAnalyzer('trigSelfEffiForMuon',
    muon = cms.InputTag("muons"),
    genParticle = cms.InputTag("genParticles"),
    trigResult = cms.InputTag("TriggerResults","","HLT"),
    trigEvent = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
    trigPath = cms.vstring('HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx'),
    processName = cms.string('HLT'),
    nMuons = cms.int32(3)
  )
