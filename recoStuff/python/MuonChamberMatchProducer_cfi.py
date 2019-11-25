import FWCore.ParameterSet.Config as cms
from Firefighter.recoStuff.DsaToPFCandidate_cff import muonsFromdSA
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi import *
from TrackingTools.TrackAssociator.DetIdAssociatorESProducer_cff import *

muonChamberMatchProducer = cms.EDProducer(
    "MuonChamberMatchProducer",
    src=cms.InputTag("cosmicMuons1Leg"),
    TrackAssociatorParameters=muonsFromdSA.TrackAssociatorParameters.clone(),
    propagateOpposite=cms.bool(False),
    maxAbsDx=muonsFromdSA.maxAbsDx,
    maxAbsDy=muonsFromdSA.maxAbsDy,
    maxAbsPullX=muonsFromdSA.maxAbsPullX,
    maxAbsPullY=muonsFromdSA.maxAbsPullY,
)