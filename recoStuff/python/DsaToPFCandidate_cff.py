import FWCore.ParameterSet.Config as cms
from TrackingTools.TrackAssociator.DetIdAssociatorESProducer_cff import *
from RecoMuon.MuonIdentification.muons1stStep_cfi import *
from Firefighter.recoStuff.MuonToPFCandProd_cfi import (
    PFCandsFromMuons as _PFCandsFromMuons,
)

muonsFromdSA = muons1stStep.clone()

muonsFromdSA.inputCollectionLabels = ["displacedStandAloneMuons"]
muonsFromdSA.inputCollectionTypes = ["outer tracks"]
muonsFromdSA.TrackExtractorPSet.inputTrackCollection = "displacedStandAloneMuons"
muonsFromdSA.TimingFillerParameters.DTTimingParameters.PruneCut = 9999
muonsFromdSA.TimingFillerParameters.CSCTimingParameters.PruneCut = 9999
muonsFromdSA.TrackAssociatorParameters.EERecHitCollectionLabel = cms.InputTag("reducedEcalRecHitsEE")
muonsFromdSA.TrackAssociatorParameters.EBRecHitCollectionLabel = cms.InputTag("reducedEcalRecHitsEB")
muonsFromdSA.TrackAssociatorParameters.HBHERecHitCollectionLabel = cms.InputTag("reducedHcalRecHits", "hbhereco")
muonsFromdSA.TrackAssociatorParameters.HORecHitCollectionLabel = cms.InputTag("reducedHcalRecHits", "horeco")
muonsFromdSA.TrackAssociatorParameters.useCalo = cms.bool(False)

muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.EERecHitCollectionLabel = cms.InputTag("reducedEcalRecHitsEE")
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.EBRecHitCollectionLabel = cms.InputTag("reducedEcalRecHitsEB")
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.HBHERecHitCollectionLabel = cms.InputTag("reducedHcalRecHits", "hbhereco")
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.HORecHitCollectionLabel = cms.InputTag("reducedHcalRecHits", "horeco")
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.useCalo = cms.bool(False)
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.useEcal = cms.bool(True)
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.useGEM = cms.bool(False)
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.useHO = cms.bool(True)
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.useHcal = cms.bool(True)
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.useME0 = cms.bool(False)
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.useMuon = cms.bool(True)
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.usePreshower = cms.bool(False)

muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.EERecHitCollectionLabel = cms.InputTag("reducedEcalRecHitsEE")
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.EBRecHitCollectionLabel = cms.InputTag("reducedEcalRecHitsEB")
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.HBHERecHitCollectionLabel = cms.InputTag("reducedHcalRecHits", "hbhereco")
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.HORecHitCollectionLabel = cms.InputTag("reducedHcalRecHits", "horeco")
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.useCalo = cms.bool(False)
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.useEcal = cms.bool(True)
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.useGEM = cms.bool(False)
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.useHO = cms.bool(True)
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.useHcal = cms.bool(True)
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.useME0 = cms.bool(False)
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.useMuon = cms.bool(True)
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.usePreshower = cms.bool(False)

muonsFromdSA.fillIsolation = False
muonsFromdSA.fillGlobalTrackQuality = False
muonsFromdSA.fillGlobalTrackRefits = False
muonsFromdSA.writeIsoDeposits = False
muonsFromdSA.arbitrateTrackerMuons = False
muonsFromdSA.fillTrackerKink = False
muonsFromdSA.runArbitrationCleaner = False

#####################################################################
from CommonTools.ParticleFlow.pfNoPileUpIso_cff import *
pfPileUpIso.PFCandidates = cms.InputTag("particleFlowPtrs")
pfNoPileUpIso.bottomCollection = cms.InputTag("particleFlowPtrs")

from CommonTools.ParticleFlow.ParticleSelectors.pfSortByType_cff import *
from RecoMuon.MuonIsolation.muonPFIsolation_cff import *

sourceMuons = 'muonsFromdSA'
muPFIsoDepositCharged.src = sourceMuons
muPFIsoDepositChargedAll.src = sourceMuons
muPFIsoDepositNeutral.src = sourceMuons
muPFIsoDepositGamma.src = sourceMuons
muPFIsoDepositPU.src = sourceMuons

pfEmbeddedDSAMuons = cms.EDProducer("MuPFIsoEmbedder",
    src = cms.InputTag("muonsFromdSA"),
    pfIsolationR03 = cms.PSet(
        chargedParticle = cms.InputTag("muPFIsoValueChargedAll03"),
        chargedHadron = cms.InputTag("muPFIsoValueCharged03"),
        neutralHadron = cms.InputTag("muPFIsoValueNeutral03"),
        photon = cms.InputTag("muPFIsoValueGamma03"),
        neutralHadronHighThreshold = cms.InputTag("muPFIsoValueNeutralHighThreshold03"),
        photonHighThreshold = cms.InputTag("muPFIsoValueGammaHighThreshold03"),
        pu = cms.InputTag("muPFIsoValuePU03")
    ),
    pfIsolationR04 = cms.PSet(
        chargedParticle = cms.InputTag("muPFIsoValueChargedAll04"),
        chargedHadron = cms.InputTag("muPFIsoValueCharged04"),
        neutralHadron = cms.InputTag("muPFIsoValueNeutral04"),
        photon = cms.InputTag("muPFIsoValueGamma04"),
        neutralHadronHighThreshold = cms.InputTag("muPFIsoValueNeutralHighThreshold04"),
        photonHighThreshold = cms.InputTag("muPFIsoValueGammaHighThreshold04"),
        pu = cms.InputTag("muPFIsoValuePU04")
    ),
    pfIsoMeanDRProfileR03 = cms.PSet(
        chargedParticle = cms.InputTag("muPFMeanDRIsoValueChargedAll03"),
        chargedHadron = cms.InputTag("muPFMeanDRIsoValueCharged03"),
        neutralHadron = cms.InputTag("muPFMeanDRIsoValueNeutral03"),
        photon = cms.InputTag("muPFMeanDRIsoValueGamma03"),
        neutralHadronHighThreshold = cms.InputTag("muPFMeanDRIsoValueNeutralHighThreshold03"),
        photonHighThreshold = cms.InputTag("muPFMeanDRIsoValueGammaHighThreshold03"),
        pu = cms.InputTag("muPFMeanDRIsoValuePU03")
    ),
    pfIsoMeanDRProfileR04 = cms.PSet(
        chargedParticle = cms.InputTag("muPFMeanDRIsoValueChargedAll04"),
        chargedHadron = cms.InputTag("muPFMeanDRIsoValueCharged04"),
        neutralHadron = cms.InputTag("muPFMeanDRIsoValueNeutral04"),
        photon = cms.InputTag("muPFMeanDRIsoValueGamma04"),
        neutralHadronHighThreshold = cms.InputTag("muPFMeanDRIsoValueNeutralHighThreshold04"),
        photonHighThreshold = cms.InputTag("muPFMeanDRIsoValueGammaHighThreshold04"),
        pu = cms.InputTag("muPFMeanDRIsoValuePU04")
    ),
    pfIsoSumDRProfileR03 = cms.PSet(
        chargedParticle = cms.InputTag("muPFSumDRIsoValueChargedAll03"),
        chargedHadron = cms.InputTag("muPFSumDRIsoValueCharged03"),
        neutralHadron = cms.InputTag("muPFSumDRIsoValueNeutral03"),
        photon = cms.InputTag("muPFSumDRIsoValueGamma03"),
        neutralHadronHighThreshold = cms.InputTag("muPFSumDRIsoValueNeutralHighThreshold03"),
        photonHighThreshold = cms.InputTag("muPFSumDRIsoValueGammaHighThreshold03"),
        pu = cms.InputTag("muPFSumDRIsoValuePU03")
    ),
    pfIsoSumDRProfileR04 = cms.PSet(
        chargedParticle = cms.InputTag("muPFSumDRIsoValueChargedAll04"),
        chargedHadron = cms.InputTag("muPFSumDRIsoValueCharged04"),
        neutralHadron = cms.InputTag("muPFSumDRIsoValueNeutral04"),
        photon = cms.InputTag("muPFSumDRIsoValueGamma04"),
        neutralHadronHighThreshold = cms.InputTag("muPFSumDRIsoValueNeutralHighThreshold04"),
        photonHighThreshold = cms.InputTag("muPFSumDRIsoValueGammaHighThreshold04"),
        pu = cms.InputTag("muPFSumDRIsoValuePU04")
    ),
)

DSAMuonIsolationSequence = cms.Sequence(
    pfNoPileUpIsoSequence
    + pfSortByTypeSequence
    + muonPFIsolationSequence
    + pfEmbeddedDSAMuons
)

#####################################################################

pfcandsFromMuondSA = _PFCandsFromMuons.clone(src=cms.InputTag("pfEmbeddedDSAMuons"))

pfcandsFromMuondSAPtr = cms.EDProducer(
    "PFCandidateFwdPtrProducer",
    src=cms.InputTag("pfcandsFromMuondSA")
)

#####################################################################

dSAToPFCandSeq = cms.Sequence(muonsFromdSA + DSAMuonIsolationSequence + pfcandsFromMuondSA + pfcandsFromMuondSAPtr)
