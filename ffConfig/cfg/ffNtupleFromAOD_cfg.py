import FWCore.ParameterSet.Config as cms

process = cms.Process("USER")

IS_DATA = False

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.Services_cff')
process.load("Configuration.EventContent.EventContent_cff")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')


process.GlobalTag.globaltag = '94X_mc2017_realistic_v15'
import os
if os.environ['CMSSW_VERSION'].startswith('CMSSW_10'):
    process.GlobalTag.globaltag = '102X_upgrade2018_realistic_v15'

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
    fileName = cms.string('ffNtuple.root'),
    closeFileFast = cms.untracked.bool(True)
)

if IS_DATA:
    process.load('Firefighter.recoStuff.ffDsaPFCandMergeCluster_d_cff')
    process.load('Firefighter.ffNtuple.ffNtuples_d_cff')
else:
    process.load('Firefighter.recoStuff.ffDsaPFCandMergeCluster_cff')
    process.load('Firefighter.ffNtuple.ffNtuples_cff')
process.ntuple_pfjet.src = cms.InputTag('ffLeptonJet')

process.ntuple_step = cms.Path(process.ffLeptonJetSeq + process.ffNtuplesSeq)
process.endjob_step = cms.EndPath(process.endOfProcess)

process.schedule = cms.Schedule(
    process.ntuple_step,
    process.endjob_step
)
