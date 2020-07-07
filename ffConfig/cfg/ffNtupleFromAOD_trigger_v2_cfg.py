import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

import os
import yaml
from os.path import join
from Firefighter.ffConfig.ffChainConstruction import customizeNtupleTrigger

options = VarParsing.VarParsing("analysis")
configDefault = join(
    os.getenv("CMSSW_BASE"), "src/Firefighter/ffConfig/cfg/ffSuperConfig.yml"
)
options.register(
    "config",
    configDefault,
    VarParsing.VarParsing.multiplicity.singleton,
    VarParsing.VarParsing.varType.string,
    "Path to ffSuperConfig.yml",
)
# options.register(
#     "keepskim",
#     0,
#     VarParsing.VarParsing.multiplicity.singleton,
#     VarParsing.VarParsing.varType.int,
#     "Whether to keep skim output.",
# )
options.parseArguments()
ffConfig = yaml.safe_load(open(options.config))


process = cms.Process("FFNTP")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.EventContent.EventContent_cff")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.EndOfProcess_cff")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
process.GlobalTag.globaltag = ffConfig["condition-spec"]["globalTag"]

process.MessageLogger.cerr.threshold = cms.untracked.string("INFO")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(
    ffConfig["job-spec"]["reportEvery"]
)

process.options = cms.untracked.PSet(
    wantSummary=cms.untracked.bool(False),
    numberOfThreads=cms.untracked.uint32(ffConfig["job-spec"]["numThreads"]),
    numberOfStreams=cms.untracked.uint32(0),
)

process.maxEvents = cms.untracked.PSet(
    input=cms.untracked.int32(ffConfig["data-spec"]["maxEvents"])
)

process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(*ffConfig["data-spec"]["inputFileList"]),
)
if ffConfig["data-spec"].get("lumiMask", None):
    import FWCore.PythonUtilities.LumiList as LumiList

    process.source.lumisToProcess = LumiList.LumiList(
        url=ffConfig["data-spec"]["lumiMask"]
    ).getVLuminosityBlockRange()

process.TFileService = cms.Service(
    "TFileService",
    fileName=cms.string(ffConfig["data-spec"]["outputFileName"]),
    closeFileFast=cms.untracked.bool(True),
)


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
)

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
)


process = customizeNtupleTrigger( process, ffConfig,
                                triggerPaths=triggerPaths,
                                triggerObjFilterLabels=triggerObjFilterLabels
                                )

# print process.dumpPython().replace('\n\n','')