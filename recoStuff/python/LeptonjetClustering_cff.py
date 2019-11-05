import FWCore.ParameterSet.Config as cms

## soure generation
## 1. PFMuon
leptonjetSourcePFMuon = cms.EDProducer("LeptonjetSourcePFMuonProducer")
## 2. DSAMuon
from Firefighter.recoStuff.DsaAdditionalValues_cff import *
leptonjetSourceDSAMuon = cms.EDProducer(
    "LeptonjetSourceDSAMuonProducer",
    minDTTimeDiff=cms.double(-20.),
    minRPCTimeDiff=cms.double(-7.5)
)
## 3. PFElectron
leptonjetSourcePFElectron = cms.EDProducer(
    "LeptonjetSourcePFElectronProducer",
    idName=cms.string("cutBasedElectronID-Fall17-94X-V2-loose")
)
## 4. PFPhoton
leptonjetSourcePFPhoton = cms.EDProducer(
    "LeptonjetSourcePFPhotonProducer",
    idName=cms.string("cutBasedPhotonID-Fall17-94X-V2-loose")
)

## merge into a single collection
from Firefighter.recoStuff.PFCandMerger_cfi import pfcandfwdptrmerger as _pfcandmerger
leptonjetSources = _pfcandmerger.clone(
    src=cms.VInputTag(
        cms.InputTag("leptonjetSourcePFMuon", "inclusive"),
        cms.InputTag("leptonjetSourceDSAMuon", "inclusive"),
        cms.InputTag("leptonjetSourcePFElectron", "inclusive"),
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

## produce addiitonal valueMaps
leptonjetExtra = cms.EDProducer(
    "LeptonjetValueMapProducer",
    src=cms.InputTag("leptonjet"),
)

## clustering sequence
leptonjetClusteringSeq = cms.Sequence(
    leptonjetSourcePFMuon
    + dsamuonExtraSeq
    + leptonjetSourceDSAMuon
    + leptonjetSourcePFElectron
    + leptonjetSourcePFPhoton
    + leptonjetSources
    + leptonjet
    + leptonjetExtra
)

## filtering sequence
from Firefighter.recoStuff.ffLeptonJetFiltering_cff import *
leptonjetFilteringSeq = cms.Sequence(
    ffLeptonJetFwdPtrs
    + filteredLeptonJet
)