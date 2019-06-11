import FWCore.ParameterSet.Config as cms
import os
from Firefighter.ffConfig.ffConfigSwitch import switches

cmsrel = os.environ["CMSSW_VERSION"]
jobtype = switches["jobtype"]

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
    if jobtype.endswith("mc"):
        process.GlobalTag.globaltag = "102X_upgrade2018_realistic_v18"
    else:
        if jobtype == "data_abc":
            process.GlobalTag.globaltag = "102X_dataRun2_Sep2018ABC_v2"
        if jobtype == "data_d":
            process.GlobalTag.globaltag = "102X_dataRun2_Prompt_v13"
if cmsrel.startswith("CMSSW_9"):
    process.GlobalTag.globaltag = "94X_mc2017_realistic_v17"
if cmsrel.startswith("CMSSW_8"):
    process.GlobalTag.globaltag = "80X_mcRun2_asymptotic_2016_TrancheIV_v8"


process.MessageLogger.cerr.threshold = cms.untracked.string("INFO")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)

process.options = cms.untracked.PSet(
    wantSummary=cms.untracked.bool(False),
    numberOfThreads=cms.untracked.uint32(2),
    numberOfStreams=cms.untracked.uint32(0),
)

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(-1))

process.source = cms.Source(
    "PoolSource", fileNames=cms.untracked.vstring("file:AODSIM.root")
)

process.TFileService = cms.Service(
    "TFileService",
    fileName=cms.string("ffNtuple.root"),
    closeFileFast=cms.untracked.bool(True),
)

if jobtype == "sigmc":
    process.load("Firefighter.recoStuff.ffDsaPFCandMergeCluster_cff")
    process.load("Firefighter.ffNtuple.ffNtuples_cff")
else:
    process.load("Firefighter.recoStuff.ffDsaPFCandMergeCluster_d_cff")
    process.load("Firefighter.ffNtuple.ffNtuples_d_cff")
process.load("Firefighter.recoStuff.ffMetFilters_cff")
process.load("Firefighter.ffEvtFilters.EventFiltering_cff")

process.recofilterSeq = cms.Sequence(
    process.ffBeginEventFilteringSeq
    + process.ffLeptonJetSeq
    + process.ffMetFilterSeq
    + process.ffEndEventFilteringSeq
)

process.ntuple_step = cms.Path(process.recofilterSeq + process.ffNtuplesSeq)
process.endjob_step = cms.EndPath(process.endOfProcess)

process.schedule = cms.Schedule(process.ntuple_step, process.endjob_step)
