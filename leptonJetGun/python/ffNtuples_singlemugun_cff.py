import FWCore.ParameterSet.Config as cms

ntuple_singlemu = cms.PSet(
    NtupleName=cms.string("ffNtupleSingleMu"),
    recoMuonSrc=cms.InputTag("muons"),
    dSAMuonSrc=cms.InputTag("muonsFromdSA"),
)

ffNtuplizer = cms.EDAnalyzer(
    "ffNtupleManager", HltProcName=cms.string("HLT"), Ntuples=cms.VPSet(ntuple_singlemu)
)

ffNtuplesSeq = cms.Sequence(ffNtuplizer)
