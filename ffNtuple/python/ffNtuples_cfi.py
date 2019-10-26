import FWCore.ParameterSet.Config as cms

from TrackingTools.TransientTrack.TransientTrackBuilder_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi import *
from TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi import *

from JetMETCorrections.Configuration.JetCorrectors_cff import *

from Firefighter.recoStuff.ffLeptonJetMVAEstimator_cfi import *
from Firefighter.recoStuff.jetIdentificationDefs_cfi import jetiddefs

ntuple_event = cms.PSet(
    NtupleName=cms.string("ffNtupleEvent"),
)

ntuple_genevent = cms.PSet(
    NtupleName=cms.string("ffNtupleGenEvent"),
    PileUp=cms.InputTag("addPileupInfo"),
    GenProd=cms.InputTag("generator"),
)

ntuple_primaryvertex = cms.PSet(
    NtupleName=cms.string('ffNtuplePrimaryVertex'),
    src=cms.InputTag("offlinePrimaryVertices"),
)

ntuple_gen = cms.PSet(
    NtupleName=cms.string("ffNtupleGen"),
    src=cms.InputTag("genParticles"),
    AllowedPids=cms.vint32([35, 32, 13, 11]), # XX, A, mu, e
    MCEvent=cms.InputTag("generatorSmeared"),
    CylinderRZ=cms.VPSet(
        cms.PSet(radius=cms.double(3.0), absZ=cms.double(29.1)),  # pixel, BPix-LYR1, FPix-DSK1
        cms.PSet(radius=cms.double(60.0), absZ=cms.double(123.0)),  # TOB, TEC inner
        cms.PSet(radius=cms.double(490.0), absZ=cms.double(800.0)),  # MB2, ME2 inner
        cms.PSet(radius=cms.double(600.0), absZ=cms.double(920.0)),  # MB3, ME3 inner
    ),
)

ntuple_genbkg = cms.PSet(
    NtupleName=cms.string("ffNtupleGenBkg"),
    src=cms.InputTag("genParticles"),
    AllowedPids=cms.vint32([6, 22, 23, 24, 25]), # top, gamma, Z, W, Higgs
    AllowedStatusOnePids=cms.vint32([13,]), # mu
)

ntuple_genjet = cms.PSet(
    NtupleName=cms.string("ffNtupleGenJet"),
    GenJets=cms.InputTag("ak4GenJetsNoNu")
)

ntuple_hlt = cms.PSet(
    NtupleName=cms.string("ffNtupleHLT"),
    TriggerEvent=cms.InputTag("hltTriggerSummaryAOD", "", "HLT"),
    TriggerResults=cms.InputTag("TriggerResults", "", "HLT"),
    TriggerPaths=cms.vstring(
        "HLT_DoubleL2Mu23NoVtx_2Cha",
        "HLT_DoubleL2Mu23NoVtx_2Cha_NoL2Matched",
        "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed",
        "HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed_NoL2Matched",
        "HLT_DoubleL2Mu25NoVtx_2Cha",
        "HLT_DoubleL2Mu25NoVtx_2Cha_NoL2Matched",
        "HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed",
        "HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed_NoL2Matched",
        "HLT_DoubleL2Mu25NoVtx_2Cha_Eta2p4",
        "HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed_Eta2p4",
        # mu-photon triggers
        "HLT_Mu38NoFiltersNoVtxDisplaced_Photon38_CaloIdL",
        "HLT_Mu43NoFiltersNoVtx_Photon43_CaloIdL",
    ),
)

ntuple_muon = cms.PSet(
    NtupleName=cms.string("ffNtupleMuon"),
    src=cms.InputTag("muons")
)

ntuple_electron = cms.PSet(
    NtupleName=cms.string("ffNtupleElectron"),
    src=cms.InputTag("gedGsfElectrons"),
    idVersion=cms.string("cutBasedElectronID-Fall17-94X-V2"),
    idLabel=cms.string("loose"), # veto, loose, medium, tight
)
def _getElectronCutNames(version, label):
    psetname = '{}_{}'.format(version.value().replace('-', '_'), label.value())
    modulename = 'RecoEgamma.ElectronIdentification.Identification.{}'.format(psetname.rsplit('_', 1)[0]+'_cff')

    import importlib
    psets = importlib.import_module(modulename).__dict__[psetname]
    _cutnames = [x.cutName.value() for x in psets.cutFlow]
    cutnames = [_cutnames[i]+'_'+str(_cutnames[:i].count(_cutnames[i])) for i in range(len(_cutnames))]
    return cms.vstring(*cutnames)

ntuple_electron.cutNames=_getElectronCutNames(ntuple_electron.idVersion, ntuple_electron.idLabel)

ntuple_photon = cms.PSet(
    NtupleName=cms.string("ffNtuplePhoton"),
    src=cms.InputTag("gedPhotons"),
    idVersion=cms.string("cutBasedPhotonID-Fall17-94X-V2"),
    idLabel=cms.string("loose"), # loose, medium, tight
)
def _getPhotonCutNames(version, label):
    psetname = '{}_{}'.format(version.value().replace('-', '_'), label.value())
    modulename = 'RecoEgamma.PhotonIdentification.Identification.{}'.format(psetname.rsplit('_', 1)[0]+'_cff')

    import importlib
    psets = importlib.import_module(modulename).__dict__[psetname]
    _cutnames = [x.cutName.value() for x in psets.cutFlow]
    cutnames = [_cutnames[i]+'_'+str(_cutnames[:i].count(_cutnames[i])) for i in range(len(_cutnames))]
    return cms.vstring(*cutnames)
ntuple_photon.cutNames=_getPhotonCutNames(ntuple_photon.idVersion, ntuple_photon.idLabel)

ntuple_dsamuon = cms.PSet(
    NtupleName=cms.string("ffNtupleDsaMuon"),
    src=cms.InputTag("pfEmbeddedDSAMuons"),
)

ntuple_pfjet = cms.PSet(
    NtupleName=cms.string("ffNtuplePfJet"),
    src=cms.InputTag("filteredLeptonJet"),
    PrimaryVertices=cms.InputTag("offlinePrimaryVertices"),
    GeneralTracks=cms.InputTag("generalTracks"),
    ParticleFlowCands=cms.InputTag("pfNoPileUpIso"),
    TrackSelection=cms.string(" && ".join(["pt>0.5", "normalizedChi2<5."])),
    kvfParam=cms.PSet(
        maxDistance=cms.double(0.01),  # It is said this is mm.
        maxNbrOfIterations=cms.int32(10),
        doSmoothing=cms.bool(True),
    ),
    IsolationRadius=cms.vdouble(0.5, 0.6, 0.7, 0.8),
    SubjetEnergyDistributioin=cms.InputTag("ffLeptonJetSubjetEMD", "energy"),
    SubjetMomentumDistribution=cms.InputTag("ffLeptonJetSubjetEMD", "momentum"),
    SubjetEcf1=cms.InputTag("ffLeptonJetSubjetECF", "ecf1"),
    SubjetEcf2=cms.InputTag("ffLeptonJetSubjetECF", "ecf2"),
    SubjetEcf3=cms.InputTag("ffLeptonJetSubjetECF", "ecf3"),
    mvaParam=ffLeptonJetMVAEstimatorParam,
    doVertexing=cms.bool(True),
    doSubstructureVariables=cms.bool(True),
    doMVA=cms.bool(True),
)

ntuple_akjet = cms.PSet(
    NtupleName=cms.string("ffNtupleAKJet"),
    src=cms.InputTag("ak4PFJetsCHS"),
    jetid=jetiddefs,
    corrector=cms.InputTag("ak4PFCHSL1FastL2L3Corrector"),
)

ntuple_hftagscore = cms.PSet(
    NtupleName=cms.string("ffNtupleHFtagScore"),
    jet=cms.InputTag("ak4PFJetsCHS"),
    deepcsv_b=cms.InputTag("pfDeepCSVJetTags", "probb"),
    deepcsv_bb=cms.InputTag("pfDeepCSVJetTags", "probbb"),
    deepcsv_c=cms.InputTag("pfDeepCSVJetTags", "probc"),
    deepcsv_udsg=cms.InputTag("pfDeepCSVJetTags", "probudsg"),
    deepflavour_b=cms.InputTag('pfDeepFlavourJetTags', 'probb'),
    deepflavour_bb=cms.InputTag('pfDeepFlavourJetTags', 'probbb'),
    deepflavour_lepb=cms.InputTag('pfDeepFlavourJetTags', 'problepb'),
    deepflavour_uds=cms.InputTag('pfDeepFlavourJetTags', 'probuds'),
    deepflavour_c=cms.InputTag('pfDeepFlavourJetTags', 'probc'),
    deepflavour_g=cms.InputTag('pfDeepFlavourJetTags', 'probg')
)

ntuple_muontiming = cms.PSet(
    NtupleName=cms.string("ffNtupleMuonTiming"),
    src=cms.InputTag("muonsFromdSA"),
    instances=cms.vstring("combined", "csc", "dt"),
)

ntuple_beamhalo = cms.PSet(
    NtupleName=cms.string("ffNtupleBeamHalo"),
    src=cms.InputTag("BeamHaloSummary")
)

ntuple_metfilters = cms.PSet(
    NtupleName=cms.string("ffNtupleMetFilters"),
)

ntuple_leptonjetsrc = cms.PSet(
    NtupleName=cms.string("ffNtupleLeptonJetSource"),
    src=cms.InputTag("particleFlowIncDSA"),
)

ntuple_leptonjetmisc = cms.PSet(
    NtupleName=cms.string("ffNtupleLeptonJetMisc"),
)

from Firefighter.ffEvtFilters.ffTriggerObjectsMatchingFilter_cfi import triggerObjectMatchingFilter
ntuple_triggerobjectmatching = cms.PSet(
    NtupleName=cms.string("ffNtupleTriggerObjectMatchingFilter"),
    debug=cms.bool(True),
    triggerNames=triggerObjectMatchingFilter.triggerNames,
)

ntuple_pfjetextra = cms.PSet(
    NtupleName=cms.string("ffNtuplePfJetExtra"),
    src=cms.InputTag("leptonjet"),
)

ntuple_cosmicveto = cms.PSet(
    NtupleName=cms.string("ffNtupleCosmicVeto"),
)

ntuple_akjetnolj = cms.PSet(
    NtupleName=cms.string("ffNtupleAKJet"),
    src=cms.InputTag("ak4chsNoLeptonjets"),
    cut=cms.string("pt>10."),
    jetid=jetiddefs,
)

ntuple_genbkgcheck = cms.PSet(
    NtupleName=cms.string("ffNtupleGenBkgCheck"),
)

ffNtupleStat = cms.EDAnalyzer("ffNtupleProcessStats")