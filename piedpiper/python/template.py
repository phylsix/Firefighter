#!/usr/bin/env python

genTemplate = """
import FWCore.ParameterSet.Config as cms

from Configuration.Generator.Pythia8CommonSettings_cfi import *
from Configuration.Generator.MCTunes2017.PythiaCP5Settings_cfi import *
from Configuration.Generator.PSweightsPythia.PythiaPSweightsSettings_cfi import *
# from Configuration.Generator.Pythia8aMCatNLOSettings_cfi import *


# External LHE producer configuration
externalLHEProducer = cms.EDProducer("ExternalLHEProducer",
    args = cms.vstring('../gridpack.tar.xz'),
    nEvents = cms.untracked.uint32(1000),
    numberOfParameters = cms.uint32(1),
    outputFile = cms.string('cmsgrid_final.lhe'),
    scriptName = cms.FileInPath('Firefighter/piedpiper/data/run_generic_tarball_cvmfs.sh')
)

# Hadronizer configuration
generator = cms.EDFilter("Pythia8HadronizerFilter",
    maxEventsToPrint = cms.untracked.int32(1),
    pythiaPylistVerbosity = cms.untracked.int32(1),
    filterEfficiency = cms.untracked.double(1.0),
    pythiaHepMCVerbosity = cms.untracked.bool(False),
    comEnergy = cms.double(13000.),
    PythiaParameters = cms.PSet(
        pythia8CommonSettingsBlock,
        pythia8CP5SettingsBlock,
        pythia8PSweightsSettingsBlock,
        # pythia8aMCatNLOSettingsBlock,
        # JetMatchingParameters = cms.vstring(
        #     'JetMatching:setMad = off',
        #     'JetMatching:scheme = 1',
        #     'JetMatching:merge = on',
        #     'JetMatching:jetAlgorithm = 2',
        #     'JetMatching:etaJetMax = 5.',
        #     'JetMatching:coneRadius = 1.',
        #     'JetMatching:slowJetPower = 1',
        #     'JetMatching:qCut = 19.', #this is the actual merging scale
        #     'JetMatching:nQmatch = 5', #5 for 5-flavour scheme (matching of b-quarks)
        #     'JetMatching:nJetMax = 1', #number of partons in born matrix element for highest multiplicity
        #     'JetMatching:doShowerKt = off', #off for MLM matching, turn on for shower-kT matching
        # ),
        processParameters = cms.vstring(
            'ParticleDecays:tau0Max = 1000.1',
            'LesHouches:setLifetime = 2',
            '32:tau0 = {CTAU}'
        ),
        parameterSets = cms.vstring('pythia8CommonSettings',
                                    'pythia8CP5Settings',
                                    'pythia8PSweightsSettings',
                                    # 'pythia8aMCatNLOSettings',
                                    # 'JetMatchingParameters',
                                    'processParameters',
                                    )
    )
)

genParticlesForFilter = cms.EDProducer(
    "GenParticleProducer",
    saveBarCodes=cms.untracked.bool(True),
    src=cms.InputTag("generator", "unsmeared"),
    abortOnUnknownPDGCode=cms.untracked.bool(False)
)

genfilter = cms.EDFilter(
    "GenParticleSelector",
    src = cms.InputTag("genParticlesForFilter"),
    cut = cms.string(' && '.join([
        '(abs(pdgId)==11 || abs(pdgId)==13)',
        'abs(eta)<2.4',
        '(vertex.rho<740. && abs(vertex.Z)<960.)',
        'pt>5.',
        'isLastCopy()',
        'isPromptFinalState()',
        'fromHardProcessFinalState()',
    ]))
)
gencount = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("genfilter"),
    minNumber = cms.uint32(4)
)

ProductionFilterSequence = cms.Sequence(
    generator * (genParticlesForFilter + genfilter + gencount)
)
"""

singlecrabConfigGEN = """
nametag: {NT}
totaljobs: {NJ}
"""

singlecrabConfigRECO = """
dataset: {DS}
nametag: {NT}
"""
