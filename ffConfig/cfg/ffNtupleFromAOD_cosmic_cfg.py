import FWCore.ParameterSet.Config as cms

process = cms.Process("FFC")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.EventContent.EventContent_cff")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.EndOfProcess_cff")

process.MessageLogger.cerr.threshold = cms.untracked.string("INFO")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)

process.options = cms.untracked.PSet(
    wantSummary=cms.untracked.bool(False),
    numberOfThreads=cms.untracked.uint32(2),
    numberOfStreams=cms.untracked.uint32(0),
)

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(-1))

process.source = cms.Source(
    "PoolSource", fileNames=cms.untracked.vstring("file:AODSIM.root")
)

process.TFileService = cms.Service(
    "TFileService",
    fileName=cms.string("ffNtupleCosmic.root"),
    closeFileFast=cms.untracked.bool(True),
)

process.load("Firefighter.recoStuff.CosmicFilter_cfi")
process.load("Firefighter.ffNtuple.ffNtuples_cosmic_cff")

process.ntuple_step = cms.Path(process.cosmicmuonfilter + process.ffNtuplesSeq)
process.endjob_step = cms.EndPath(process.endOfProcess)

process.schedule = cms.Schedule(process.ntuple_step, process.endjob_step)
