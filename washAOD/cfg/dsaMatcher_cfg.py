import FWCore.ParameterSet.Config as cms
from Firefighter.washAOD.setupProcess_cff import setup_process

process = cms.Process("USER")
process = setup_process(process,
                        testFileName='dsaMatcher.root')[0]

process.dsaMatcher = cms.EDAnalyzer("dsaMatching")
process.p = cms.Path( process.dsaMatcher )
