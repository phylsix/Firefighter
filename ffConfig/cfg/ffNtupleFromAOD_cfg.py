import FWCore.ParameterSet.Config as cms

process = cms.Process("USER")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.Services_cff')
process.load("Configuration.EventContent.EventContent_cff")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')


process.GlobalTag.globaltag = '94X_mc2017_realistic_v15'

process.MessageLogger.cerr.threshold = cms.untracked.string('INFO')
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False),
    numberOfThreads = cms.untracked.uint32(4),
    numberOfStreams = cms.untracked.uint32(0)
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
        'file:AODSIM.root'
    )
 )

process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string('testffNtuple.root'),
    closeFileFast = cms.untracked.bool(True)
)

process.load('Firefighter.recoStuff.ffDsaPFCandMergeCluster_cff')
process.load('Firefighter.ffNtuple.ffNtuples_cff')

process.leptonjet_step = cms.Path(process.ffLeptonJetSeq)
process.ntuple_step = cms.Path(process.ffNtuplesSeq)
process.endjob_step = cms.EndPath(process.endOfProcess)

process.schedule = cms.Schedule(
    process.leptonjet_step,
    process.ntuple_step,
    process.endjob_step
)
