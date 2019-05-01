import FWCore.ParameterSet.Config as cms

ntuple_muontiming = cms.PSet(
    NtupleName=cms.string("ffNtupleMuonTiming"),
    src=cms.InputTag("muons"),
    instances=cms.vstring("combined", "csc", "dt"),
)

ffNtuplizer = cms.EDAnalyzer(
    "ffNtupleManager",
    HltProcName=cms.string("HLT"),
    Ntuples=cms.VPSet(ntuple_muontiming),
)

ffNtuplesSeq = cms.Sequence(ffNtuplizer)
