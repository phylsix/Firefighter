import FWCore.ParameterSet.Config as cms

## soure generation
## 1. PFMuon
leptonjetSourcePFMuon = cms.EDProducer("LeptonjetSourcePFMuonProducer")
## 2. DSAMuon
leptonjetSourceDSAMuon = cms.EDProducer("LeptonjetSourceDSAMuonProducer")
## 3. PFElectron
leptonjetSourcePFElectron = cms.EDProducer("LeptonjetSourcePFElectronProducer")
## 4. PFPhoton
leptonjetSourcePFPhoton = cms.EDProducer("LeptonjetSourcePFPhotonProducer")

## merge into a single collection
from Firefighter.recoStuff.PFCandMerger_cfi import pfcandfwdptrmerger as _pfcandmerger
leptonjetSources = _pfcandmerger.clone(
    src=cms.VInputTag(
        cms.InputTag("leptonjetSourcePFMuon", "nonisolated"),
        cms.InputTag("leptonjetSourceDSAMuon", "nonisolated"),
        cms.InputTag("leptonjetSourcePFElectron", "nonisolated"),
        cms.InputTag("leptonjetSourcePFPhoton"),
    )
)

## anti-kt cluster
from RecoJets.Configuration.RecoPFJets_cff import ak4PFJets
leptonjet = ak4PFJets.clone(
    src=cms.InputTag("leptonjetSources"),
    rParam=cms.double(0.4),
    jetAlgorithm=cms.string("AntiKt"),
)

## clustering sequence
leptonjetClusteringSeq = cms.Sequence(
    leptonjetSourcePFMuon
    + leptonjetSourceDSAMuon
    + leptonjetSourcePFElectron
    + leptonjetSourcePFPhoton
    + leptonjetSources
    + leptonjet
)