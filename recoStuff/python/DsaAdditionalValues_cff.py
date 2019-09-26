import FWCore.ParameterSet.Config as cms

# This requires Firefighter.recoStuff.DsaToPFCandidate_cff ahead.
dsamuonExtra = cms.EDProducer(
    "DSAMuonValueMapProducer",
    src=cms.InputTag("pfEmbeddedDSAMuons"),
)

dsamuonExtraSeq = cms.Sequence(dsamuonExtra)