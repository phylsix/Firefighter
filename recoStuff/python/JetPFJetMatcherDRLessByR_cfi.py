import FWCore.ParameterSet.Config as cms

jetpfjetmatcherdrlessbyr = cms.EDProducer(
    "JetPFJetMatcherDRLessByR",
    src=cms.InputTag("ffLeptonJetSubjets", "", "FF"),
    matched=cms.InputTag("ffLeptonJet", "", "FF"),
    resolveAmbiguities=cms.bool(True),
    resolveByMatchQuality=cms.bool(True),
    # For matcher
    maxDeltaR=cms.double(0.4),
    # For pre-selector
    cut=cms.string("abs(eta)<2.5 && numberOfDaughters>0"),
    maxdist=cms.double(0.4),
)
