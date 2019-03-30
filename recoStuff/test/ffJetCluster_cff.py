import FWCore.ParameterSet.Config as cms

from RecoJets.Configuration.RecoGenJets_cff import ak4GenJets
from RecoJets.Configuration.RecoPFJets_cff import ak4PFJets
from PhysicsTools.PatAlgos.tools.jetTools import addJetCollection

PUMethd = "Plain"
postFix = "Comb"
newPFCollection = True
nameNewPFCollection = "combPFdSA"
Cut = ""


jetAlgo = "ak"
algorithm = "AntiKt"
size = "4"
jetSize = 0.4
jetALGO = "AK4"
jetalgo = "ak4"
JETCorrLevels = ["L1FastJet", "L2Relative", "L3Absolute"]
subJETCorrLevels = ["L1FastJet", "L2Relative", "L3Absolute"]
JETCorrPayload = "AK" + size + "PF"  # AK4PF
subJETCorrPayload = "AK4PF"
JEC = ("AK4PF", subJETCorrLevels, "None")
patJets = "patJets"
patSubJets = ""
selPatJets = "selectedPatJets"


defaultBTagDiscriminators = [
    "pfTrackCountingHighEffBJetTags",
    "pfTrackCountingHighPurBJetTags",
    "pfJetProbabilityBJetTags",
    "pfJetBProbabilityBJetTags",
    "pfSimpleSecondaryVertexHighEffBJetTags",
    "pfSimpleSecondaryVertexHighPurBJetTags",
    "pfCombinedSecondaryVertexV2BJetTags",
    "pfCombinedInclusiveSecondaryVertexV2BJetTags",
    "pfCombinedMVAV2BJetTags",
]

bTagDiscriminators = defaultBTagDiscriminators
subjetBTagDiscriminators = defaultBTagDiscriminators

mod = {
    "PATJetsLabel": jetALGO + "PF" + PUMethod,  # AK4PFPlain
    "PATJetsLabelPost": mod["PATJetsLabel"] + postFix,  # AK4PFPlainComb
    "SubstructureLabel": jetALGO + PUMethod + postFix,  # AK4PlainComb
    "GenJetsNoNu": jetalgo + "GenJetsNoNu",  # ak4GenJetsNoNu
    "PFJets": jetalgo + "PFJets" + postFix,  # ak4PFJetsComb
    "PFJetsConstituents": mod["PFJets"] + "Constituents",  # ak4PFJetsCombConstituents
    "PFJetsConstituentsColon": mod["PFJets"]
    + "Constituents:constituents",  # ak4PFJetsCombConstituents:constituents
    "PFJetsConstituentsColonOrUpdate": mod["PFJetsConstituentsColon"],
    "PATJets": patJets + mod["PATJetsLabelPost"],  # patJetsAK4PFPlainComb
    "selPATJets": selPatJets + mod["PATJetsLabelPost"],  # selectedPatJetsAK4PFPlainComb
}

genParticlesLabel = "genParticles"
pvLabel = "offlinePrimaryVertices"
tvLabel = "generalTracks"
muLabel = "muons"
elLabel = "gedGsfElectrons"
pfCand = "combPFdSA"
svLabel = "inclusiveCandidateSecondaryVertices"

# runOnMC
process.load("RecoJets.Configuration.GenJetParticles_cff")
# mod['GenJetsNoNu']
process.ak4GenJetsNoNu = ak4GenJets.clone(
    src="genParticlesForJetsNoNu",
    rParam=jetSize,  # 0.4
    jetAlgorithm=algorithm,  # AntiKt
)

# mod["PFJets"]
process.ak4PFJetsComb = ak4PFJets.clone(
    src=pfCand,  # combPFdSA
    doAreaFastjet=True,
    rParam=jetSize,  # 0.4
    jetAlgorithm=algorithm,  # AntiKt
)


# mod['PFJetsConstituents']
process.ak4PFJetsCombConstituents = cms.EDProducer(
    "PFJetConstituentSelector",
    src=cms.InputTag(mod["PFJets"]),  # ak4PFJetsComb
    cut=cms.string(Cut),  #
)

# http://cmslxr.fnal.gov/source/PhysicsTools/PatAlgos/python/tools/jetTools.py#0896
addJetCollection(
    proc,
    labelName=mod["PATJetsLabel"],
    jetSource=cms.InputTag(mod["PFJets"]),
    postfix=postFix,
    algo=jetalgo,
    rParam=jetSize,
    jetCorrections=JEC if JEC is not None else None,
    pfCandidates=cms.InputTag(pfCand),
    svSource=cms.InputTag(svLabel),
    genJetCollection=cms.InputTag(mod["GenJetsNoNu"]),
    pvSource=cms.InputTag(pvLabel),
    muSource=cms.InputTag(muLabel),
    elSource=cms.InputTag(elLabel),
    btagDiscriminators=bTagDiscriminators,
    btagInfos=bTagInfos,
    getJetMCFlavour=GetJetMCFlavour,
    genParticles=cms.InputTag(genParticlesLabel),
    outputModules=["outputFile"],
)


####
jetSeq = cms.Sequence(
    process.ak4GenJetsNoNu + process.ak4PFJetsComb + process.ak4PFJetsCombConstituents
)
