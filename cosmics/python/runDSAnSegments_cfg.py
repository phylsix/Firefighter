import FWCore.ParameterSet.Config as cms
_event_runover = -1
_report_every = 1000
_data_runover = ['/store/data/Run2018E/Cosmics/RAW-RECO/CosmicTP-PromptReco-v1/000/325/695/00000/19C3CFD7-4E31-CC42-871C-246493D956F7.root']

_output_fname = 'DSAnSegments.root'
_globaltag = '102X_dataRun2_Prompt_v11'

process = cms.Process("FFCosmics")
process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.EventContent.EventContent_cff")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.EndOfProcess_cff")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
process.GlobalTag.globaltag = _globaltag

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = cms.untracked.string("INFO")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(_report_every)

process.options = cms.untracked.PSet(
    wantSummary=cms.untracked.bool(False),
    numberOfThreads=cms.untracked.uint32(8),
    numberOfStreams=cms.untracked.uint32(0),
)

process.maxEvents = cms.untracked.PSet(
    input=cms.untracked.int32(_event_runover)
)

process.source = cms.Source(
    "PoolSource", fileNames=cms.untracked.vstring(*_data_runover)
)

process.TFileService = cms.Service(
    "TFileService",
    fileName=cms.string(_output_fname),
    closeFileFast=cms.untracked.bool(True),
)

## reco DSA Seq
process.load("RecoMuon.Configuration.RecoMuonPPonly_cff")
process.dsaSeq = cms.Sequence(process.displacedMuonSeeds*process.displacedStandAloneMuons)

## analyzer
process.ana = cms.EDAnalyzer("DSAnSegments")


process.p = cms.Path(process.dsaSeq*process.ana)