import FWCore.ParameterSet.Config as cms

from RecoJets.Configuration.RecoPFJets_cff import ak4PFJets
from RecoJets.JetProducers.SubJetParameters_cfi import SubJetParameters
from RecoJets.JetProducers.ECFAdder_cfi import ECFAdder
from Firefighter.recoStuff.JetPFJetMatcherDRLessByR_cfi import (
    jetpfjetmatcherdrlessbyr as _jetpfjetmatcherdrlessbyr,
)
from Firefighter.recoStuff.JetEMDAdder_cfi import jetemdadder as _jetemdadder

ffLeptonJetConstituents = cms.EDProducer(
    "PFJetConstituentSelector",
    src=cms.InputTag("ffLeptonJet"),
    cut=cms.string("abs(eta)<2.5"),
)

ffLeptonJetSubjets = ak4PFJets.clone(
    SubJetParameters.clone(nFilt=cms.int32(5)),
    src=cms.InputTag("ffLeptonJetConstituents", "constituents"),
    writeCompound=cms.bool(True),
    jetCollInstanceName=cms.string("SubJets"),
)

ffLeptonJetSLeptonJetMap = _jetpfjetmatcherdrlessbyr.clone()

ffLeptonJetSubjetEMD = _jetemdadder.clone()

maxECF = 3
ecfBeta = 1.0
ffLeptonJetSubjetECF = ECFAdder.clone(
    src=cms.InputTag("ffLeptonJet"),
    Njets=cms.vuint32(range(1, maxECF + 1)),
    beta=cms.double(ecfBeta),
)

ffLeptonJetSubjetSeq = cms.Sequence(
    (
        ffLeptonJetConstituents
        + ffLeptonJetSubjets
        + ffLeptonJetSLeptonJetMap
        + ffLeptonJetSubjetEMD
    )
    * ffLeptonJetSubjetECF
)
