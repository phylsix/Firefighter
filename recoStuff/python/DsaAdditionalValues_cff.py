import FWCore.ParameterSet.Config as cms
# from Firefighter.recoStuff.MuonChamberMatchProducer_cfi import muonChamberMatchProducer as cosmicMuon1LegChamberMatch
from Firefighter.recoStuff.ReRecoCosmicMuonOneLeg_cff import *

# This requires Firefighter.recoStuff.DsaToPFCandidate_cff ahead.
dsamuonExtra = cms.EDProducer(
    "DSAMuonValueMapProducer",
    src=cms.InputTag("pfEmbeddedDSAMuons"),
    cosmic=cms.InputTag( "FFcosmicMuons1Leg" ),
    cosmicMatchCut=cms.PSet(
        minPt=cms.double(10.),
        maxNormChi2=cms.double(20.),
        minNumChamberTop=cms.int32(2),
        minNumChamberBottom=cms.int32(2),
        minImpactDist2D=cms.double(20.),
        minCount=cms.uint32(2),
    ),
)
# cosmicMuon1LegChamberMatch.propagateOpposite = cms.bool(True)
# dsamuonExtraSeq = cms.Sequence(cosmicMuon1LegChamberMatch + muoncosmicreco1legSTA + dsamuonExtra)
dsamuonExtraSeq = cms.Sequence(muoncosmicreco1legSTA + dsamuonExtra)