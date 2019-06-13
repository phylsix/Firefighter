import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
import os
import sys
from Firefighter.ffConfig.dataSample import samples

options = VarParsing.VarParsing("analysis")
options.register(
    "dataType",
    "signal-4mu",
    VarParsing.VarParsing.multiplicity.singleton,
    VarParsing.VarParsing.varType.string,
    "Type of sample to process. Default is 'signal-4mu'",
)
options.register(
    "fast",
    1,
    VarParsing.VarParsing.multiplicity.singleton,
    VarParsing.VarParsing.varType.int,
    "testing fast. aka. only 50 events. Default is True.",
)
options.register(
    "keepSkim",
    0,
    VarParsing.VarParsing.multiplicity.singleton,
    VarParsing.VarParsing.varType.int,
    "whether to keep intermediate skimmed file. Default is False.",
)

cmsrel = os.environ["CMSSW_VERSION"]
options.parseArguments()

process = cms.Process("FF")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.EventContent.EventContent_cff")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.EndOfProcess_cff")
if cmsrel.startswith("CMSSW_8"):
    process.load("Configuration.StandardSequences.MagneticField_cff")
    process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
else:
    process.load("Configuration.StandardSequences.MagneticField_38T_cff")
    process.load(
        "Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff"
    )

if cmsrel.startswith("CMSSW_10"):
    process.GlobalTag.globaltag = "102X_upgrade2018_realistic_v18"
    if "data_abc" in options.dataType.lower():
        process.GlobalTag.globaltag = "102X_dataRun2_Sep2018ABC_v2"
    if "data_d" in options.dataType.lower():
        process.GlobalTag.globaltag = "102X_dataRun2_Prompt_v13"
if cmsrel.startswith("CMSSW_9"):
    process.GlobalTag.globaltag = "94X_mc2017_realistic_v17"
if cmsrel.startswith("CMSSW_8"):
    process.GlobalTag.globaltag = "80X_mcRun2_asymptotic_2016_TrancheIV_v8"


_event_runover = -1
_report_every = 1000
try:
    _data_runover = (
        [samples[options.dataType]]
        if isinstance(samples[options.dataType], str)
        else samples[options.dataType]
    )
    _data_runover = map(
        lambda f: f if f.startswith("root://") else "file:" + f, _data_runover
    )
    _output_fname = "ffNtuple_{}.root".format(options.dataType)
except KeyError:
    sys.exit(
        "Sample '{}' not available! choose from {}".format(
            options.dataType, samples.keys()
        )
    )

if options.fast:
    _event_runover = 50
    _report_every = 1
    _data_runover = [_data_runover[0]]

process.MessageLogger.cerr.threshold = cms.untracked.string("INFO")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(_report_every)

process.options = cms.untracked.PSet(
    wantSummary=cms.untracked.bool(False),
    numberOfThreads=cms.untracked.uint32(8),
    numberOfStreams=cms.untracked.uint32(0),
)

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(_event_runover))

process.source = cms.Source(
    "PoolSource", fileNames=cms.untracked.vstring(*_data_runover)
)

process.TFileService = cms.Service(
    "TFileService",
    fileName=cms.string(_output_fname),
    closeFileFast=cms.untracked.bool(True),
)
if "signal" in options.dataType.lower():
    process.load("Firefighter.recoStuff.ffDsaPFCandMergeCluster_cff")
    process.load("Firefighter.ffNtuple.ffNtuples_cff")
else:
    process.load("Firefighter.recoStuff.ffDsaPFCandMergeCluster_d_cff")
    process.load("Firefighter.ffNtuple.ffNtuples_d_cff")
process.load("Firefighter.recoStuff.ffMetFilters_cff")
process.load("Firefighter.ffEvtFilters.EventFiltering_cff")

from Firefighter.recoStuff.skimOutput_cfi import skimOutput as _skimoutput

process.skimOutput = _skimoutput.clone(
    fileName=cms.untracked.string("skim" + _output_fname)
)

process.recofilterSeq = cms.Sequence(
    process.ffBeginEventFilteringSeq
    + process.ffLeptonJetSeq
    + process.ffMetFilterSeq
    + process.ffEndEventFilteringSeq
)

process.ntuple_step = cms.Path(process.recofilterSeq + process.ffNtuplesSeq)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.output_step = cms.EndPath(process.skimOutput)

if options.keepSkim:
    process.schedule = cms.Schedule(
        process.ntuple_step, process.endjob_step, process.output_step
    )
else:
    process.schedule = cms.Schedule(process.ntuple_step, process.endjob_step)
