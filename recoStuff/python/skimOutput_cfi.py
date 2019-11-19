import FWCore.ParameterSet.Config as cms

skimOutput = cms.OutputModule(
    "PoolOutputModule",
    fileName=cms.untracked.string("skimOutputLeptonJet.root"),
    outputCommands=cms.untracked.vstring(
        "drop *_*_*_*",
        "keep *_*_*_FF*",
        "keep *_TriggerResults_*_HLT",
        "keep *GenParticle*_genParticles_*_HLT",
        "keep *Track*_displacedStandAloneMuons_*_RECO",
        "keep *_generalTracks__RECO",
        "drop *TrackingRecHit*_*_*_*",
        "drop *TrackExtra*_*_*_*",
        "keep *PFCandidate*_particleFlow__RECO",
        "keep *PFJet*_ak4PFJetsCHS__RECO",
        "keep *Muon*_muons__RECO",
    ),
)

fullOutput = cms.OutputModule(
    "PoolOutputModule",
    fileName=cms.untracked.string("ffAOD.root"),
    outputCommands=cms.untracked.vstring(
        "keep *_*_*_*",
        "drop *_*_*_FF*",
    ),
)

def customizeSkimOutputContent(process, modulename='skimOutput', level=1):
    if level==1: # all intermediate products and some original content
        outcmd = cms.untracked.vstring(
            "drop *_*_*_*",
            "keep *_*_*_FF*",
            "keep *_TriggerResults_*_HLT",
            "keep *GenParticle*_genParticles_*_HLT",
            "keep *Track*_displacedStandAloneMuons_*_RECO",
            "keep *_generalTracks__RECO",
            "drop *TrackingRecHit*_*_*_*",
            "drop *TrackExtra*_*_*_*",
            "keep *PFCandidate*_particleFlow__RECO",
            "keep *PFJet*_ak4PFJetsCHS__RECO",
            "keep *Muon*_muons__RECO",
        )
    elif level==2: # minimum content to make energy scatter style event display
        outcmd = cms.untracked.vstring(
            "drop *_*_*_*",
            "keep *PFCandidate*_particleFlow__RECO",
            "keep *PFCandidate*_pfcandsFromMuondSA__FF*",
            "keep *PFCandidate*_pfNoPileUpIso_*_FF*",
            "keep *PFJet*_filteredLeptonJet_*_FF*",
            "keep *PFCandidate*_leptonjetSource*_*_FF*",
        )

    getattr(process, modulename).outputCommands = outcmd