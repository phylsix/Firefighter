import FWCore.ParameterSet.Config as cms

from Configuration.Generator.Pythia8CommonSettings_cfi import *
from Configuration.Generator.MCTunes2017.PythiaCP5Settings_cfi import *
from Configuration.Generator.Pythia8aMCatNLOSettings_cfi import *

generator = cms.EDFilter(
    "Pythia8GeneratorFilter",
    maxEventsToPrint=cms.untracked.int32(1),
    pythiaPylistVerbosity=cms.untracked.int32(1),
    filterEfficiency=cms.untracked.double(1.0),
    pythiaHepMCVerbosity=cms.untracked.bool(False),
    comEnergy=cms.double(13000.0),
    RandomizedParameters=cms.VPSet(),
)

from .scanning_points import points

for point in points:
    basePythiaParameters = cms.PSet(
        pythia8CommonSettingsBlock,
        pythia8CP5SettingsBlock,
        pythia8aMCatNLOSettingsBlock,
        processParameters=cms.vstring(point["procParam"]),
        parameterSets=cms.vstring(
            "pythia8CommonSettings",
            "pythia8CP5Settings",
            "pythia8aMCatNLOSettings",
            "processParameters",
        ),
    )

    generator.RandomizedParameters.append(
        cms.PSet(
            ConfigWeight=cms.double(point["weight"]),
            ConfigDescription=cms.string(point["name"]),
            GridpackPath=cms.string(point["gridpack"]),
            PythiaParameters=basePythiaParameters,
        )
    )

genParticlesForFilter = cms.EDProducer(
    "GenParticleProducer",
    saveBarCodes=cms.untracked.bool(True),
    src=cms.InputTag("generator", "unsmeared"),
    abortOnUnknownPDGCode=cms.untracked.bool(False),
)

genfilter = cms.EDFilter(
    "GenParticleSelector",
    src=cms.InputTag("genParticlesForFilter"),
    cut=cms.string(
        " && ".join(
            [
                "(abs(pdgId)==11 || abs(pdgId)==13)",
                "abs(eta)<2.4",
                "(vertex.rho<740. && abs(vertex.Z)<960.)",
                "pt>5.",
                "isLastCopy()",
                "isPromptFinalState()",
                "fromHardProcessFinalState()",
            ]
        )
    ),
)
gencount = cms.EDFilter("CandViewCountFilter",
                        src=cms.InputTag("genfilter"),
                        minNumber=cms.uint32(4))

ProductionFilterSequence = cms.Sequence(
    generator * (genParticlesForFilter + genfilter + gencount)
)
