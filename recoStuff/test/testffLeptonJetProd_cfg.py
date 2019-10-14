import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.EventContent.EventContent_cff")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load("Configuration.StandardSequences.EndOfProcess_cff")
process.load(
    "Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff"
)


process.GlobalTag.globaltag = "94X_mc2017_realistic_v15"
import os

if os.environ["CMSSW_VERSION"].startswith("CMSSW_10"):
    process.GlobalTag.globaltag = "102X_upgrade2018_realistic_v15"

process.MessageLogger.cerr.threshold = cms.untracked.string("INFO")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(10)

process.options = cms.untracked.PSet(
    wantSummary=cms.untracked.bool(False),
    numberOfThreads=cms.untracked.uint32(1),
    numberOfStreams=cms.untracked.uint32(0),
)

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(50))

process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(
        "root://cmseos.fnal.gov//store/user/lpcmetx/MCSIDM/AODSIM/2018/CRAB_PrivateMC/SIDM_BsTo2DpTo4Mu_MBs-150_MDp-5_ctau-250/181228_055735/0000/SIDM_AODSIM_1.root"
    ),
)


process.skimOutput = cms.OutputModule(
    "PoolOutputModule",
    fileName=cms.untracked.string("skimOutputLeptonJetProd.root"),
    outputCommands=cms.untracked.vstring(
        "drop *_*_*_*",
        "keep *_*_*_USER",
        "keep recoGenParticles_genParticles*_*_*",
        "keep recoPFCandidates_particleFlow_*_RECO",
        "keep recoMuons_muons__RECO",
        "keep recoTracks_generalTracks_*_RECO",
        "keep recoTracks_*tandAloneMuons_*_RECO",
        "keep recoTrackExtras_*tandAloneMuons_*_RECO",
    ),
)

process.genfilter = cms.EDFilter(
    "GenParticleSelector",
    src=cms.InputTag("genParticles"),
    cut=cms.string(
        " && ".join(
            [
                "(abs(pdgId)==11 || abs(pdgId)==13)",
                "abs(eta)<2.4",
                "(vertex.rho<740. && abs(vertex.Z)<960.)",
                "pt>5.",
                "isHardProcess()",
            ]
        )
    ),
)
process.gencount = cms.EDFilter(
    "CandViewCountFilter", src=cms.InputTag("genfilter"), minNumber=cms.uint32(4)
)
process.gen_step = cms.Path(process.genfilter + process.gencount)

process.load("Firefighter.recoStuff.ffDsaPFCandMergeCluster_cff")

## *hadFree*
process.filteredPFCandsFwdPtr.src = cms.InputTag("particleFlowPtrs")
process.filteredPFCandsFwdPtr.cut = cms.string(
    " && ".join(
        ["abs(eta)<2.5", "particleId!=1", "particleId!=5", "particleId!=6"]
    )
)
process.filteringPFCands = cms.Sequence(process.filteringPFCands_hadFree)
process.ffLeptonJetProd = cms.Sequence(process.ffLeptonJetProd_hadFree)
process.ffLeptonJetFwdPtrs.src = cms.InputTag("ffLeptonJetHadFree")

process.leptonjet_step = cms.Path(process.ffLeptonJetSeq)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.output_step = cms.EndPath(process.skimOutput)


process.schedule = cms.Schedule(
    process.gen_step, process.leptonjet_step, process.endjob_step, process.output_step
)
