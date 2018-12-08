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

process.MessageLogger = cms.Service(
    "MessageLogger",
    destinations = cms.untracked.vstring('joblog', 'cerr'),
    debugModules = cms.untracked.vstring('ffNtuples'),
    joblog = cms.untracked.PSet(
        threshold = cms.untracked.string('ERROR')
    )
)

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False),
    numberOfThreads = cms.untracked.uint32(8),
    numberOfStreams = cms.untracked.uint32(0)
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
        'file:AODSIM.root',
    )
 )

process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string('testffNtuple.root'),
    closeFileFast = cms.untracked.bool(True)
)

process.load('Firefighter.ffNtuple.ffNtuples_cff')
process.ntuple_step = cms.Path(process.ffNtuplesSeq)
process.endjob_step = cms.EndPath(process.endOfProcess)

process.schedule = cms.Schedule(
    process.ntuple_step,
    process.endjob_step
)
