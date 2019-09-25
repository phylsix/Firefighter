import FWCore.ParameterSet.Config as cms

## soure generation
## 1. PFMuon
leptonjetSourcePFMuon = cms.EDProducer("LeptonjetSourcePFMuonProducer")
## 2. DSAMuon
leptonjetSourceDSAMuon = cms.EDProducer("LeptonjetSourceDSAMuonProducer")


## clustering sequence
leptonjetClusteringSeq = cms.Sequence(
    leptonjetSourcePFMuon
    + leptonjetSourceDSAMuon
)