import FWCore.ParameterSet.Config as cms
import os
cmsrel = os.environ['CMSSW_VERSION']

process = cms.Process("USER")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.Services_cff')
process.load("Configuration.EventContent.EventContent_cff")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.EndOfProcess_cff')
if cmsrel.startswith('CMSSW_8'):
    process.load('Configuration.StandardSequences.MagneticField_cff')
    process.load(
        'Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
else:
    process.load('Configuration.StandardSequences.MagneticField_38T_cff')
    process.load(
        'Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')


if cmsrel.startswith('CMSSW_10'):
    process.GlobalTag.globaltag = '102X_upgrade2018_realistic_v15'
if cmsrel.startswith('CMSSW_9'):
    process.GlobalTag.globaltag = '94X_mc2017_realistic_v17'
if cmsrel.startswith('CMSSW_8'):
    process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_TrancheIV_v8'


process.MessageLogger.cerr.threshold = cms.untracked.string('INFO')
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)

process.options = cms.untracked.PSet(
    wantSummary=cms.untracked.bool(False),
    numberOfThreads=cms.untracked.uint32(2),
    numberOfStreams=cms.untracked.uint32(0)
)

process.maxEvents = cms.untracked.PSet(
    input=cms.untracked.int32(-1)
)

process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(
        'file:AODSIM.root'
    )
)

process.TFileService = cms.Service(
    "TFileService",
    fileName=cms.string('ffNtuple.root'),
    closeFileFast=cms.untracked.bool(True)
)

process.load('Firefighter.recoStuff.ffDsaPFCandMergeCluster_cff')
process.load('Firefighter.ffNtuple.ffNtuples_cff')

process.ntuple_pfjet.src = cms.InputTag('ffLeptonJet')

process.ntuple_step = cms.Path(process.ffLeptonJetSeq + process.ffNtuplesSeq)
process.endjob_step = cms.EndPath(process.endOfProcess)

process.schedule = cms.Schedule(
    process.ntuple_step,
    process.endjob_step
)
