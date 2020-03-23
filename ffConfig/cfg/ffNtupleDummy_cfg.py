import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

import os
import yaml
from os.path import join

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
options.register(
    "keepskim",
    0,
    VarParsing.VarParsing.multiplicity.singleton,
    VarParsing.VarParsing.varType.int,
    "Whether to keep skim output.",
)
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
process.load('Configuration.StandardSequences.Generator_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('GeneratorInterface.Core.genFilterSummary_cff')
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

process.dummy2 = cms.EDAnalyzer("GenXSecAnalyzer")
# Path and EndPath definitions
process.ana = cms.Path(process.dummy2)
# Schedule definition
process.schedule = cms.Schedule(process.ana)