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

TEST_FAST = True

from testDataSource import *
_event_runover = -1
_report_every = 100
_data_runover = datafiles
_output_fname = outputfilename

if TEST_FAST:
    _event_runover= 50
    _report_every = 1
    _data_runover = [datafiles[0]]
    _output_fname = 'testffNtuple.root'

process.MessageLogger.cerr.threshold = cms.untracked.string('INFO')
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(_report_every)

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False),
    numberOfThreads = cms.untracked.uint32(8),
    numberOfStreams = cms.untracked.uint32(0)
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(_event_runover)
)

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
        *_data_runover
    )
 )

process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string(_output_fname),
    closeFileFast = cms.untracked.bool(True)
)

process.load('Firefighter.ffNtuple.ffNtuples_cff')
process.ntuple_step = cms.Path(process.ffNtuplesSeq)
process.endjob_step = cms.EndPath(process.endOfProcess)

process.schedule = cms.Schedule(
    process.ntuple_step,
    process.endjob_step
)
