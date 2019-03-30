import FWCore.ParameterSet.Config as cms

skimOutput = cms.OutputModule(
    "PoolOutputModule",
    fileName=cms.untracked.string('skimOutputLeptonJet.root'),
    outputCommands=cms.untracked.vstring(
        'drop *_*_*_*', 'keep *_*_*_FF', 'keep *_TriggerResults_*_HLT',
        'keep *GenParticle*_genParticles_*_HLT',
        'keep *Track*_displacedStandAloneMuons_*_RECO',
        'drop *TrackingRecHit*_*_*_*', 'drop *TrackExtra*_*_*_*',
        'keep *PFCandidate*_particleFlow__RECO'))
