import FWCore.ParameterSet.Config as cms

_data_runover = ['file:/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14_EGamma/src/Firefighter/ffConfig/crabGarage/191213a/result_D.root']
_output_fname = 'unpackL1.root'
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
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1)

process.options = cms.untracked.PSet(
    wantSummary=cms.untracked.bool(False),
    numberOfThreads=cms.untracked.uint32(1),
    numberOfStreams=cms.untracked.uint32(0),
)

process.maxEvents = cms.untracked.PSet(
    input=cms.untracked.int32(1)
)

process.source = cms.Source(
    "PoolSource", fileNames=cms.untracked.vstring(*_data_runover)
)

process.TFileService = cms.Service(
    "TFileService",
    fileName=cms.string(_output_fname),
    closeFileFast=cms.untracked.bool(True),
)



process.l1bits = cms.EDProducer(
    "ff::L1TriggerResultsConverter",
    src=cms.InputTag("gtStage2Digis"),
    legacyL1=cms.bool(False),
    storeUnprefireableBit=cms.bool(True),
    src_ext=cms.InputTag("gtStage2Digis")
    )

process.l1bitsana = cms.EDAnalyzer(
    "L1TriggerResultsAnalyzer",
    src=cms.InputTag("l1bits"),
)

process.p = cms.Path(process.l1bits+process.l1bitsana)

# process.Out = cms.OutputModule("PoolOutputModule",
#     fileName = cms.untracked.string('unpackL1_EDM.root'),
#     outputCommands=cms.untracked.vstring(
#         'drop *_*_*_*',
#         'keep *_*_*_FF*',
#     )
# )

# process.end = cms.EndPath(process.Out)