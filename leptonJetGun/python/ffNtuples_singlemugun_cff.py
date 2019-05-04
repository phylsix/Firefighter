import FWCore.ParameterSet.Config as cms

ntuple_singlemu = cms.PSet(
    NtupleName=cms.string("ffNtupleSingleMu"),
    recoMuonSrc=cms.InputTag("muons"),
    dSAMuonSrc=cms.InputTag("muonsFromdSA"),
    deltaR=cms.double(0.1),
    overlapRatio=cms.double(0.8),
)

ffNtuplizer = cms.EDAnalyzer(
    "ffNtupleManager", HltProcName=cms.string("HLT"), Ntuples=cms.VPSet(ntuple_singlemu)
)

ffNtuplesSeq = cms.Sequence(ffNtuplizer)
