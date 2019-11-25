import FWCore.ParameterSet.Config as cms
from Firefighter.recoStuff.MuonChamberMatchProducer_cfi import muonChamberMatchProducer as cosmicMuon1LegChamberMatch

# This requires Firefighter.recoStuff.DsaToPFCandidate_cff ahead.
dsamuonExtra = cms.EDProducer(
    "DSAMuonValueMapProducer",
    src=cms.InputTag("pfEmbeddedDSAMuons"),
    cosmicMatchCut=cms.PSet(
        minpt=cms.double(5.),
        requireDT=cms.bool(True),
        mindz=cms.double(48.), # 2*24
    ),
)

dsamuonExtraSeq = cms.Sequence(cosmicMuon1LegChamberMatch + dsamuonExtra)