import FWCore.ParameterSet.Config as cms

from TrackingTools.TransientTrack.TransientTrackBuilder_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi import *

ntuple_event = cms.PSet(
    NtupleName=cms.string('ffNtupleEvent')
)


ntuple_gen = cms.PSet(
    NtupleName=cms.string('ffNtupleGen'),
    GenParticles=cms.InputTag('genParticles'),
    GenPU=cms.InputTag('addPileupInfo'),
    MCEvent=cms.InputTag('generatorSmeared'),
    CylinderRZ=cms.VPSet(
        cms.PSet(
            radius=cms.double(3.),
            absZ=cms.double(29.1)
        ),  # pixel, BPix-LYR1, FPix-DSK1
        cms.PSet(
            radius=cms.double(60.),
            absZ=cms.double(123.)
        ),  # TOB, TEC inner
        cms.PSet(
            radius=cms.double(600.),
            absZ=cms.double(800.)
        )  # MB2, ME2 inner
    )
)


ntuple_genjet = cms.PSet(
    NtupleName=cms.string('ffNtupleGenJet'),
    GenJets=cms.InputTag('ak4GenJetsNoNu')
)

ntuple_hlt = cms.PSet(
    NtupleName=cms.string('ffNtupleHLT'),
    TriggerEvent=cms.InputTag("hltTriggerSummaryAOD", "", "HLT"),
    TriggerResults=cms.InputTag("TriggerResults", "", "HLT"),
    TriggerPaths=cms.vstring(
        'HLT_TrkMu12_DoubleTrkMu5NoFiltersNoVtx',
        'HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx'
    )
)

ntuple_muon = cms.PSet(
    NtupleName=cms.string("ffNtupleMuon"),
    src=cms.InputTag('muons')
)

ntuple_dsamuon = cms.PSet(
    NtupleName=cms.string("ffNtupleDsaMuon"),
    src=cms.InputTag('displacedStandAloneMuons'),
    UseMuonHypothesis=cms.bool(True)
)

ntuple_pfjet = cms.PSet(
    NtupleName=cms.string("ffNtuplePfJet"),
    src=cms.InputTag('ak4PFJets'),
    PrimaryVertices=cms.InputTag('offlinePrimaryVertices'),
    GeneralTracks=cms.InputTag('generalTracks'),
    ParticleFlowCands=cms.InputTag('particleFlow'),
    PFJetSelection=cms.string(' && '.join([
        'abs(eta)<2.4',
        'numberOfDaughters>1',
        'neutralHadronEnergyFraction<0.99',
        'neutralEmEnergyFraction<0.99',
        '(chargedEmEnergyFraction+muonEnergyFraction+neutralEmEnergyFraction)>0.5'
    ])),
    TrackSelection=cms.string(' && '.join([
        'pt>0.5',
        'normalizedChi2<5.'
    ])),
    kvfParam=cms.PSet(
        maxDistance=cms.double(0.01),  # It is said this is mm.
        maxNbrOfIterations=cms.int32(10),
        doSmoothing=cms.bool(True),
    ),
    IsolationRadius=cms.double(0.5),
)


ffNtuplizer = cms.EDAnalyzer(
    'ffNtupleManager',
    HltProcName=cms.string('HLT'),
    Ntuples=cms.VPSet(
        ntuple_event,
        ntuple_gen,
        ntuple_genjet,
        ntuple_hlt,
        ntuple_muon,
        ntuple_dsamuon,
        ntuple_pfjet
    )
)
