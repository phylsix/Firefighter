import FWCore.ParameterSet.Config as cms

from TrackingTools.TransientTrack.TransientTrackBuilder_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi import *

ntuple_event = cms.PSet(
    NtupleName=cms.string("ffNtupleEvent"),
    PileUp=cms.InputTag("addPileupInfo"),
    GenProd=cms.InputTag("generator"),
)


ntuple_gen = cms.PSet(
    NtupleName=cms.string("ffNtupleGen"),
    GenParticles=cms.InputTag("genParticles"),
    MCEvent=cms.InputTag("generatorSmeared"),
    CylinderRZ=cms.VPSet(
        cms.PSet(
            radius=cms.double(3.0), absZ=cms.double(29.1)
        ),  # pixel, BPix-LYR1, FPix-DSK1
        cms.PSet(radius=cms.double(60.0), absZ=cms.double(123.0)),  # TOB, TEC inner
        cms.PSet(radius=cms.double(600.0), absZ=cms.double(800.0)),  # MB2, ME2 inner
    ),
)


ntuple_genjet = cms.PSet(
    NtupleName=cms.string("ffNtupleGenJet"), GenJets=cms.InputTag("ak4GenJetsNoNu")
)

ntuple_hlt = cms.PSet(
    NtupleName=cms.string("ffNtupleHLT"),
    TriggerEvent=cms.InputTag("hltTriggerSummaryAOD", "", "HLT"),
    TriggerResults=cms.InputTag("TriggerResults", "", "HLT"),
    TriggerPaths=cms.vstring(
        "HLT_TrkMu12_DoubleTrkMu5NoFiltersNoVtx",
        "HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx",
    ),
)

ntuple_muon = cms.PSet(NtupleName=cms.string("ffNtupleMuon"), src=cms.InputTag("muons"))

ntuple_electron = cms.PSet(
    NtupleName=cms.string("ffNtupleElectron"), src=cms.InputTag("gedGsfElectrons")
)

ntuple_dsamuon = cms.PSet(
    NtupleName=cms.string("ffNtupleDsaMuon"),
    src=cms.InputTag("displacedStandAloneMuons"),
    UseMuonHypothesis=cms.bool(True),
)

ntuple_pfjet = cms.PSet(
    NtupleName=cms.string("ffNtuplePfJet"),
    src=cms.InputTag("ffLeptonJet"),
    PrimaryVertices=cms.InputTag("offlinePrimaryVertices"),
    GeneralTracks=cms.InputTag("generalTracks"),
    ParticleFlowCands=cms.InputTag("particleFlow"),
    PFJetSelection=cms.string(
        " && ".join(
            [
                "abs(eta)<2.4",
                "numberOfDaughters>1",
                # "neutralHadronEnergyFraction<0.99",
                # "neutralEmEnergyFraction<0.99",
                # "(chargedEmEnergyFraction+muonEnergyFraction+neutralEmEnergyFraction)>0.6",
                "!test_bit(muonMultiplicity(), 0)",  # even multiplicity
                "!test_bit(electronMultiplicity(), 0)",  # even multiplicity
            ]
        )
    ),
    TrackSelection=cms.string(" && ".join(["pt>0.5", "normalizedChi2<5."])),
    kvfParam=cms.PSet(
        maxDistance=cms.double(0.01),  # It is said this is mm.
        maxNbrOfIterations=cms.int32(10),
        doSmoothing=cms.bool(True),
    ),
    IsolationRadius=cms.vdouble(0.5, 0.6, 0.7),
    MinChargedMass=cms.double(0.0),
    MaxTimeLimit=cms.double(10.),
    SubjetEnergyDistributioin=cms.InputTag("ffLeptonJetSubjetEMD", "energy"),
    SubjetMomentumDistribution=cms.InputTag("ffLeptonJetSubjetEMD", "momentum"),
    SubjetEcf1=cms.InputTag("ffLeptonJetSubjetECF", "ecf1"),
    SubjetEcf2=cms.InputTag("ffLeptonJetSubjetECF", "ecf2"),
    SubjetEcf3=cms.InputTag("ffLeptonJetSubjetECF", "ecf3"),
)

ntuple_hftagscore = cms.PSet(
    NtupleName=cms.string("ffNtupleHFtagScore"),
    jet=cms.InputTag("ak4PFJetsCHS"),
    deepcsv_b=cms.InputTag("pfDeepCSVJetTags", "probb"),
    deepcsv_bb=cms.InputTag("pfDeepCSVJetTags", "probbb"),
    deepcsv_c=cms.InputTag("pfDeepCSVJetTags", "probc"),
    deepcsv_udsg=cms.InputTag("pfDeepCSVJetTags", "probudsg"),
    csvv2_b=cms.InputTag("pfCombinedInclusiveSecondaryVertexV2BJetTags"),
)

ntuple_muontiming = cms.PSet(
    NtupleName=cms.string("ffNtupleMuonTiming"),
    src=cms.InputTag("muonsFromdSA"),
    instances=cms.vstring("combined", "csc", "dt"),
)

ntuple_beamhalo = cms.PSet(
    NtupleName=cms.string("ffNtupleBeamHalo"), src=cms.InputTag("BeamHaloSummary")
)

ffNtuplizer = cms.EDAnalyzer(
    "ffNtupleManager",
    HltProcName=cms.string("HLT"),
    Ntuples=cms.VPSet(
        ntuple_event,
        ntuple_gen,
        ntuple_genjet,
        ntuple_hlt,
        ntuple_muon,
        ntuple_electron,
        ntuple_dsamuon,
        ntuple_pfjet,
        ntuple_hftagscore,
        ntuple_muontiming,
        ntuple_beamhalo,
    ),
)
