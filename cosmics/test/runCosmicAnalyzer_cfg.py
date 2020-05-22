import FWCore.ParameterSet.Config as cms
_event_runover = -1
_report_every = 1000
_data_runover = ['file:/uscmst1b_scratch/lpc1/3DayLifetime/wsi/MUO-RunIISummer19CosmicDR-00001.root']

_output_fname = 'cosmic_MC.root'
_globaltag = '106X_upgrade2018cosmics_realistic_deco_v4'

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
    numberOfThreads=cms.untracked.uint32(1),
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


## analyzer
process.ana = cms.EDAnalyzer("CosmicEfficiencyAnalyzer")


process.p = cms.Path(process.ana)