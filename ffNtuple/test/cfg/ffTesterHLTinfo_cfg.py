import FWCore.ParameterSet.Config as cms

_data_runover = ['file:/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14_EGamma/src/Firefighter/ffConfig/crabGarage/191213a/result_D.root']
_output_fname = 'HLTinfo.root'
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
    input=cms.untracked.int32(5)
)

process.source = cms.Source(
    "PoolSource", fileNames=cms.untracked.vstring(*_data_runover)
)

process.TFileService = cms.Service(
    "TFileService",
    fileName=cms.string(_output_fname),
    closeFileFast=cms.untracked.bool(True),
)



process.HLTinfo = cms.EDAnalyzer(
    "ffTesterHLTinfo",
    triggerResults=cms.InputTag("TriggerResults", "", "HLT"),
    triggerEvent=cms.InputTag("hltTriggerSummaryAOD", "", "HLT"),
    triggerPaths=cms.vstring(
        # L2DoubleMu triggers
        "HLT_DoubleL2Mu23NoVtx_2Cha",
        "HLT_DoubleL2Mu23NoVtx_2Cha_NoL2Matched",
        "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed",
        "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed_NoL2Matched",
        "HLT_DoubleL2Mu25NoVtx_2Cha",
        "HLT_DoubleL2Mu25NoVtx_2Cha_NoL2Matched",
        "HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed",
        "HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed_NoL2Matched",
        "HLT_DoubleL2Mu25NoVtx_2Cha_Eta2p4",
        "HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed_Eta2p4",

        # mu photon trigger
        "HLT_Mu38NoFiltersNoVtxDisplaced_Photon38_CaloIdL",

        # single mu trigger
        "HLT_IsoMu24",
        "HLT_Mu50",

        # single electron trigger
        "HLT_Ele28_WPTight_Gsf",

        # tri- muon trigger
        "HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx",
    ),
    triggerObjFilterLabels=cms.vstring(
        # L2 DoubleMu triggers
        "hltL2DoubleMu23NoVertexL2Filtered2Cha",
        "hltL2fL1sMuORL1f0DoubleL2NoVtx23Q2ChaNoL2Matched",
        "hltL2fL1sMuORL1f0DoubleL2NoVtx23Q2ChaCosmicSeed",
        "hltL2fL1sMuORL1f0DoubleL2NoVtx23Q2ChaCosmicSeedNoMatched",
        "hltL2DoubleMu25NoVtxFiltered2Cha",
        "hltL2fL1sMuORL1f0DoubleL2NoVtx25Q2ChaNoL2Matched",
        "hltL2fL1sMuORL1f0DoubleL2NoVtx25Q2ChaCosmicSeed",
        "hltL2fL1sMuORL1f0DoubleL2NoVtx25Q2ChaCosmicSeedNoMatched",
        "hltL2DoubleMu25NoVtxFiltered2ChaEta2p4",
        "hltL2fL1sMuORL1f0DoubleL2NoVtx25Q2ChaCosmicSeedEta2p4",

        "hltL1sDoubleMu125to157ORTripleMu444",
        "hltL1sDoubleMu125to157ORTripleMu444ORSingleMu22",

        # mu photon trigger
        "hltMu38NoFiltersNoVtxPhoton38CaloIdLHEFilter",
        "hltL3fL1sMu5EG20orMu20EG15L1f5L2NVf16L3NoFiltersNoVtxFiltered38Displaced",
        "hltL1sMu5EG23IorMu7EG23IorMu20EG17IorMu23EG10",
        "hltEGL1Mu5EG20Filter",

        # single mu trigger
        "hltL3crIsoL1sSingleMu22L1f0L2f10QL3f24QL3trkIsoFiltered0p07",
        "hltL1sSingleMu22",
        "hltL3fL1sMu22Or25L1f0L2f10QL3Filtered50Q",
        "hltL1sSingleMu22or25",

        # single electron trigger
        "hltEle28WPTightGsfTrackIsoFilter",
        "hltEGL1SingleEGOrFilter",

        # tri- muon trigger
        "hltL3fL1sDoubleMu155ORTripleMu444L1f0L2f10OneMuL3Filtered16NoVtx",
        "hltL1sDoubleMu125to157ORTripleMu444",
    ),
    debug=cms.bool(True),
)


process.Analyzer = cms.Path( process.HLTinfo )

# process.Out = cms.OutputModule("PoolOutputModule",
#     fileName = cms.untracked.string('unpackL1_EDM.root'),
#     outputCommands=cms.untracked.vstring(
#         'drop *_*_*_*',
#         'keep *_*_*_FF*',
#     )
# )

# process.end = cms.EndPath(process.Out)

process.schedule = cms.Schedule(process.Analyzer)