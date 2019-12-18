import FWCore.ParameterSet.Config as cms

## seed generateor
from RecoMuon.MuonSeedGenerator.CosmicMuonSeedProducer_cfi import *
CosmicMuonSeed.DTRecSegmentLabel = 'dt4DCosmicSegments'

## STA track producer
from RecoMuon.CosmicMuonProducer.cosmicMuons_cff import *
FFcosmicMuons1Leg = cosmicMuons.clone()
FFcosmicMuons1Leg.TrajectoryBuilderParameters.BuildTraversingMuon = True
FFcosmicMuons1Leg.TrajectoryBuilderParameters.Strict1Leg = True
FFcosmicMuons1Leg.TrajectoryBuilderParameters.DTRecSegmentLabel = 'dt4DCosmicSegments'
FFcosmicMuons1Leg.MuonSeedCollectionLabel = 'CosmicMuonSeed'

muoncosmicreco1legSTA = cms.Sequence(CosmicMuonSeed*FFcosmicMuons1Leg)
