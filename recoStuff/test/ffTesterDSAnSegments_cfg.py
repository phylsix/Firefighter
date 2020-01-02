import FWCore.ParameterSet.Config as cms
_event_runover = -1
_report_every = 1
_data_runover = ['file:/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14_EGamma/src/Firefighter/ffConfig/crabGarage/191213a/result_D.root']
_output_fname = 'ffDSAnSegments.root'
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

process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi")
## analyzer
process.fftest = cms.EDAnalyzer("ffTesterDSAnSegments",
                    verbose=cms.bool(True)
                    )

process.p = cms.Path(process.fftest)
