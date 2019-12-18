import FWCore.ParameterSet.Config as cms
from Firefighter.recoStuff.DsaToPFCandidate_cff import muonsFromdSA
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi import *
from TrackingTools.TrackAssociator.DetIdAssociatorESProducer_cff import *
from TrackingTools.TrackAssociator.default_cfi import TrackAssociatorParameterBlock

muonChamberMatchProducer = cms.EDProducer(
    "MuonChamberMatchProducer",
    src=cms.InputTag("cosmicMuons1Leg"),
    TrackAssociatorParameters=TrackAssociatorParameterBlock.TrackAssociatorParameters,
    propagateOpposite=cms.bool(True),
    maxAbsDx=muonsFromdSA.maxAbsDx,
    maxAbsDy=muonsFromdSA.maxAbsDy,
    maxAbsPullX=muonsFromdSA.maxAbsPullX,
    maxAbsPullY=muonsFromdSA.maxAbsPullY,
)
muonChamberMatchProducer.TrackAssociatorParameters.DTRecSegment4DCollectionLabel = cms.InputTag('dt4DCosmicSegments')
muonChamberMatchProducer.TrackAssociatorParameters.useEcal = cms.bool(False)
muonChamberMatchProducer.TrackAssociatorParameters.useHO = cms.bool(False)
muonChamberMatchProducer.TrackAssociatorParameters.useHcal = cms.bool(False)
