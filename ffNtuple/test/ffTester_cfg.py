import FWCore.ParameterSet.Config as cms
import FWCore.PythonUtilities.LumiList as LumiList
import sys
import os

process = cms.Process("FFTEST")

process.load("FWCore.MessageService.MessageLogger_cfi")

dataType = sys.argv[2]
TEST_FAST = True
if len(sys.argv) > 3 and "full" in sys.argv:
    TEST_FAST = False

from Firefighter.ffConfig.dataSample import samples, skimmedSamples

if len(sys.argv) > 3 and "skim" in sys.argv:
    samples = skimmedSamples

_event_runover = -1
_report_every = 1000
try:
    _data_runover = (
        [samples[dataType]] if isinstance(samples[dataType], str) else samples[dataType]
    )
    _data_runover = map(
        lambda f: f if f.startswith("root://") else "file:" + f, _data_runover
    )
    _output_fname = "ffTest_{}.root".format(dataType)
except KeyError:
    sys.exit(
        "Sample '{}' not available! choose from {}".format(dataType, samples.keys())
    )

if TEST_FAST:
    _event_runover = 100
    _report_every = 10
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

process.fftest = cms.EDAnalyzer("ffTesterNonSkim")
if len(sys.argv) > 3 and "skim" in sys.argv:
    process.fftest = cms.EDAnalyzer("ffTesterForSkim")
process.p = cms.Path(process.fftest)

if "NoBPTX" in dataType:
    process.load("Configuration.StandardSequences.Services_cff")
    process.load("Configuration.EventContent.EventContent_cff")
    process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
    process.load("Configuration.StandardSequences.MagneticField_38T_cff")
    process.load(
        "Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff"
    )
    process.GlobalTag.globaltag = "102X_dataRun2_Sep2018Rereco_v1"
    jsonfilepath = os.path.join(
        os.environ["CMSSW_BASE"], "src/Firefighter/ffConfig/data/NoBPTX2018_json.txt"
    )
    process.source.lumisToProcess = LumiList.LumiList(
        filename=jsonfilepath
    ).getVLuminosityBlockRange()
    process.load("Firefighter.recoStuff.NoBPTXSkimMuonTime_cff")
    process.p = cms.Path(process.noBPTXSkimMuonTimeSeq + process.fftest)
