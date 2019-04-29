import FWCore.ParameterSet.Config as cms

from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi import *
from TrackingTools.TrackAssociator.DetIdAssociatorESProducer_cff import *
from RecoMuon.MuonIdentification.muons1stStep_cfi import *
from Firefighter.recoStuff.HLTFilter_cfi import hltfilter as _hltfilter
from Firefighter.recoStuff.CosmicFilter_cfi import cosmicmuonfilter as _cosmicfilter

noBPTXHLTFilter = _hltfilter.clone(
    TriggerPaths=cms.vstring("HLT_L2Mu10_NoVertex_NoBPTX3BX")
)

#####################################################################

muonsFromdSA = muons1stStep.clone()

muonsFromdSA.inputCollectionLabels = ["displacedStandAloneMuons"]
muonsFromdSA.inputCollectionTypes = ["outer tracks"]
muonsFromdSA.TrackExtractorPSet.inputTrackCollection = "displacedStandAloneMuons"
muonsFromdSA.TimingFillerParameters.DTTimingParameters.PruneCut = 9999
muonsFromdSA.TimingFillerParameters.CSCTimingParameters.PruneCut = 9999
muonsFromdSA.TrackAssociatorParameters.useEcal = False
muonsFromdSA.TrackAssociatorParameters.useHO = False
muonsFromdSA.fillIsolation = False
muonsFromdSA.fillGlobalTrackQuality = False
muonsFromdSA.fillGlobalTrackRefits = False
muonsFromdSA.writeIsoDeposits = False
muonsFromdSA.arbitrateTrackerMuons = False
muonsFromdSA.fillTrackerKink = False
muonsFromdSA.runArbitrationCleaner = False

#####################################################################

cosmicFilter = _cosmicfilter.clone(muons=cms.InputTag("muonsFromdSA"))

noBPTXSkimMuonTimeSeq = cms.Sequence(noBPTXHLTFilter + muonsFromdSA + cosmicFilter)
