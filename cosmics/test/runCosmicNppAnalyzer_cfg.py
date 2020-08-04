import FWCore.ParameterSet.Config as cms
_event_runover = -1
_report_every = 1000
_data_runover = [
    '/store/data/Run2018E/Cosmics/RAW-RECO/CosmicTP-PromptReco-v1/000/325/688/00000/DFD298A2-52D0-DE4C-ACE6-E657D3D237B4.root',
    '/store/data/Run2018E/Cosmics/RAW-RECO/CosmicTP-PromptReco-v1/000/325/698/00000/DF296CBC-F556-5C4A-AA72-77B5BACCCCAF.root',
    '/store/data/Run2018E/Cosmics/RAW-RECO/CosmicTP-PromptReco-v1/000/325/698/00000/8293825F-62F5-9143-873F-D3570875AA7F.root',
    '/store/data/Run2018E/Cosmics/RAW-RECO/CosmicTP-PromptReco-v1/000/325/698/00000/5A781B3E-E700-E24E-9341-B88ED6284D59.root',
    '/store/data/Run2018E/Cosmics/RAW-RECO/CosmicTP-PromptReco-v1/000/325/698/00000/361C7BC7-F696-A54A-A98A-09A01F9077DB.root',
    '/store/data/Run2018E/Cosmics/RAW-RECO/CosmicTP-PromptReco-v1/000/325/697/00000/CF440E5A-7A65-D543-BC90-7B8819786BD0.root',
]

_output_fname = 'cosmic_18E.root'
_globaltag = '102X_dataRun2_Prompt_v11'

process = cms.Process("FFCosmics")
process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.EventContent.EventContent_cff")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.EndOfProcess_cff")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
process.GlobalTag.globaltag = _globaltag

process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi")
process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi")
process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = cms.untracked.string("INFO")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(_report_every)

process.options = cms.untracked.PSet(
    wantSummary=cms.untracked.bool(True),
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

## filter
process.goodCosmics = cms.EDFilter("TrackSelector",
    src = cms.InputTag("cosmicMuons"),
    cut = cms.string("pt>5. & abs(eta)<1.2"),
)
process.moreThanOneCosmic = cms.EDFilter("TrackCountFilter",
    src = cms.InputTag('goodCosmics'),
    minNumber = cms.uint32(1)
)
process.goodOfflinePrimaryVertices = cms.EDFilter("VertexSelector",
    src = cms.InputTag("offlinePrimaryVertices"),
    cut = cms.string("!isFake && ndof > 4 && abs(z) <= 24 && position.Rho <= 2"),
    filter = cms.bool(False),
)
## producer
process.load("Firefighter.ffEvtFilters.ffCosmicEventFilter_cfi")
## analyzer
process.parallelCosmicPairs = cms.EDAnalyzer("CosmicParallelPairAnalyzer",
    src = cms.InputTag('goodCosmics'),
)

process.nppSequence = cms.Sequence(
    process.goodCosmics + process.moreThanOneCosmic
    + process.goodOfflinePrimaryVertices
    + process.ffcosmiceventfilter
    + process.parallelCosmicPairs
)


process.p = cms.Path(process.nppSequence)