#!/usr/bin/env python
import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

def setup_process(proc, testFileName='test.root'):

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
                options.inputFiles = 'file:/eos/uscms/store/user/wsi/MCSIDM/SIDMmumu_Mps-200_MZp-1p2_ctau-1p2/180720-105132/SIDMmumu_Mps-200_MZp-1p2_ctau-1p2_10980067_AOD.root'
            if options.year == 2018:
                options.inputFiles = 'file:/eos/uscms/store/user/wsi/MCSIDM/SIDMmumu_Mps-200_MZp-1p2_ctau-0p12/180726-222103/SIDMmumu_Mps-200_MZp-1p2_ctau-0p12_10268877_AOD.root'
        elif 'lxplus' in platform.node():
            options.inputFiles = 'file:/eos/user/w/wsi/prelimSamples/SIDMmumu_Mps-200_MZp-1p2_ctau-1_12714105_AOD.root'
        options.maxEvents = -1
        options.outputFile = testFileName
    else:
        options.maxEvents = -1


    proc.load("FWCore.MessageService.MessageLogger_cfi")
    proc.load('Configuration.StandardSequences.Services_cff')
    proc.load("Configuration.EventContent.EventContent_cff")
    proc.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi")
    proc.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi")
    proc.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi")
    proc.load("RecoMuon.DetLayers.muonDetLayerGeometry_cfi")
    proc.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
    proc.load("Configuration.StandardSequences.GeometryRecoDB_cff")
    proc.load('Configuration.StandardSequences.MagneticField_38T_cff')
    proc.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

    if options.year == 2017:
        proc.GlobalTag.globaltag = '94X_mc2017_realistic_v15'
    if options.year == 2018:
        proc.GlobalTag.globaltag = '102X_upgrade2018_realistic_v9'

    proc.load("FWCore.MessageService.MessageLogger_cfi")
    proc.MessageLogger = cms.Service("MessageLogger",
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
    proc.options = cms.untracked.PSet(
        wantSummary = cms.untracked.bool(False),
        numberOfThreads = cms.untracked.uint32(8)
    )
    proc.maxEvents = cms.untracked.PSet(
                    input = cms.untracked.int32(options.maxEvents)
                    )
    proc.source = cms.Source("PoolSource",
                    fileNames = cms.untracked.vstring(options.inputFiles)
                    )

    proc.TFileService = cms.Service("TFileService",
                    fileName = cms.string(options.outputFile),
                    closeFileFast = cms.untracked.bool(True)
                    )

    return (proc, options)
