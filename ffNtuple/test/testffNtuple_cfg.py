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
    process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
else:
    process.load('Configuration.StandardSequences.MagneticField_38T_cff')
    process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')



if cmsrel.startswith('CMSSW_10'):
    process.GlobalTag.globaltag = '102X_upgrade2018_realistic_v15'
if cmsrel.startswith('CMSSW_9'):
    process.GlobalTag.globaltag = '94X_mc2017_realistic_v17'
if cmsrel.startswith('CMSSW_8'):
    process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_TrancheIV_v8'

TEST_FAST = False
SIG_MC = False

from testDataSource import *
_event_runover = -1
_report_every = 100
_data_runover = datafiles
_output_fname = outputfilename

if TEST_FAST:
    _event_runover= 50
    _report_every = 1
    _data_runover = [datafiles[0]]
    if SIG_MC:
        _data_runover = [
            'root://cmseos.fnal.gov//store/group/lpcmetx/MCSIDM/AODSIM/2018/CRAB_PrivateMC/SIDM_BsTo2DpTo4Mu_MBs-150_MDp-5_ctau-250/181228_055735/0000/SIDM_AODSIM_1.root',
        ]
    else:
        _data_runover = [
            'root://xrootd.unl.edu//store/mc/RunIIAutumn18DRPremix/ZZTo4L_TuneCP5_13TeV_powheg_pythia8/AODSIM/102X_upgrade2018_realistic_v15_ext1-v2/60000/3248C8A6-00AC-E14A-B832-4E8B81E81BA2.root',
        ]
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
if SIG_MC:
    process.load('Firefighter.recoStuff.ffDsaPFCandMergeCluster_cff')
    process.load('Firefighter.ffNtuple.ffNtuples_cff')
else:
    process.load('Firefighter.recoStuff.ffDsaPFCandMergeCluster_d_cff')
    process.load('Firefighter.ffNtuple.ffNtuples_d_cff')

process.ntuple_step = cms.Path(process.ffLeptonJetSeq + process.ffNtuplesSeq)
process.endjob_step = cms.EndPath(process.endOfProcess)

process.schedule = cms.Schedule(
    process.ntuple_step,
    process.endjob_step
)
