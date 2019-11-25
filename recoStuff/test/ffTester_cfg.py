import FWCore.ParameterSet.Config as cms
import FWCore.PythonUtilities.LumiList as LumiList
import sys
import os

_event_runover = -1
_report_every = 1
_data_runover = ['file:/uscms_data/d3/wsi/lpcdm/CMSSW_10_2_14_EGamma/src/Firefighter/ffLite/DataCR/picker/eventlist__4mu/skimmed_data18CR_D_AOD.root']
_output_fname = 'fftest.root'
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
    numberOfThreads=cms.untracked.uint32(1),
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




process.load("Firefighter.recoStuff.MuonChamberMatchProducer_cfi")

process.fftest = cms.EDAnalyzer("ffTesterNonSkim")
if len(sys.argv) > 3 and "skim" in sys.argv:
    process.fftest = cms.EDAnalyzer("ffTesterForSkim")
process.p = cms.Path(process.muonChamberMatchProducer + process.fftest)
# process.p = cms.Path(process.fftest)


_NoBPTX = False
if _NoBPTX:
    process.GlobalTag.globaltag = "102X_dataRun2_Sep2018Rereco_v1"
    jsonfilepath = os.path.join(os.environ["CMSSW_BASE"], "src/Firefighter/ffConfig/data/NoBPTX2018_json.txt")
    process.source.lumisToProcess = LumiList.LumiList(filename=jsonfilepath).getVLuminosityBlockRange()
    process.load("Firefighter.recoStuff.NoBPTXSkimMuonTime_cff")
    process.p = cms.Path(process.noBPTXSkimMuonTimeSeq + process.fftest)
