import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

import os

from LeptonJet.ntupleBuilder.SelectTrack_cfi               import goodDsaMuon
from LeptonJet.ntupleBuilder.TrackPFCandidateProducer_cfi  import trackPFCand
from LeptonJet.ntupleBuilder.ForkCandAgainstOthersByDR_cfi import forkcandagainstothersbydr
from LeptonJet.ntupleBuilder.CandMerger_cfi                import candmerger

from JMEAnalysis.JetToolbox.jetToolbox_cff                 import jetToolbox
from Firefighter.washAOD.pfJetAnalysis_cfi                 import pfjetana

process = cms.Process("USER")
options = VarParsing.VarParsing('analysis')
options.register('test',
                 0,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Run for a test or not")
options.register('year',
                 2017,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "sample of the year")

options.parseArguments()

if options.test:
    import platform
    if 'cmslpc' in platform.node():
        if options.year == 2017:
            options.inputFiles = 'root://cmseos.fnal.gov///store/user/wsi/MCSIDM/SIDMmumu_Mps-200_MZp-1p2_ctau-1p2/180720-105132/SIDMmumu_Mps-200_MZp-1p2_ctau-1p2_10980067_AOD.root'
        if options.year == 2018:
           options.inputFiles = 'file:/eos/uscms/store/user/wsi/MCSIDM/SIDMmumu_Mps-200_MZp-1p2_ctau-0p12/180726-222103/SIDMmumu_Mps-200_MZp-1p2_ctau-0p12_10268877_AOD.root'
    elif 'lxplus' in platform.node():
        options.inputFiles = 'file:/eos/user/w/wsi/prelimSamples/SIDMmumu_Mps-200_MZp-1p2_ctau-1_12714105_AOD.root'
    options.maxEvents = -1
    options.outputFile = 'jetTuplizer.root'
else:
    options.maxEvents = -1

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.Services_cff')
process.load("Configuration.EventContent.EventContent_cff")
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
if options.year == 2017:
    process.GlobalTag.globaltag = '94X_mc2017_realistic_v15'
if options.year == 2018:
    process.GlobalTag.globaltag = '102X_upgrade2018_realistic_v9'

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger = cms.Service("MessageLogger",
     destinations   =  cms.untracked.vstring('messages', 'cerr'),
     debugModules   = cms.untracked.vstring('*'),
     categories     = cms.untracked.vstring('FwkReport'),
     messages       = cms.untracked.PSet(
                        extension = cms.untracked.string('.txt'),
                        threshold =  cms.untracked.string('ERROR')
                    ),
     cerr           = cms.untracked.PSet(
                        FwkReport = cms.untracked.PSet(
                            reportEvery = cms.untracked.int32(10000)
                        )
                    )
    )
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False),
    numberOfThreads = cms.untracked.uint32(8)
)
process.maxEvents = cms.untracked.PSet(
                input = cms.untracked.int32(options.maxEvents)
                )
process.source = cms.Source("PoolSource",
                fileNames = cms.untracked.vstring(options.inputFiles)
                )

process.TFileService = cms.Service("TFileService",
                fileName = cms.string(options.outputFile),
                closeFileFast = cms.untracked.bool(True)
                )

process.selectedDsaMuons = goodDsaMuon.clone()
process.dSAmuPFCand      = trackPFCand.clone(src=cms.InputTag("selectedDsaMuons"))
process.dSAmuPFCandFork  = forkcandagainstothersbydr.clone()
process.combPFdSA        = candmerger.clone()

process.mergePFdSASeq = cms.Sequence(
    process.selectedDsaMuons +
    process.dSAmuPFCand +
    process.dSAmuPFCandFork +
    process.combPFdSA
)

process.p1 = cms.Path( process.mergePFdSASeq )
# AK4
jetToolbox( process, 'ak4', 'jetSequence', 'out',
            PUMethod='Plain', runOnMC=True, miniAOD=False, postFix='Comb',
            newPFCollection = True, nameNewPFCollection = 'combPFdSA',
            # addSoftDrop=True, addPruning=True, addTrimming=True, saveJetCollection=True,
            verbosity=0 )
# AK2
jetToolbox( process, 'ak2', 'jetSequence', 'out',
            PUMethod='Plain', runOnMC=True, miniAOD=False, postFix='Comb',
            newPFCollection = True, nameNewPFCollection = 'combPFdSA',
            verbosity=0 )
# AK1
jetToolbox( process, 'ak1', 'jetSequence', 'out',
            PUMethod='Plain', runOnMC=True, miniAOD=False, postFix='Comb',
            newPFCollection = True, nameNewPFCollection = 'combPFdSA',
            verbosity=0 )

process.ak4PfJet = pfjetana.clone( src=cms.InputTag("ak4PFJetsComb", "", "USER") )
process.ak2PfJet = pfjetana.clone( src=cms.InputTag("ak2PFJetsComb", "", "USER") )
process.ak1PfJet = pfjetana.clone( src=cms.InputTag("ak1PFJetsComb", "", "USER") )

process.p2 = cms.Path(
    process.ak4PfJet +
    process.ak2PfJet +
    process.ak1PfJet
)