import FWCore.ParameterSet.Config as cms
_event_runover = -1
_report_every = 1
_data_runover = ['file:/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14_EGamma/src/Firefighter/ffConfig/crabGarage/191213a/result_D.root']
_output_fname = 'ffReRecoCosmicMuon1leg.root'
_globaltag = '102X_dataRun2_Prompt_v15'

process = cms.Process("FFTEST")
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

## seed generateor
from RecoMuon.MuonSeedGenerator.CosmicMuonSeedProducer_cfi import *
CosmicMuonSeed.DTRecSegmentLabel = 'dt4DCosmicSegments'
process.CosmicMuonSeed = CosmicMuonSeed

## STA track producer
process.load("RecoMuon.CosmicMuonProducer.cosmicMuons_cff")
process.cosmicMuons1Leg = process.cosmicMuons.clone()
process.cosmicMuons1Leg.TrajectoryBuilderParameters.BuildTraversingMuon = True
process.cosmicMuons1Leg.TrajectoryBuilderParameters.Strict1Leg = True
process.cosmicMuons1Leg.TrajectoryBuilderParameters.DTRecSegmentLabel = 'dt4DCosmicSegments'
process.cosmicMuons1Leg.MuonSeedCollectionLabel = 'CosmicMuonSeed'

process.muoncosmicreco1legSTA = cms.Sequence(process.CosmicMuonSeed*process.cosmicMuons1Leg)


## analyzer
process.fftest = cms.EDAnalyzer("ffTesterMuonsFromCosmics1Leg")

process.p = cms.Path(process.muoncosmicreco1legSTA
                    *process.fftest)

process.Out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('reRecoCosmicMuon1Leg.root'),
    outputCommands=cms.untracked.vstring(
        'drop *_*_*_*',
        'keep *_*_*_FF*',
    )
)

process.end = cms.EndPath(process.Out)