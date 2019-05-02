import FWCore.ParameterSet.Config as cms
import sys

process = cms.Process("FFGUN")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.EventContent.EventContent_cff")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.EndOfProcess_cff")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load(
    "Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff"
)
process.GlobalTag.globaltag = "102X_upgrade2018_realistic_v15"

dataType = "muplus1000"  # sys.argv[2]
TEST_FAST = True
if "full" in sys.argv:
    TEST_FAST = False

from Firefighter.ffConfig.dataSample import samples

_event_runover = -1
_report_every = 1000
try:
    _data_runover = (
        [samples[dataType]] if isinstance(samples[dataType], str) else samples[dataType]
    )
    _data_runover = map(
        lambda f: f if f.startswith("root://") else "file:" + f, _data_runover
    )
    _output_fname = "ffNtuple_{}.root".format(dataType)
except KeyError:
    sys.exit(
        "Sample '{}' not available! choose from {}".format(dataType, samples.keys())
    )

if TEST_FAST:
    _event_runover = 100
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

process.load("Firefighter.leptonJetGun.DsaToRecoMuon_cff")
process.load("Firefighter.leptonJetGun.ffNtuples_singlemugun_cff")

process.ntuple_step = cms.Path(process.dSAToRecoMuonSeq + process.ffNtuplesSeq)
process.endjob_step = cms.EndPath(process.endOfProcess)

process.schedule = cms.Schedule(process.ntuple_step, process.endjob_step)
