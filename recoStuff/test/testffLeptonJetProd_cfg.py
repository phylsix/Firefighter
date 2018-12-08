import FWCore.ParameterSet.Config as cms

process = cms.Process("USER")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.Services_cff')
process.load("Configuration.EventContent.EventContent_cff")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')


process.GlobalTag.globaltag = '94X_mc2017_realistic_v15'

process.MessageLogger = cms.Service(
    "MessageLogger",
    destinations = cms.untracked.vstring('joblog', 'cerr'),
    joblog = cms.untracked.PSet(
        threshold = cms.untracked.string('ERROR')
    )
)

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False),
    numberOfThreads = cms.untracked.uint32(8),
    numberOfStreams = cms.untracked.uint32(0)
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
        'file:AODSIM.root',
    )
 )


process.skimOutput = cms.OutputModule(
    "PoolOutputModule",
    fileName=cms.untracked.string('skimOutputLeptonJetProd.root'),
    outputCommands = cms.untracked.vstring(
        'drop *_*_*_*',
        'keep *_*_*_USER',
        'keep recoGenParticles_genParticles*_*_*',
        'keep recoPFCandidates_particleFlow_*_RECO',
        'keep recoMuons_muons__RECO',
        'keep recoTracks_generalTracks_*_RECO',
        'keep recoTracks_*tandAloneMuons_*_RECO',
        'keep recoTrackExtras_*tandAloneMuons_*_RECO',
    )
)

process.load('Firefighter.recoStuff.ffDsaPFCandMergeCluster_cff')
process.leptonjet_step = cms.Path(process.ffLeptonJetSeq)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.output_step = cms.EndPath(process.skimOutput)


process.schedule = cms.Schedule(
    process.leptonjet_step,
    process.endjob_step,
    process.output_step
)
