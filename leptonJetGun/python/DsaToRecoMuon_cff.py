import FWCore.ParameterSet.Config as cms

from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi import *
from TrackingTools.TrackAssociator.DetIdAssociatorESProducer_cff import *
from RecoMuon.MuonIdentification.muons1stStep_cfi import *

###############################################################################

muonsFromdSA = muons1stStep.clone()

muonsFromdSA.inputCollectionLabels = ["displacedStandAloneMuons"]
muonsFromdSA.inputCollectionTypes = ["outer tracks"]
muonsFromdSA.TrackExtractorPSet.inputTrackCollection = "displacedStandAloneMuons"
muonsFromdSA.TimingFillerParameters.DTTimingParameters.PruneCut = 9999
muonsFromdSA.TimingFillerParameters.CSCTimingParameters.PruneCut = 9999
muonsFromdSA.TrackAssociatorParameters.EERecHitCollectionLabel = cms.InputTag(
    "reducedEcalRecHitsEE"
)
muonsFromdSA.TrackAssociatorParameters.EBRecHitCollectionLabel = cms.InputTag(
    "reducedEcalRecHitsEB"
)
muonsFromdSA.TrackAssociatorParameters.HBHERecHitCollectionLabel = cms.InputTag(
    "reducedHcalRecHits", "hbhereco"
)
muonsFromdSA.TrackAssociatorParameters.HORecHitCollectionLabel = cms.InputTag(
    "reducedHcalRecHits", "horeco"
)
muonsFromdSA.fillIsolation = False
muonsFromdSA.fillGlobalTrackQuality = False
muonsFromdSA.fillGlobalTrackRefits = False
muonsFromdSA.writeIsoDeposits = False
muonsFromdSA.arbitrateTrackerMuons = False
muonsFromdSA.fillTrackerKink = False
muonsFromdSA.runArbitrationCleaner = False

###############################################################################

dSAToRecoMuonSeq = cms.Sequence(muonsFromdSA)
